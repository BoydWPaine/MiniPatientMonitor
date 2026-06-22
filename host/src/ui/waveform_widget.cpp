#include "ui/waveform_widget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <vector>

namespace mpm::host {

WaveformWidget::WaveformWidget(const QColor& color, QWidget* parent)
    : QWidget(parent), color_(color)
{
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::black);
    setPalette(palette);

    refresh_timer_ = new QTimer(this);
    connect(refresh_timer_, &QTimer::timeout, this, &WaveformWidget::onRefresh);
    refresh_timer_->start(40);
}

void WaveformWidget::addSample(int32_t sample)
{
    buffer_.push(sample);
}

void WaveformWidget::clearSamples()
{
    buffer_.clear();
    update();
}

void WaveformWidget::onRefresh()
{
    update();
}

void WaveformWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    std::vector<int32_t> samples;
    buffer_.copy_ordered(samples);
    if (samples.size() < 2) {
        return;
    }

    const double x_step = static_cast<double>(width() - 1) / static_cast<double>(kRingBufferCapacity - 1);
    const double y_scale = static_cast<double>(height() - 8) / (2.0 * 2048.0);
    const double y_center = static_cast<double>(height()) / 2.0;

    QPen pen(color_);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPointF previous;
    for (size_t i = 0; i < samples.size(); ++i) {
        const double x = static_cast<double>(kRingBufferCapacity - samples.size() + i) * x_step;
        const double y = y_center - (static_cast<double>(samples[i]) * y_scale);
        const QPointF point(x, y);
        if (i > 0) {
            painter.drawLine(previous, point);
        }
        previous = point;
    }
}

}  // namespace mpm::host