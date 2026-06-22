#ifndef MPM_HOST_WAVEFORM_WIDGET_H
#define MPM_HOST_WAVEFORM_WIDGET_H

#include "ring_buffer.h"

#include <QColor>
#include <QWidget>

class QTimer;

namespace mpm::host {

class WaveformWidget : public QWidget {
    Q_OBJECT

public:
    explicit WaveformWidget(const QColor& color, QWidget* parent = nullptr);

    void addSample(int32_t sample);
    void clearSamples();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onRefresh();

private:
    RingBuffer<kRingBufferCapacity> buffer_;
    QColor color_;
    QTimer* refresh_timer_ = nullptr;
};

}  // namespace mpm::host

#endif