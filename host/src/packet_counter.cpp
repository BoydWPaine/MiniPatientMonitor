// Standalone packet counter for M2 integration checks (not linked by default).
#include "proto/encoder.h"
#include "osal/osal.h"

#include <cstdio>
#include <cstring>

int main()
{
    if (!osal_tcp_init()) {
        return 1;
    }

    osal_socket_t socket = OSAL_SOCKET_INVALID;
    for (uint32_t attempt = 1; attempt <= 50; ++attempt) {
        socket = osal_tcp_connect(MPM_TCP_HOST, static_cast<uint16_t>(MPM_TCP_PORT));
        if (socket != OSAL_SOCKET_INVALID) {
            break;
        }
        osal_thread_sleep_ms(100);
    }

    if (socket == OSAL_SOCKET_INVALID) {
        std::fprintf(stderr, "connect failed\n");
        return 1;
    }

    uint32_t ecg = 0;
    uint32_t spo2 = 0;
    uint32_t resp = 0;
    uint32_t temp = 0;
    const uint32_t start_ms = osal_get_tick_ms();

    while (osal_get_tick_ms() - start_ms < 10000U) {
        monitor::Envelope envelope;
        if (!mpm::proto::read_frame(socket, envelope)) {
            break;
        }

        switch (envelope.payload_case()) {
        case monitor::Envelope::kEcg:
            ++ecg;
            break;
        case monitor::Envelope::kSpo2:
            ++spo2;
            break;
        case monitor::Envelope::kResp:
            ++resp;
            break;
        case monitor::Envelope::kTemp:
            ++temp;
            break;
        default:
            break;
        }
    }

    std::printf("ecg=%u spo2=%u resp=%u temp=%u\n", ecg, spo2, resp, temp);
    osal_tcp_close(socket);
    osal_tcp_shutdown();
    return (ecg >= 250 && spo2 >= 250 && resp >= 250 && temp >= 250) ? 0 : 1;
}