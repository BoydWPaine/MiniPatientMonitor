#ifndef MPM_HOST_MAIN_WINDOW_H
#define MPM_HOST_MAIN_WINDOW_H

#include "monitor.pb.h"
#include "nibp_controller.h"
#include "net_thread.h"
#include "ui/param_panel.h"
#include "ui/top_bar.h"
#include "ui/waveform_widget.h"

#include <QMainWindow>

namespace mpm::host {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onEnvelopeReceived(const QByteArray& payload);
    void onManualNibpClicked();

private:
    int32_t pickLeadSample(const monitor::EcgPacket& packet, int lead_index) const;

    NetThread net_thread_;
    NibpController nibp_controller_;
    WaveformWidget* ecg_lead2_widget_ = nullptr;
    WaveformWidget* ecg_leadv_widget_ = nullptr;
    WaveformWidget* pleth_widget_ = nullptr;
    WaveformWidget* resp_widget_ = nullptr;
    WaveformWidget* temp_widget_ = nullptr;
    ParamPanel* param_panel_ = nullptr;
};

}  // namespace mpm::host

#endif