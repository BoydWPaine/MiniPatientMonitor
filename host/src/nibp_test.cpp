#include "proto/encoder.h"
#include "osal/osal.h"

#include <cstdio>

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
        return 1;
    }

    monitor::Envelope request;
    request.mutable_heartbeat()->set_timestamp_ms(osal_get_tick_ms());
    request.mutable_nibp_request();
    if (!mpm::proto::write_frame(socket, request)) {
        return 1;
    }

    monitor::Envelope response;
    if (!mpm::proto::read_frame(socket, response)) {
        return 1;
    }

    const bool ok = response.payload_case() == monitor::Envelope::kNibp &&
                    response.nibp().nibp_sys() > 0;
    if (ok) {
        std::printf("nibp=%u/%u (%u)\n",
                    response.nibp().nibp_sys(),
                    response.nibp().nibp_dia(),
                    response.nibp().nibp_mean());
    }

    osal_tcp_close(socket);
    osal_tcp_shutdown();
    return ok ? 0 : 1;
}