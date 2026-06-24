#ifndef MPM_HOST_WAVEFORM_WIDGET_H
#define MPM_HOST_WAVEFORM_WIDGET_H

#include <QColor>
#include <QWidget>

#include <cstdint>
#include <deque>
#include <vector>

class QResizeEvent;
class QTimer;

namespace mpm::host {

class WaveformWidget : public QWidget {
    Q_OBJECT

public:
    static constexpr int kSweepGap = 20;

    explicit WaveformWidget(const QColor& color, QWidget* parent = nullptr);

    void addSample(int32_t sample);
    void clearSamples();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onRefresh();

private:
    void resetColumns();
    void advanceSweep(int32_t sample);
    int sampleToPixelY(int32_t sample) const;

    QColor color_;
    QTimer* refresh_timer_ = nullptr;
    std::deque<int32_t> pending_samples_;
    std::vector<int> column_y_;
    std::vector<bool> column_valid_;
    int draw_x_ = 0;
};

}  // namespace mpm::host

#endif