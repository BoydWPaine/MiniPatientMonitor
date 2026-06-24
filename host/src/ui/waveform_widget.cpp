#include "ui/waveform_widget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTimer>

namespace mpm::host {

WaveformWidget::WaveformWidget(const QColor& color, QWidget* parent)
    : QWidget(parent), color_(color)
{
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::black);
    setPalette(palette);

    resetColumns();

    refresh_timer_ = new QTimer(this);
    connect(refresh_timer_, &QTimer::timeout, this, &WaveformWidget::onRefresh);
    refresh_timer_->start(10);
}

void WaveformWidget::addSample(int32_t sample)
{
    pending_samples_.push_back(sample);
}

void WaveformWidget::clearSamples()
{
    pending_samples_.clear();
    draw_x_ = 0;
    resetColumns();
    update();
}

void WaveformWidget::resetColumns()
{
    const int w = std::max(width(), 1);
    column_y_.assign(static_cast<size_t>(w), 0);
    column_valid_.assign(static_cast<size_t>(w), false);
}

void WaveformWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    resetColumns();
    draw_x_ = 0;
}

int WaveformWidget::sampleToPixelY(int32_t sample) const
{
    const double y_scale = static_cast<double>(height() - 8) / (2.0 * 2048.0);
    const double y_center = static_cast<double>(height()) / 2.0;
    return static_cast<int>(y_center - (static_cast<double>(sample) * y_scale));
}

void WaveformWidget::advanceSweep(int32_t sample)
{
    const int w = width();
    if (w <= 0) {
        return;
    }

    if (static_cast<int>(column_y_.size()) != w) {
        resetColumns();
    }

    const int erase_x = (draw_x_ + kSweepGap) % w;
    column_valid_[static_cast<size_t>(erase_x)] = false;

    column_y_[static_cast<size_t>(draw_x_)] = sampleToPixelY(sample);
    column_valid_[static_cast<size_t>(draw_x_)] = true;

    draw_x_ = (draw_x_ + 1) % w;
}

void WaveformWidget::onRefresh()
{
    if (pending_samples_.empty()) {
        return;
    }

    const int32_t sample = pending_samples_.front();
    pending_samples_.pop_front();
    advanceSweep(sample);
    update();
}

void WaveformWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    const int w = width();
    if (w <= 1 || static_cast<int>(column_y_.size()) != w) {
        return;
    }

    QPen pen(color_);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (int x = 0; x < w - 1; ++x) {
        if (!column_valid_[static_cast<size_t>(x)] ||
            !column_valid_[static_cast<size_t>(x + 1)]) {
            continue;
        }
        painter.drawLine(x,
                         column_y_[static_cast<size_t>(x)],
                         x + 1,
                         column_y_[static_cast<size_t>(x + 1)]);
    }
}

}  // namespace mpm::host