#include "net_server.h"

#include "device_config.h"
#include "ecg_sim.h"
#include "nibp_sim.h"
#include "sim_common.h"
#include "proto/encoder.h"
#include "resp_sim.h"
#include "spo2_sim.h"
#include "temp_sim.h"
#include "osal/osal.h"

#include <cstdio>

namespace mpm::device {
namespace {

monitor::Envelope make_envelope()
{
    monitor::Envelope envelope;
    envelope.mutable_heartbeat()->set_timestamp_ms(osal_get_tick_ms());
    return envelope;
}

bool send_packet(osal_socket_t client, monitor::Envelope& envelope)
{
    envelope.mutable_heartbeat()->set_timestamp_ms(osal_get_tick_ms());
    return mpm::proto::write_frame(client, envelope);
}

void handle_incoming(osal_socket_t client,
                     DeviceConfigStore& config_store,
                     NibpSim& nibp_sim)
{
    if (!osal_tcp_wait_readable(client, 0)) {
        return;
    }

    monitor::Envelope request;
    if (!mpm::proto::read_frame(client, request)) {
        return;
    }

    if (request.payload_case() != monitor::Envelope::kNibpRequest) {
        return;
    }

    const DeviceConfig cfg = config_store.snapshot();
    if (!cfg.nibp_enabled) {
        return;
    }

    monitor::Envelope response = make_envelope();
    *response.mutable_nibp() = nibp_sim.measure();
    if (!send_packet(client, response)) {
        std::fprintf(stderr, "[device] failed to send NibpPacket\n");
    } else {
        std::printf("[device] sent NibpPacket in response to NibpRequest\n");
    }
}

}  // namespace

int run_server(const char* host,
               uint16_t port,
               DeviceConfigStore& config_store,
               bool disable_spo2)
{
    if (!osal_tcp_init()) {
        std::fprintf(stderr, "[device] TCP init failed\n");
        return 1;
    }

    if (disable_spo2) {
        DeviceConfig cfg = config_store.snapshot();
        cfg.spo2_enabled = false;
        config_store.update(cfg);
        std::printf("[device] SpO2 module disabled (TC-IT-09 test mode)\n");
    }

    EcgSim ecg_sim(config_store);
    Spo2Sim spo2_sim(config_store);
    RespSim resp_sim(config_store);
    TempSim temp_sim(config_store);
    NibpSim nibp_sim(config_store);

    const osal_socket_t listener = osal_tcp_listen(host, port);
    if (listener == OSAL_SOCKET_INVALID) {
        std::fprintf(stderr, "[device] listen failed on %s:%u\n", host, port);
        osal_tcp_shutdown();
        return 1;
    }

    std::printf("[device] listening on %s:%u\n", host, port);

    const osal_socket_t client = osal_tcp_accept(listener);
    if (client == OSAL_SOCKET_INVALID) {
        std::fprintf(stderr, "[device] accept failed\n");
        osal_tcp_close(listener);
        osal_tcp_shutdown();
        return 1;
    }

    std::printf("[device] host connected\n");
    osal_tcp_close(listener);

    uint32_t vital_frames = 0;
    uint32_t heartbeat_frames = 0;
    uint32_t next_vital_ms = osal_get_tick_ms();
    uint32_t next_heartbeat_ms = next_vital_ms + 1000U;

    while (true) {
        handle_incoming(client, config_store, nibp_sim);

        const uint32_t now = osal_get_tick_ms();
        const DeviceConfig cfg = config_store.snapshot();

        if (now >= next_vital_ms) {
            if (cfg.ecg_enabled) {
                monitor::Envelope envelope = make_envelope();
                *envelope.mutable_ecg() = ecg_sim.next_packet();
                if (!send_packet(client, envelope)) {
                    break;
                }
            }

            if (cfg.spo2_enabled) {
                monitor::Envelope envelope = make_envelope();
                *envelope.mutable_spo2() = spo2_sim.next_packet();
                if (!send_packet(client, envelope)) {
                    break;
                }
            }

            if (cfg.resp_enabled) {
                monitor::Envelope envelope = make_envelope();
                *envelope.mutable_resp() = resp_sim.next_packet();
                if (!send_packet(client, envelope)) {
                    break;
                }
            }

            if (cfg.temp_enabled) {
                monitor::Envelope envelope = make_envelope();
                *envelope.mutable_temp() = temp_sim.next_packet();
                if (!send_packet(client, envelope)) {
                    break;
                }
            }

            ++vital_frames;
            next_vital_ms += kSamplePeriodMs;
            if (now >= next_vital_ms) {
                next_vital_ms = now + kSamplePeriodMs;
            }
        }

        if (now >= next_heartbeat_ms) {
            monitor::Envelope envelope = make_envelope();
            envelope.mutable_null_packet();
            if (!send_packet(client, envelope)) {
                break;
            }
            ++heartbeat_frames;
            next_heartbeat_ms += 1000U;
            if (now >= next_heartbeat_ms) {
                next_heartbeat_ms = now + 1000U;
            }
        }

        const uint32_t until_vital =
            next_vital_ms > now ? next_vital_ms - now : 0U;
        const uint32_t until_heartbeat =
            next_heartbeat_ms > now ? next_heartbeat_ms - now : 0U;
        const uint32_t sleep_ms = until_vital < until_heartbeat ? until_vital : until_heartbeat;
        if (sleep_ms > 0U) {
            osal_thread_sleep_ms(sleep_ms > 5U ? sleep_ms - 1U : 1U);
        }
    }

    std::fprintf(stderr,
                 "[device] connection closed after %u vital cycles, %u heartbeats\n",
                 vital_frames,
                 heartbeat_frames);
    osal_tcp_close(client);
    osal_tcp_shutdown();
    return 0;
}

}  // namespace mpm::device