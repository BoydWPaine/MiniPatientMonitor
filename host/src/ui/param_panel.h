#ifndef MPM_HOST_PARAM_PANEL_H
#define MPM_HOST_PARAM_PANEL_H

#include <QLabel>
#include <QWidget>

namespace mpm::host {

class ParamPanel : public QWidget {
    Q_OBJECT

public:
    explicit ParamPanel(QWidget* parent = nullptr);

    void setHr(uint32_t hr);
    void setNibp(uint32_t sys, uint32_t dia, uint32_t mean);
    void setSpo2Pr(uint32_t spo2, uint32_t pr);
    void setRespRate(uint32_t resp_rate);
    void setTemperature(uint32_t temperature_tenths);

private:
    QLabel* hr_label_ = nullptr;
    QLabel* nibp_label_ = nullptr;
    QLabel* spo2_label_ = nullptr;
    QLabel* pr_label_ = nullptr;
    QLabel* resp_label_ = nullptr;
    QLabel* temp_label_ = nullptr;
};

}  // namespace mpm::host

#endif