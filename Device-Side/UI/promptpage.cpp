#include "promptpage.h"
#include "ui_promptpage.h"
#include <QFile>
#include <QPainter>

PromptPage::PromptPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PromptPage)
{
    this->setObjectName("PromptPage");
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    ui->setupUi(this);

    QFile file{":/qss/prompt.qss"};
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

    int w = ui->saveButton->width();
    int h = ui->saveButton->height();
    ui->saveButton->setFixedWidth(2*w);
    ui->saveButton->setFixedHeight(2*h);

    w = ui->cancelButton->width();
    h = ui->cancelButton->height();
    ui->cancelButton->setFixedWidth(2*w);
    ui->cancelButton->setFixedHeight(2*h);

    connect(
        ui->saveButton,
        &QPushButton::clicked,
        this,
        &PromptPage::save
    );

    connect(
        ui->cancelButton,
        &QPushButton::clicked,
        this,
        &PromptPage::cancel
    );
}

PromptPage::~PromptPage()
{
    delete ui;
}

void PromptPage::paintEvent(QPaintEvent *ev)
{
    QPainter painter{this};
    QPixmap pix{":/img/prompt_background.png"};
    painter.drawPixmap(this->rect(), pix);

    QWidget::paintEvent(ev);
}

void PromptPage::setMarkdownInfo(const QString &msg)
{
    ui->infoLabel->setText(msg);
}

