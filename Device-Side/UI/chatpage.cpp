#include "chatpage.h"
#include "ui_chatpage.h"
#include <QPainter>
#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QScrollBar>
#include <QtNetwork/QNetworkReply>

chatPage::chatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatPage)
{
    ui->setupUi(this);

    /* qss */
    QFile file{":/qss/chat.qss"};
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

    /* voice Button */
    ui->voiceButton->setFixedSize(200, 200);

    voiceButtonStyles[0] = "QPushButton#voiceButton {"
                     "border-image: url(:/img/voice_unstarted.png) 0 0 0 0 stretch stretch;"
                     "border-radius: 100px;"
                     "background-repeat: no-repeat;"
                     "background-position: center;"
                           "border: none; }";

    voiceButtonStyles[1] = "QPushButton#voiceButton {"
                     "border-image: url(:/img/voice_taking.png) 0 0 0 0 stretch stretch;"
                     "border-radius: 100px;"
                     "background-repeat: no-repeat;"
                     "background-position: center;"
                           "border: none; }";

    ui->voiceButton->setStyleSheet(voiceButtonStyles[static_cast<int>(voiceStatus_)]);
    ui->voiceButton->setText("");

    connect(
        ui->voiceButton,
        &QPushButton::clicked,
        this,
        &chatPage::voiceRecord
    );

    /* other Button */
    ui->backButton->setFixedSize(130, 100);
    ui->certainButton->setFixedSize(150, 120);
    ui->clearButton->setFixedSize(150, 120);

    connect(
        ui->clearButton,
        &QPushButton::clicked,
        this,
        [=]() -> void {
            ui->inputEdit->clear();
        }
    );

    connect(
        ui->certainButton,
        &QPushButton::clicked,
        this,
        &chatPage::sendHumanMsg
    );

    connect(
        ui->backButton,
        &QPushButton::clicked,
        this,
        &chatPage::backToMain
    );

    /* keyboard */
    keyboard_ = new SoftKeyboard{};

    /* input Editor */
    ui->inputEdit->installEventFilter(this);

    /* layout */
    QVBoxLayout *layout = new QVBoxLayout{ui->chatWindow};
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    layout->addSpacing(10);

    /* chat Window */
    ui->chatWindow->setStyleSheet(R"(
        background-color: rgba(255, 255, 255, 0.8);
    )");
}

chatPage::~chatPage()
{
    delete ui;
}

void chatPage::paintEvent(QPaintEvent *ev)
{
    QPainter painter{this};
    QPixmap pix{":/img/chat-background"};
    painter.drawPixmap(this->rect(), pix);

    QWidget::paintEvent(ev);
}

void chatPage::voiceRecord()
{
    if (voiceStatus_ == VoiceStatus::RAW) {
        /* TODO: Begin Record Voice */
        voiceStatus_ = VoiceStatus::TAKING;


    } else if (voiceStatus_ == VoiceStatus::TAKING) {
        /* TODO: End Record Voice */
        voiceStatus_ = VoiceStatus::RAW;

    }

    ui->voiceButton->setStyleSheet(voiceButtonStyles[static_cast<int>(voiceStatus_)]);
}

bool chatPage::eventFilter(QObject *watched, QEvent *ev)
{
    if (watched == ui->inputEdit &&
        ev->type() == QEvent::FocusIn) {
        keyboard_->hideInputBufferArea(ui->inputEdit);
        keyboard_->resize(this->width()/2,
                          this->height()/2);
        int x = (this->width() - keyboard_->width())/2;
        int y = (this->height() - keyboard_->height())/2;

        keyboard_->move(x, y);
        keyboard_->show();
    }

    return QWidget::eventFilter(watched, ev);
}

void chatPage::sendHumanMsg()
{
    if (status_ == ChatStatus::CHATING) return ;

    status_ = ChatStatus::CHATING;

    QScrollBar *bar = ui->chatArea->verticalScrollBar();
    bar->setValue(bar->maximum());

    ChatBubbleWidget *record = new ChatBubbleWidget{ChatRole::HUMAN};
    int maxLineSize = ui->chatArea->viewport()->width()*2/3;

    ui->chatWindow->layout()->addWidget(record);
    record->setMaxLineSize(maxLineSize);
    msgRecords_.push_back(record);

    record->setText(ui->inputEdit->toPlainText());
    record->adjustBubbleSize();

    /* TODO: Call LLM */
    ChatBubbleWidget *resp = new ChatBubbleWidget{ChatRole::AI};
    resp->setMaxLineSize(maxLineSize);
    ui->chatWindow->layout()->addWidget(resp);
    msgRecords_.push_back(resp);

    /* Call LLM */
    std::function<void(QNetworkReply *)> onready = std::bind(&chatPage::onStreamProcess,
                                                             this,
                                                             resp,
                                                             std::placeholders::_1);
    std::function<void(QNetworkReply *)> onfinish = std::bind(&chatPage::onStreamFinish,
                                                              this,
                                                              resp,
                                                              std::placeholders::_1);

    client_->stream(ui->inputEdit->toPlainText(),
                    onready,
                    onfinish);
}

void chatPage::onStreamProcess(ChatBubbleWidget *aiMessage,
                               QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray chunk = reply->readAll();
        aiMessage->appendText(HttpClient::getWordFromStream(chunk));
    }
}

void chatPage::onStreamFinish(ChatBubbleWidget *aiMessage,
                              QNetworkReply *reply)
{
    status_ = ChatStatus::INIT;
    reply->deleteLater();
}

void chatPage::onChatProcess(ChatBubbleWidget *aiMessage,
                             QNetworkReply *reply)
{
    status_ = ChatStatus::INIT;
    reply->deleteLater();
}

void chatPage::clearMessage()
{
    for (ChatBubbleWidget *msg: msgRecords_) {
        if (msg != nullptr) {
            ui->chatWindow->layout()->removeWidget(msg);
            delete msg;
            msg = nullptr;
        }
    }

    msgRecords_.clear();

    ui->inputEdit->clear();
}

ChatBubbleWidget::ChatBubbleWidget(ChatRole role,
                                   QWidget *parent):
    QWidget(parent)
{

    avatar_ = new QtMaterialAvatar{};
    avatar_->setSize(100);
    avatar_->setFixedSize(120, 120);

    content_ = new QLabel{this};
    content_->setAlignment(Qt::AlignTop);
    content_->setWordWrap(true);
    content_->clear();

    QFont font = content_->font();
    font.setPointSize(15);
    content_->setFont(font);

    QHBoxLayout *layout = new QHBoxLayout{this};

    if (role == ChatRole::AI) {
        content_->setTextFormat(Qt::MarkdownText);
        content_->setStyleSheet(
        "border-radius: 8px;"
        "padding: 8px 10px;"
        "font-family: \"Microsoft YaHei\", \"SimHei\", sans-serif;"
        "line-height: 1.5;"
        "background-color: #f0f0f0;"
        "word-wrap: break-word;"
        "color: #333333;");

        avatar_->setImage(QImage{":/img/ai_avatar.png"});
        layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        layout->addWidget(avatar_);
        layout->addWidget(content_);
        layout->addItem(new QSpacerItem(40, 20,
                                        QSizePolicy::Expanding,
                                        QSizePolicy::Minimum));
    } else if (role == ChatRole::HUMAN) {
        content_->setStyleSheet(
        "border-radius: 8px;"
        "padding: 8px 10px;"
        "font-family: \"Microsoft YaHei\", \"SimHei\", sans-serif;"
        "line-height: 1.5;"
        "word-wrap: break-word;"
        "background-color: #95ec69;"
        "color: #333333;");

        avatar_->setImage(QImage{":/img/avatar.png"});
        layout->setAlignment(Qt::AlignTop | Qt::AlignRight);
        layout->addItem(new QSpacerItem(40, 20,
                                        QSizePolicy::Expanding,
                                        QSizePolicy::Minimum));
        layout->addWidget(content_);
        layout->addWidget(avatar_);
    }

}

ChatBubbleWidget::~ChatBubbleWidget()
{

}

void ChatBubbleWidget::paintEvent(QPaintEvent *ev)
{
    /* QPainter painter{this};
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor bgColor = QColor(255, 255, 255, static_cast<int>(255*0.8));
    painter.setBrush(bgColor);

    painter.setPen(Qt::NoPen);

    int radius = 20;
    QRectF rect = this->rect();
    painter.drawRoundedRect(rect, radius, radius); */

    QWidget::paintEvent(ev);
}

void ChatBubbleWidget::adjustBubbleSize()
{
    content_->setMaximumWidth(maxLineSize_);

    QSize labelSize = content_->sizeHint();
    content_->setFixedHeight(labelSize.height());

    this->adjustSize();

    this->layout()->setAlignment(avatar_, Qt::AlignTop);
}

void ChatBubbleWidget::setText(const QString &msg)
{
    content_->setText(msg);
    if (content_->text().size() % 10 == 0) {
        adjustBubbleSize();
    }
}

void ChatBubbleWidget::appendText(const QString &msg)
{
    content_->setText(content_->text()+msg);
    if (content_->text().size() % 10 == 0) {
        adjustBubbleSize();
    }
}

void ChatBubbleWidget::setMaxLineSize(int maxLineSize)
{
    maxLineSize_ = maxLineSize;
    content_->setMaximumWidth(maxLineSize_);
}
