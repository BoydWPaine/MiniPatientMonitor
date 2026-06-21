#include "net_client.h"

#include "proto/encoder.h"
#include "osal/osal.h"

#include <cstdio>

namespace mpm::host {

int run_client(const char* host, uint16_t port, uint32_t max_frames)
{
    if (!osal_tcp_init()) {
        std::fprintf(stderr, "[host] TCP init failed\n");
        return 1;
    }

    osal_socket_t socket = OSAL_SOCKET_INVALID;
    for (uint32_t attempt = 1; attempt <= 50; ++attempt) {
        socket = osal_tcp_connect(host, port);
        if (socket != OSAL_SOCKET_INVALID) {
            break;
        }

        std::printf("[host] connect attempt %u failed, retrying...\n", attempt);
        osal_thread_sleep_ms(100);
    }

    if (socket == OSAL_SOCKET_INVALID) {
        std::fprintf(stderr, "[host] connect failed to %s:%u\n", host, port);
        osal_tcp_shutdown();
        return 1;
    }

    std::printf("[host] connected to %s:%u\n", host, port);

    uint32_t frames_received = 0;
    while (max_frames == 0 || frames_received < max_frames) {
        monitor::Envelope envelope;
        if (!mpm::proto::read_frame(socket, envelope)) {
            std::fprintf(stderr, "[host] recv failed after %u frames\n", frames_received);
            break;
        }

        ++frames_received;
        std::printf("[host] frame #%u heartbeat ts=%llu payload=%s\n",
                    frames_received,
                    static_cast<unsigned long long>(envelope.heartbeat().timestamp_ms()),
                    envelope.payload_case() == monitor::Envelope::kNullPacket ? "NullPacket"
                                                                              : "other");
    }

    osal_tcp_close(socket);
    osal_tcp_shutdown();

    if (frames_received == 0) {
        return 1;
    }

    std::printf("[host] received %u frame(s), connection OK\n", frames_received);
    return 0;
}

}  // namespace mpm::host