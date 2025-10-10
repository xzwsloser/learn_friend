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

    keyboard_ = new SoftKeyboard;
    frame = new QLabel{this};
    params_ = new WatcherParams;
    timer_ = new QTimer{this};

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

    connect(
        ui->onButton,
        &QPushButton::clicked,
        this,
        &WatcherPage::toggleScreen
    );

    connect(
        ui->startButton,
        &QPushButton::clicked,
        this,
        &WatcherPage::startWatcher
    );

    connect(
        ui->stopButton,
        &QPushButton::clicked,
        this,
        &WatcherPage::stopWatcher
    );

    connect(
        ui->endButton,
        &QPushButton::clicked,
        this,
        &WatcherPage::endWatcher
    );

    /* timer start */
    updateTime();

    connect(
        timer_,
        &QTimer::timeout,
        this,
        [=]() -> void {
        params_->passedTime_ ++;
        if (params_->passedTime_ >= params_->watcherTime_) {
            endWatcher();
            return ;
        }
        QString prompt = QString("已经学习  %1 分钟")
                     .arg(params_->passedTime_);
        ui->statusLabel->setText(prompt);
        }
    );


    /* update status */
    updateStatus();
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

    if (ev->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mev = static_cast<QMouseEvent*>(ev);
        if (mev->button() == Qt::LeftButton) {
            if (watched == ui->timeSettingLabel) {
                QPoint pos = this->pos() +
                             ui->mainWidget->pos() +
                             ui->selectWidget->pos() +
                             ui->paramsWidget->pos() +
                             ui->timeSettingWidget->pos();
                keyboard_->hideInputBufferArea(ui->timeSettingLabel);
                keyboard_->resize(this->width()/2, this->height()/2);
                keyboard_->move(pos +
                               ui->timeSettingLabel->pos() +
                               pos);
                keyboard_->show();
            }
        }
    }

    return QWidget::eventFilter(watched, ev);
}


void WatcherPage::showMessage(const QString &msg, bool normal, int interval)
{
    frame->setText(msg);
    frame->setWindowFlag(Qt::ToolTip);
    frame->resize(this->width()/6, this->height()/10);
    QFont font = frame->font();
    font.setPointSize(22);
    frame->setFont(font);
    if (normal) {
        frame->setStyleSheet("background-color:rgba(255,255,255,0.8); color: green; border-radius:8px; padding:8px;");
    } else {
        frame->setStyleSheet("background-color:rgba(255,255,255,0.8); color: red; border-radius:8px; padding:8px;");
    }

    frame->setAlignment(Qt::AlignCenter);
    frame->move(this->width()/2 - frame->width()/2, this->height()/10);
    frame->show();

    QTimer::singleShot(interval, [=]() -> void {
        if (!frame->isHidden()) {
            frame->hide();
        }
    });
}

void WatcherPage::toggleScreen()
{
    if (status_ != WatcherStatus::WATCHING) return ;

    if (!showCamera_) {
        showCamera_ = true;
        ui->onButton->setText("关闭监控画面");
        /* turn on screen */

    } else {
        showCamera_ = false;
        ui->onButton->setText("显示监控画面");
        /* turn off screen */

    }
}

/* state machine */

void WatcherPage::startWatcher()
{
    if (status_ == WatcherStatus::UNSTARTED) {
        /* valid params */
        QString timeStr = ui->timeSettingLabel->text();
        bool ok{true};
        int settedTime = timeStr.toInt(&ok);
        if (!ok || settedTime < 1 || settedTime > 1440) {
            showMessage("监控时间有误", false);
            return ;
        }

        if (!ui->attentioncheckBox->isChecked() &&
            !ui->distractedcheckBox->isChecked() &&
            !ui->othercheckBox->isChecked())
        {
            showMessage("至少勾选一项", false);
            return ;
        }

        params_->watcherTime_ = settedTime;
        params_->passedTime_ = 0;
        params_->attention_ = ui->attentioncheckBox->isChecked();
        params_->distracted_ = ui->distractedcheckBox->isChecked();
        params_->other_ = ui->othercheckBox->isChecked();

        status_ = WatcherStatus::WATCHING;
        showMessage("开始监控", true);
        /* TODO: 开启监控, 设置倒计时 */

    }

    updateStatus();
}

void WatcherPage::stopWatcher()
{
    if (status_ == WatcherStatus::WATCHING) {
        status_ = WatcherStatus::STOPED;
        ui->stopButton->setText("继续监控");
        showMessage("监控已暂停", true);
        /* TODO: 停止监控, 暂停倒计时 */

    } else if (status_ == WatcherStatus::STOPED) {
        status_ = WatcherStatus::WATCHING;
        ui->stopButton->setText("暂停监控");
        showMessage("监控已重启", true);
        /* TODO: 重启监控, 继续倒计时 */

    }

    updateStatus();
}

void WatcherPage::endWatcher()
{
    if (status_ == WatcherStatus::WATCHING ||
        status_ == WatcherStatus::STOPED) {
        status_ = ENDED;
        showMessage("监控已结束", true);
        /* TODO: 结束监控, 展示参数 */

    } else if (status_ == WatcherStatus::ENDED) {
        /* DEBUG */
        status_ = WatcherStatus::UNSTARTED;
        ui->timeSettingLabel->setText("");
        ui->timeSettingLabel->setLabel("请输入监控时长 (mid)");
        ui->attentioncheckBox->setCheckState(Qt::Unchecked);
        ui->distractedcheckBox->setCheckState(Qt::Unchecked);
        ui->othercheckBox->setCheckState(Qt::Unchecked);
    }

    updateStatus();
}

void WatcherPage::updateStatus()
{
    if (status_ == WatcherStatus::WATCHING) {
        QString prompt = QString("已经学习  %1 分钟")
                                     .arg(params_->passedTime_);
        ui->statusLabel->setText(prompt);
        timer_->start(60 * 1000);
    } else if (status_ == WatcherStatus::STOPED) {
        timer_->stop();
        QString prompt = QString("学习暂停中  %1 分钟")
                             .arg(params_->passedTime_);
        ui->statusLabel->setText(prompt);
    } else if (status_ == WatcherStatus::ENDED) {
        timer_->stop();
        QString prompt = QString("学习结束  %1 分钟")
                             .arg(params_->passedTime_);
        ui->statusLabel->setText(prompt);
    } else if (status_ == WatcherStatus::UNSTARTED) {
        QString prompt = QString("请开启学习监控");
        ui->statusLabel->setText(prompt);
    }
}
