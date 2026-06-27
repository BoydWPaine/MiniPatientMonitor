#include "ui/waveform_widget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTimer>

#include <algorithm>

namespace mpm::host {

WaveformWidget::WaveformWidget(const QColor& color,
                               const QString& label,
                               const bool show_ecg_scale,
                               QWidget* parent)
    : QWidget(parent),
      color_(color),
      label_text_(label),
      show_ecg_scale_(show_ecg_scale),
      trace_end_x_(show_ecg_scale ? kTraceEndXEcg : kTraceEndXDefault),
      amplitude_range_(show_ecg_scale ? kEcgAmplitudeRange : kDefaultAmplitudeRange)
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
    draw_index_ = 0;
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
    draw_index_ = 0;
}

bool WaveformWidget::isTraceColumn(const int x) const
{
    return x >= kTraceStartX && x <= trace_end_x_;
}

int WaveformWidget::sampleToPixelY(int32_t sample) const
{
    const int32_t clamped = std::max(-amplitude_range_, std::min(amplitude_range_, sample));
    const double y_scale =
        static_cast<double>(height() - 8) / (2.0 * static_cast<double>(amplitude_range_));
    const double y_center = static_cast<double>(height()) / 2.0;
    return static_cast<int>(y_center - (static_cast<double>(clamped) * y_scale));
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

    const int trace_width = trace_end_x_ - kTraceStartX + 1;
    const int erase_offset = (draw_index_ + kSweepGap) % trace_width;
    const int erase_x = kTraceStartX + erase_offset;
    const int draw_x = kTraceStartX + draw_index_;

    column_valid_[static_cast<size_t>(erase_x)] = false;

    column_y_[static_cast<size_t>(draw_x)] = sampleToPixelY(sample);
    column_valid_[static_cast<size_t>(draw_x)] = true;

    draw_index_ = (draw_index_ + 1) % trace_width;
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

void WaveformWidget::drawOverlay(QPainter& painter) const
{
    QFont label_font = painter.font();
    label_font.setBold(true);
    label_font.setPointSize(14);
    painter.setFont(label_font);
    painter.setPen(color_);
    painter.drawText(QRect(0, 0, kLabelWidth, kLabelHeight),
                     Qt::AlignCenter,
                     label_text_);

    if (!show_ecg_scale_) {
        return;
    }

    painter.fillRect(kScaleLineX, 0, kScaleLineWidth, height(), color_);

    QFont unit_font = painter.font();
    unit_font.setBold(true);
    unit_font.setPointSize(9);
    painter.setFont(unit_font);
    painter.drawText(QRect(kScaleUnitX,
                           kScaleUnitY,
                           kScaleUnitWidth,
                           kScaleUnitHeight),
                     Qt::AlignCenter,
                     QStringLiteral("1mV"));
}

void WaveformWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    const int w = width();
    if (w <= 1 || static_cast<int>(column_y_.size()) != w) {
        drawOverlay(painter);
        return;
    }

    QPen pen(color_);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (int x = kTraceStartX; x < trace_end_x_; ++x) {
        if (!isTraceColumn(x) || !isTraceColumn(x + 1)) {
            continue;
        }
        if (!column_valid_[static_cast<size_t>(x)] ||
            !column_valid_[static_cast<size_t>(x + 1)]) {
            continue;
        }
        painter.drawLine(x,
                         column_y_[static_cast<size_t>(x)],
                         x + 1,
                         column_y_[static_cast<size_t>(x + 1)]);
    }

    drawOverlay(painter);
}

}  // namespace mpm::host