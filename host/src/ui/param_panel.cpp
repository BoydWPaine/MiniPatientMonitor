#include "ui/param_panel.h"

#include <QFont>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace mpm::host {

namespace {
constexpr int kRowHeight = 128;
constexpr int kPanelWidth = 256;

QWidget* makeSeparator(QWidget* parent)
{
    auto* line = new QWidget(parent);
    line->setFixedHeight(1);
    line->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(128, 128, 128));
    line->setPalette(palette);
    return line;
}

QLabel* makeValueLabel(QWidget* parent, const QColor& color, int font_size)
{
    auto* label = new QLabel(QStringLiteral("--"), parent);
    QFont font = label->font();
    font.setPointSize(font_size);
    font.setBold(true);
    label->setFont(font);
    label->setStyleSheet(QStringLiteral("color: rgb(%1,%2,%3);")
                             .arg(color.red())
                             .arg(color.green())
                             .arg(color.blue()));
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    return label;
}

}  // namespace

ParamPanel::ParamPanel(QWidget* parent) : QWidget(parent)
{
    setFixedWidth(kPanelWidth);
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::black);
    setPalette(palette);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 0, 4, 0);
    layout->setSpacing(0);

    auto add_simple_row = [&](const QColor& color, QLabel*& value_label, int font_size) {
        auto* row = new QWidget(this);
        row->setFixedHeight(kRowHeight);
        auto* row_layout = new QHBoxLayout(row);
        row_layout->setContentsMargins(0, 0, 0, 0);
        value_label = makeValueLabel(row, color, font_size);
        row_layout->addWidget(value_label);
        layout->addWidget(row);
        layout->addWidget(makeSeparator(this));
    };

    add_simple_row(QColor(9, 78, 22), hr_label_, 48);
    add_simple_row(QColor(94, 94, 94), nibp_label_, 24);

    auto* spo2_row = new QWidget(this);
    spo2_row->setFixedHeight(kRowHeight);
    auto* spo2_layout = new QHBoxLayout(spo2_row);
    spo2_layout->setContentsMargins(0, 0, 0, 0);
    spo2_label_ = makeValueLabel(spo2_row, QColor(9, 68, 58), 48);
    pr_label_ = makeValueLabel(spo2_row, QColor(9, 68, 58), 18);
    pr_label_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    spo2_layout->addWidget(spo2_label_);
    spo2_layout->addStretch();
    spo2_layout->addWidget(pr_label_);
    layout->addWidget(spo2_row);
    layout->addWidget(makeSeparator(this));

    add_simple_row(QColor(98, 80, 4), resp_label_, 40);
    add_simple_row(QColor(255, 255, 255), temp_label_, 40);

    setNibp(0, 0, 0);
}

void ParamPanel::setHr(uint32_t hr)
{
    hr_label_->setText(QString::number(hr));
}

void ParamPanel::setNibp(uint32_t sys, uint32_t dia, uint32_t mean)
{
    if (sys == 0 && dia == 0 && mean == 0) {
        nibp_label_->setText(QStringLiteral("---/---\n(---)"));
        return;
    }
    nibp_label_->setText(QStringLiteral("%1/%2\n(%3)").arg(sys).arg(dia).arg(mean));
}

void ParamPanel::setSpo2Pr(uint32_t spo2, uint32_t pr)
{
    spo2_label_->setText(QString::number(spo2));
    pr_label_->setText(QString::number(pr));
}

void ParamPanel::setRespRate(uint32_t resp_rate)
{
    resp_label_->setText(QString::number(resp_rate));
}

void ParamPanel::setTemperature(uint32_t temperature_tenths)
{
    const double celsius = static_cast<double>(temperature_tenths) / 10.0;
    temp_label_->setText(QString::number(celsius, 'f', 1));
}

}  // namespace mpm::host