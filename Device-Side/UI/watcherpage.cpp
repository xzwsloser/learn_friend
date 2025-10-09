#include "watcherpage.h"
#include "ui_watcherpage.h"
#include <QPainter>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QTime>
#include <QIntValidator>
#include <QTimer>

WatcherPage::WatcherPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WatcherPage)
{
    ui->setupUi(this);

    QFile file{":/qss/watcher.qss"};
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        /* qss style */
        QString styleSheet = QLatin1String(file.readAll());
        ui->statusWidget->setStyleSheet(styleSheet);
        ui->screenWidget->setStyleSheet(styleSheet);
        ui->settingWidget->setStyleSheet(styleSheet);
        ui->screen->setStyleSheet(styleSheet);
        ui->backButton->setStyleSheet(styleSheet);
        ui->onButton->setStyleSheet(styleSheet);
        ui->settingPromptLabel->setStyleSheet(styleSheet);
        ui->attentioncheckBox->setStyleSheet(styleSheet);
        ui->distractedcheckBox->setStyleSheet(styleSheet);
        ui->othercheckBox->setStyleSheet(styleSheet);
        ui->startButton->setStyleSheet(styleSheet);
        ui->stopButton->setStyleSheet(styleSheet);
        ui->endButton->setStyleSheet(styleSheet);
        file.close();
    }

    /* status bar */
    ui->backButton->setFixedWidth(90);
    ui->backButton->setFixedHeight(65);

    /* screen widget */
    ui->onButton->setFixedHeight(60);

    /* param setting widget */
    ui->timeSettingLabel->setLabelFontSize(15);
    QFont font = ui->timeSettingLabel->font();
    font.setPointSize(30);
    ui->timeSettingLabel->setFont(font);
    ui->timeSettingLabel->setLabel("请输入监控时长 (mid)");
    ui->timeSettingLabel->installEventFilter(this);
    QIntValidator *validator = new QIntValidator{1, 1440, ui->timeSettingLabel};
    ui->timeSettingLabel->setValidator(validator);

    ui->startButton->setFixedHeight(60);
    ui->stopButton->setFixedHeight(60);
    ui->endButton->setFixedHeight(60);

    /* timer start */
    updateTime();
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

void WatcherPage::updateTime()
{
    QDateTime now = QDateTime::currentDateTime();
    QString curTimeStr = now.toString("yyyy年MM月dd日 HH:mm");
    ui->timeLabel->setText(curTimeStr);

    QTime t = now.time();
    int distance = t.second()*1000 + t.msec();
    int interval = 60000 - distance;
    if (interval <= 0 || interval > 60000) {
        interval = 60000;
    }

    QTimer::singleShot(interval, [=]()->void {
        this->updateTime();
    });
}

bool WatcherPage::eventFilter(QObject *watched, QEvent *ev)
{
    if (watched == ui->timeSettingLabel) {
        if (ui->timeSettingLabel->text() != "") {
            ui->timeSettingLabel->setLabel("");
        } else {
            ui->timeSettingLabel->setLabel("请输入监控时长 (mid)");
        }
    }

    return QWidget::eventFilter(watched, ev);
}










