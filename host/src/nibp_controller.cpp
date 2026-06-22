#include "nibp_controller.h"

#include "net_thread.h"

#include <QTimer>

namespace mpm::host {

NibpController::NibpController(NetThread* net_thread, QObject* parent)
    : QObject(parent), net_thread_(net_thread)
{
    stat_timer_ = new QTimer(this);
    stat_timer_->setInterval(stat_interval_sec_ * 1000);
    connect(stat_timer_, &QTimer::timeout, this, &NibpController::onStatTimeout);
}

void NibpController::setStatIntervalSec(int seconds)
{
    stat_interval_sec_ = seconds > 0 ? seconds : 300;
    stat_timer_->setInterval(stat_interval_sec_ * 1000);
}

int NibpController::statIntervalSec() const
{
    return stat_interval_sec_;
}

void NibpController::triggerManualMeasure()
{
    if (net_thread_ != nullptr) {
        net_thread_->requestNibp();
        emit measureRequested();
    }
}

void NibpController::startStatTimer()
{
    stat_timer_->start();
}

void NibpController::stopStatTimer()
{
    stat_timer_->stop();
}

void NibpController::onStatTimeout()
{
    triggerManualMeasure();
}

}  // namespace mpm::host