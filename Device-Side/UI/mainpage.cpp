#include "mainpage.h"
#include "ui_mainpage.h"
#include <QPainter>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QTimer>

MainPage::MainPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainPage)
{
    ui->setupUi(this);
    QFile file{":/qss/mainpage.qss"};
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String{file.readAll()};
        ui->statusWidget->setStyleSheet(styleSheet);
        ui->watcherWidget->setStyleSheet(styleSheet);
        ui->knowledgeWidget->setStyleSheet(styleSheet);
        ui->agentWidget->setStyleSheet(styleSheet);
        ui->welcomeLabel->setStyleSheet(styleSheet);
        ui->timeLabel->setStyleSheet(styleSheet);
        ui->watcherLabel->setStyleSheet(styleSheet);
        ui->knowledgeLabel->setStyleSheet(styleSheet);
        ui->agentLabel->setStyleSheet(styleSheet);
        ui->backbutton->setStyleSheet(styleSheet);
        file.close();
    }

    qInfo() << "the Size of Avatar: " << ui->avatar->size();
    ui->avatar->setSize(100);
    QImage img{":/img/avatar.png"};
    if (img.isNull()) {
        qInfo() << "avatar file not exists";
    }

    qInfo() << "the Size of Image is: " << img.size();
    ui->avatar->setImage(img);

    ui->watcherWidget->installEventFilter(this);
    ui->knowledgeWidget->installEventFilter(this);
    ui->agentWidget->installEventFilter(this);

    QString welcomeText = QString("Hello, %1同学, 欢迎使用智慧伴学系统!")
                              .arg(username_);

    ui->welcomeLabel->setText(welcomeText);

    qInfo() << "the Width of button: " << ui->backbutton->width();
    qInfo() << "the Height of button: " << ui->backbutton->height();
    ui->backbutton->setFixedWidth(200);

    connect(
        ui->backbutton,
        &QPushButton::clicked,
        this,
        &MainPage::backToLoginPage
    );

    updateTime();
}

MainPage::~MainPage()
{
    delete ui;
}

void MainPage::paintEvent(QPaintEvent *ev)
{
    QPainter painter{this};
    QPixmap pix{":/img/mainpage-background.png"};
    painter.drawPixmap(this->rect(), pix);
    QWidget::paintEvent(ev);
}

void MainPage::updateTime()
{
    const QDateTime now = QDateTime::currentDateTime();
    ui->timeLabel->setText(now.toString("yyyy年MM月dd日 HH:mm"));

    QTime t = now.time();
    int passedMsec = t.second()*1000 + t.msec();
    int interval = 60000 - passedMsec;
    if (interval <= 0 || interval > 60000) {
        interval = 60000;
    }

    QTimer::singleShot(interval, [=]() -> void {
        this->updateTime();
    });
}

bool MainPage::eventFilter(QObject *watched, QEvent *ev)
{
    if (ev->type() == QEvent::MouseButtonPress) {
        if (watched == ui->watcherWidget) {
            qInfo() << "点击学习监控模式";
        } else if (watched == ui->knowledgeWidget) {
            qInfo() << "点击知识库管理";
        } else if (watched == ui->agentWidget) {
            qInfo() << "点击助学助手";
        }
    }
    return QWidget::eventFilter(watched, ev);
}

void MainPage::setUsername(const QString &username)
{
    username_ = username;
    QString welcomeText = QString("Hello, %1同学, 欢迎使用智慧伴学系统!")
                              .arg(username_);

    ui->welcomeLabel->setText(welcomeText);
}
