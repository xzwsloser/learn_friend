#include "watcherpage.h"
#include "ui_watcherpage.h"
#include <QPainter>
#include <QFile>

WatcherPage::WatcherPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WatcherPage)
{
    ui->setupUi(this);

    QFile file{":/qss/watcher.qss"};
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        ui->statusWidget->setStyleSheet(styleSheet);
        ui->screenWidget->setStyleSheet(styleSheet);
        ui->settingWidget->setStyleSheet(styleSheet);
        file.close();
    }

    ui->timeSettingLabel->setLabel("监控时间设置(min)");
}

WatcherPage::~WatcherPage()
{
    delete ui;
}

void WatcherPage::paintEvent(QPaintEvent *ev)
{
    QPainter painter{this};
    QPixmap pix{":/img/watcher_setting_background.png"};
    painter.drawPixmap(this->rect(), pix);
    QWidget::paintEvent(ev);
}
