#ifndef MPM_HOST_NET_THREAD_H
#define MPM_HOST_NET_THREAD_H

#include <QByteArray>
#include <QThread>
#include <atomic>
#include <mutex>

namespace mpm::host {

class NetThread : public QThread {
    Q_OBJECT

public:
    explicit NetThread(QObject* parent = nullptr);
    ~NetThread() override;

    void requestStop();
    void requestNibp();

signals:
    void envelopeReceived(const QByteArray& payload);
    void connectionStateChanged(bool connected);
    void connectionError(const QString& message);

protected:
    void run() override;

private:
    int connectWithRetry();
    bool sendPendingRequests(int socket_fd);

    std::atomic<bool> stop_requested_{false};
    std::mutex request_mutex_;
    bool pending_nibp_request_ = false;
};

}  // namespace mpm::host

#endif