#ifndef MPM_HOST_WAVEFORM_WIDGET_H
#define MPM_HOST_WAVEFORM_WIDGET_H

#include <QColor>
#include <QString>
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
    static constexpr int kLabelWidth = 64;
    static constexpr int kLabelHeight = 32;
    static constexpr int kTraceStartX = 64;
    static constexpr int kTraceEndXEcg = 747;
    static constexpr int kTraceEndXDefault = 767;
    static constexpr int kScaleLineX = 748;
    static constexpr int kScaleLineWidth = 2;
    static constexpr int kScaleUnitX = 751;
    static constexpr int kScaleUnitY = 32;
    static constexpr int kScaleUnitWidth = 16;
    static constexpr int kScaleUnitHeight = 32;
    static constexpr int kEcgAmplitudeRange = 512;
    static constexpr int kDefaultAmplitudeRange = 2048;

    explicit WaveformWidget(const QColor& color,
                            const QString& label,
                            bool show_ecg_scale = false,
                            QWidget* parent = nullptr);

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
    void drawOverlay(QPainter& painter) const;
    bool isTraceColumn(int x) const;

    QColor color_;
    QString label_text_;
    bool show_ecg_scale_ = false;
    int trace_end_x_ = kTraceEndXDefault;
    int32_t amplitude_range_ = kDefaultAmplitudeRange;
    QTimer* refresh_timer_ = nullptr;
    std::deque<int32_t> pending_samples_;
    std::vector<int> column_y_;
    std::vector<bool> column_valid_;
    int draw_index_ = 0;
};

}  // namespace mpm::host

#endif