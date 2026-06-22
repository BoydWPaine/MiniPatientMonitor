#ifndef MPM_HOST_TOP_BAR_H
#define MPM_HOST_TOP_BAR_H

#include <QLabel>
#include <QTimer>
#include <QWidget>

namespace mpm::host {

class TopBar : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWidget* parent = nullptr);

private slots:
    void updateDateTime();

private:
    QLabel* date_label_ = nullptr;
    QLabel* time_label_ = nullptr;
    QTimer clock_timer_;
};

}  // namespace mpm::host

#endif