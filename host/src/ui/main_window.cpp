#include "ui/main_window.h"

#include "monitor.pb.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace mpm::host {

namespace {
constexpr int kWindowWidth = 1024;
constexpr int kWindowHeight = 768;
constexpr int kBottomBarHeight = 64;
constexpr int kWaveformWidth = 640;
constexpr int kRowHeight = 128;
}  // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), nibp_controller_(&net_thread_)
{
    setWindowTitle(QStringLiteral("MiniPatientMonitor Host"));
    setFixedSize(kWindowWidth, kWindowHeight);

    auto* central = new QWidget(this);
    central->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    central->setPalette(palette);
    setCentralWidget(central);

    auto* root_layout = new QVBoxLayout(central);
    root_layout->setContentsMargins(0, 0, 0, 0);
    root_layout->setSpacing(0);

    root_layout->addWidget(new TopBar(central));

    auto* body = new QHBoxLayout();
    body->setContentsMargins(0, 0, 0, 0);
    body->setSpacing(0);
    root_layout->addLayout(body);

    auto* waveform_panel = new QWidget(central);
    waveform_panel->setFixedSize(kWaveformWidth, kRowHeight * 5);
    auto* waveform_layout = new QVBoxLayout(waveform_panel);
    waveform_layout->setContentsMargins(0, 0, 0, 0);
    waveform_layout->setSpacing(0);

    ecg_lead2_widget_ = new WaveformWidget(QColor(9, 78, 22), waveform_panel);
    ecg_leadv_widget_ = new WaveformWidget(QColor(9, 78, 22), waveform_panel);
    pleth_widget_ = new WaveformWidget(QColor(9, 68, 58), waveform_panel);
    resp_widget_ = new WaveformWidget(QColor(98, 80, 4), waveform_panel);
    temp_widget_ = new WaveformWidget(QColor(255, 255, 255), waveform_panel);

    for (WaveformWidget* widget :
         {ecg_lead2_widget_, ecg_leadv_widget_, pleth_widget_, resp_widget_, temp_widget_}) {
        widget->setFixedHeight(kRowHeight);
        waveform_layout->addWidget(widget);
    }

    body->addWidget(waveform_panel);

    param_panel_ = new ParamPanel(central);
    param_panel_->setFixedHeight(kRowHeight * 5);
    body->addWidget(param_panel_);

    auto* bottom_bar = new QWidget(central);
    bottom_bar->setFixedHeight(kBottomBarHeight);
    auto* bottom_layout = new QHBoxLayout(bottom_bar);
    bottom_layout->setContentsMargins(0, 0, 0, 0);
    bottom_layout->setSpacing(0);

    const QStringList labels = {
        QStringLiteral("左翻页"),
        QStringLiteral("接收/解除"),
        QStringLiteral("事件"),
        QStringLiteral("回顾"),
        QStringLiteral("配置"),
        QStringLiteral("声音"),
        QStringLiteral("待机"),
        QStringLiteral("右翻页"),
    };

    for (const QString& label : labels) {
        auto* button = new QPushButton(label, bottom_bar);
        button->setFixedSize(128, kBottomBarHeight);
        button->setEnabled(label == QStringLiteral("回顾"));
        if (label == QStringLiteral("回顾")) {
            connect(button, &QPushButton::clicked, this, &MainWindow::onManualNibpClicked);
        }
        bottom_layout->addWidget(button);
    }

    root_layout->addWidget(bottom_bar);

    connect(&net_thread_, &NetThread::envelopeReceived, this, &MainWindow::onEnvelopeReceived);
    nibp_controller_.startStatTimer();
    QTimer::singleShot(2000, &nibp_controller_, &NibpController::triggerManualMeasure);
    net_thread_.start();
}

MainWindow::~MainWindow()
{
    net_thread_.requestStop();
    net_thread_.wait();
}

void MainWindow::onManualNibpClicked()
{
    nibp_controller_.triggerManualMeasure();
}

int32_t MainWindow::pickLeadSample(const monitor::EcgPacket& packet, int lead_index) const
{
    switch (lead_index) {
    case 2:
        return packet.ecg_lead_ii_size() > 0 ? packet.ecg_lead_ii(0) : 0;
    case 8:
        return packet.ecg_lead_v1_size() > 0 ? packet.ecg_lead_v1(0) : 0;
    default:
        return 0;
    }
}

void MainWindow::onEnvelopeReceived(const QByteArray& payload)
{
    monitor::Envelope envelope;
    if (!envelope.ParseFromArray(payload.data(), payload.size())) {
        return;
    }

    switch (envelope.payload_case()) {
    case monitor::Envelope::kEcg: {
        const auto& ecg = envelope.ecg();
        param_panel_->setHr(ecg.hr());
        ecg_lead2_widget_->addSample(pickLeadSample(ecg, 2));
        ecg_leadv_widget_->addSample(pickLeadSample(ecg, 8));
        break;
    }
    case monitor::Envelope::kSpo2: {
        const auto& spo2 = envelope.spo2();
        param_panel_->setSpo2Pr(spo2.spo2(), spo2.pr());
        if (spo2.pleth_wave_size() > 0) {
            pleth_widget_->addSample(spo2.pleth_wave(0));
        }
        break;
    }
    case monitor::Envelope::kResp: {
        const auto& resp = envelope.resp();
        param_panel_->setRespRate(resp.resp_rate());
        if (resp.resp_wave_size() > 0) {
            resp_widget_->addSample(resp.resp_wave(0));
        }
        break;
    }
    case monitor::Envelope::kTemp: {
        const auto& temp = envelope.temp();
        param_panel_->setTemperature(temp.temperature());
        if (temp.temp_wave_size() > 0) {
            temp_widget_->addSample(temp.temp_wave(0));
        }
        break;
    }
    case monitor::Envelope::kNibp: {
        const auto& nibp = envelope.nibp();
        param_panel_->setNibp(nibp.nibp_sys(), nibp.nibp_dia(), nibp.nibp_mean());
        break;
    }
    case monitor::Envelope::kNullPacket:
    default:
        break;
    }
}

}  // namespace mpm::host