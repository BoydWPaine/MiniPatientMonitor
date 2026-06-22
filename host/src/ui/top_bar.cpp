#include "ui/top_bar.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QVBoxLayout>

namespace mpm::host {

namespace {
constexpr int kTopBarHeight = 64;
constexpr int kPatientInfoWidth = 180;
constexpr int kAlarmWidth = 300;
constexpr int kIconsWidth = 64;
constexpr int kDateTimeWidth = 180;
constexpr int kSubRowHeight = 32;
constexpr int kTwoLineFontSize = 16;
constexpr int kAlarmFontSize = 18;

QString twoLineLabelStyle(const QString& color)
{
    return QStringLiteral("color: %1; background: transparent; font-size: %2px; font-weight: bold;")
        .arg(color)
        .arg(kTwoLineFontSize);
}

QString alarmLabelStyle(const QString& color, const QString& background)
{
    return QStringLiteral("color: %1; background: %2; font-size: %3px; font-weight: bold;")
        .arg(color, background)
        .arg(kAlarmFontSize);
}

QWidget* makeTwoLineBlock(QWidget* parent,
                          int width,
                          const QString& top_text,
                          const QString& bottom_text,
                          const QString& background,
                          const QString& color)
{
    auto* block = new QWidget(parent);
    block->setFixedSize(width, kTopBarHeight);
    block->setStyleSheet(QStringLiteral("background: %1;").arg(background));

    auto* layout = new QVBoxLayout(block);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto add_line = [&](const QString& text) {
        auto* label = new QLabel(text, block);
        label->setFixedHeight(kSubRowHeight);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(twoLineLabelStyle(color));
        layout->addWidget(label);
    };

    add_line(top_text);
    add_line(bottom_text);
    return block;
}

QLabel* makeAlarmBanner(QWidget* parent,
                        int width,
                        const QString& text,
                        const QString& background,
                        const QString& color)
{
    auto* label = new QLabel(text, parent);
    label->setFixedSize(width, kTopBarHeight);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(alarmLabelStyle(color, background));
    return label;
}

QWidget* makeDateTimeBlock(QWidget* parent, QLabel*& date_label, QLabel*& time_label)
{
    auto* block = new QWidget(parent);
    block->setFixedSize(kDateTimeWidth, kTopBarHeight);
    block->setStyleSheet(QStringLiteral("background: black;"));

    auto* layout = new QVBoxLayout(block);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    date_label = new QLabel(block);
    date_label->setFixedHeight(kSubRowHeight);
    date_label->setAlignment(Qt::AlignCenter);
    date_label->setStyleSheet(twoLineLabelStyle(QStringLiteral("white")));

    time_label = new QLabel(block);
    time_label->setFixedHeight(kSubRowHeight);
    time_label->setAlignment(Qt::AlignCenter);
    time_label->setStyleSheet(twoLineLabelStyle(QStringLiteral("white")));

    layout->addWidget(date_label);
    layout->addWidget(time_label);
    return block;
}

}  // namespace

TopBar::TopBar(QWidget* parent) : QWidget(parent)
{
    setFixedHeight(kTopBarHeight);
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    setPalette(palette);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(makeTwoLineBlock(this,
                                       kPatientInfoWidth,
                                       QStringLiteral("John McCarthy"),
                                       QStringLiteral("ICU-1076"),
                                       QStringLiteral("black"),
                                       QStringLiteral("white")));

    layout->addWidget(makeAlarmBanner(this,
                                       kAlarmWidth,
                                       QStringLiteral("Ventricular Tachycardia"),
                                       QStringLiteral("red"),
                                       QStringLiteral("white")));

    auto* icon_area = new QLabel(QStringLiteral("!"), this);
    icon_area->setFixedSize(kIconsWidth, kTopBarHeight);
    icon_area->setAlignment(Qt::AlignCenter);
    icon_area->setStyleSheet(QStringLiteral("color: white; background: black; font-size: 28px; font-weight: bold;"));
    layout->addWidget(icon_area);

    layout->addWidget(makeAlarmBanner(this,
                                       kAlarmWidth,
                                       QStringLiteral("Missing Leads"),
                                       QStringLiteral("yellow"),
                                       QStringLiteral("black")));

    layout->addWidget(makeDateTimeBlock(this, date_label_, time_label_));

    connect(&clock_timer_, &QTimer::timeout, this, &TopBar::updateDateTime);
    clock_timer_.start(1000);
    updateDateTime();
}

void TopBar::updateDateTime()
{
    const QDateTime now = QDateTime::currentDateTime();
    date_label_->setText(now.toString(QStringLiteral("MMMM d, yyyy")));
    time_label_->setText(now.toString(QStringLiteral("HH:mm:ss")));
}

}  // namespace mpm::host