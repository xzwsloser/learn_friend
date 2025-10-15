#include "knowledgepage.h"
#include "ui_knowledgepage.h"
#include <QPainter>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QTime>
#include <QTimer>

KnowledgePage::KnowledgePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KnowledgePage)
{
    ui->setupUi(this);

    /* QSS */
    QFile file{":/qss/knowledge.qss"};
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

    /* Tabs */
    /* ui->tabWidget->addTab("知识库检索") ;
    ui->tabWidget->addTab("笔记上传");
    ui->tabWidget->setCurrentTab(0);
    ui->tabWidget->setHaloVisible(false);
    ui->tabWidget->setFixedWidth(this->width()/2); */

    /* backButton */
    ui->backButton->setFixedSize(150, 120);

    /* Stacked Widget */
    QWidget *page_1 = ui->stackedWidget->widget(0);
    QWidget *page_2 = ui->stackedWidget->widget(1);
    if (page_1) {
        ui->stackedWidget->removeWidget(page_1);
    }

    if (page_2) {
        ui->stackedWidget->removeWidget(page_2);
    }

    retrievedPage_ = new KnowledgeRetrievePage{ui->stackedWidget};
    ui->stackedWidget->addWidget(retrievedPage_);
    ui->stackedWidget->setCurrentIndex(static_cast<int>(curpage_));

    QFont font = ui->timeLabel->font();
    font.setPointSize(20);
    ui->timeLabel->setFont(font);

    connect(
        ui->backButton,
        &QPushButton::clicked,
        this,
        &KnowledgePage::backToMain
    );

    updateTime();
}

KnowledgePage::~KnowledgePage()
{
    delete ui;
}

void KnowledgePage::paintEvent(QPaintEvent *ev)
{
    QPainter painter{this};
    QPixmap pix{":/img/knowledgePage_background.png"};
    painter.drawPixmap(this->rect(), pix);

    QWidget::paintEvent(ev);
}

void KnowledgePage::resizeEvent(QResizeEvent *ev)
{
    ui->backButton->setFixedWidth(this->width()/20);
}

void KnowledgePage::updateTime()
{
    QDateTime curTime = QDateTime::currentDateTime();
    ui->timeLabel->setText(curTime.toString("yyyy年MM月dd日 HH:mm"));

    QTime now = curTime.time();
    int passedTime = now.second()*1000 + now.msec();
    int leftedTime = 60000 - passedTime;

    if (leftedTime <= 0 || leftedTime > 60000) {
        leftedTime = 60000;
    }

    QTimer::singleShot(leftedTime, [=]() -> void {
        updateTime();
    });
}

void KnowledgePage::clearAllText()
{
    KnowledgeRetrievePage *page =
        static_cast<KnowledgeRetrievePage*>(ui->stackedWidget->currentWidget());
    page->clearText();
}

