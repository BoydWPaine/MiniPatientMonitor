#include "net_server.h"

#include "proto/encoder.h"
#include "osal/osal.h"

#include <cstdio>

namespace mpm::device {
namespace {

monitor::Envelope make_heartbeat_envelope()
{
    monitor::Envelope envelope;
    envelope.mutable_heartbeat()->set_timestamp_ms(osal_get_tick_ms());
    envelope.mutable_null_packet();
    return envelope;
}

}  // namespace

int run_server(const char* host, uint16_t port)
{
    if (!osal_tcp_init()) {
        std::fprintf(stderr, "[device] TCP init failed\n");
        return 1;
    }

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

    uint32_t frames_sent = 0;
    while (true) {
        const monitor::Envelope envelope = make_heartbeat_envelope();
        if (!mpm::proto::write_frame(client, envelope)) {
            std::fprintf(stderr, "[device] send failed after %u frames\n", frames_sent);
            break;
        }

        ++frames_sent;
        if (frames_sent == 1 || (frames_sent % 10) == 0) {
            std::printf("[device] sent heartbeat frame #%u (ts=%llu)\n",
                        frames_sent,
                        static_cast<unsigned long long>(envelope.heartbeat().timestamp_ms()));
        }

        osal_thread_sleep_ms(1000);
    }

    osal_tcp_close(client);
    osal_tcp_shutdown();
    return 0;
}

}  // namespace mpm::device