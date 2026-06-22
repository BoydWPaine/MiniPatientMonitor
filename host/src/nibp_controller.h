#ifndef MPM_HOST_NIBP_CONTROLLER_H
#define MPM_HOST_NIBP_CONTROLLER_H

#include <QObject>
#include <QTimer>

namespace mpm::host {

class NetThread;

class NibpController : public QObject {
    Q_OBJECT

public:
    explicit NibpController(NetThread* net_thread, QObject* parent = nullptr);

    void setStatIntervalSec(int seconds);
    int statIntervalSec() const;

public slots:
    void triggerManualMeasure();
    void startStatTimer();
    void stopStatTimer();

signals:
    void measureRequested();

private slots:
    void onStatTimeout();

private:
    NetThread* net_thread_ = nullptr;
    QTimer* stat_timer_ = nullptr;
    int stat_interval_sec_ = 300;
};

}  // namespace mpm::host

#endif