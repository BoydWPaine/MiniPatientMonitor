#include "net_thread.h"

#include "proto/encoder.h"
#include "osal/osal.h"

#include <QByteArray>

namespace mpm::host {

NetThread::NetThread(QObject* parent) : QThread(parent) {}

NetThread::~NetThread()
{
    requestStop();
    wait();
}

void NetThread::requestStop()
{
    stop_requested_.store(true);
}

void NetThread::requestNibp()
{
    std::lock_guard<std::mutex> lock(request_mutex_);
    pending_nibp_request_ = true;
}

int NetThread::connectWithRetry()
{
    for (uint32_t attempt = 1; attempt <= 50 && !stop_requested_.load(); ++attempt) {
        const osal_socket_t socket = osal_tcp_connect(MPM_TCP_HOST, static_cast<uint16_t>(MPM_TCP_PORT));
        if (socket != OSAL_SOCKET_INVALID) {
            return socket;
        }
        osal_thread_sleep_ms(100);
    }
    return OSAL_SOCKET_INVALID;
}

bool NetThread::sendPendingRequests(int socket_fd)
{
    std::lock_guard<std::mutex> lock(request_mutex_);
    if (!pending_nibp_request_) {
        return true;
    }

    monitor::Envelope envelope;
    envelope.mutable_heartbeat()->set_timestamp_ms(osal_get_tick_ms());
    envelope.mutable_nibp_request();
    if (!mpm::proto::write_frame(socket_fd, envelope)) {
        return false;
    }

    pending_nibp_request_ = false;
    return true;
}

void NetThread::run()
{
    if (!osal_tcp_init()) {
        emit connectionError(QStringLiteral("TCP init failed"));
        emit connectionStateChanged(false);
        return;
    }

    while (!stop_requested_.load()) {
        const osal_socket_t socket = connectWithRetry();
        if (socket == OSAL_SOCKET_INVALID) {
            emit connectionError(QStringLiteral("connect timeout"));
            emit connectionStateChanged(false);
            osal_thread_sleep_ms(500);
            continue;
        }

        emit connectionStateChanged(true);

        bool connected = true;
        while (!stop_requested_.load() && connected) {
            if (!sendPendingRequests(socket)) {
                connected = false;
                break;
            }

            if (osal_tcp_wait_readable(socket, 5)) {
                monitor::Envelope envelope;
                if (!mpm::proto::read_frame(socket, envelope)) {
                    emit connectionError(QStringLiteral("read_frame failed"));
                    connected = false;
                    break;
                }

                std::string bytes;
                if (envelope.SerializeToString(&bytes)) {
                    emit envelopeReceived(QByteArray(bytes.data(), static_cast<int>(bytes.size())));
                }
            }
        }

        osal_tcp_close(socket);
        emit connectionStateChanged(false);

        if (!stop_requested_.load()) {
            osal_thread_sleep_ms(500);
        }
    }

    osal_tcp_shutdown();
}

}  // namespace mpm::host