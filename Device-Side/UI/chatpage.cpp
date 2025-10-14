#include "chatpage.h"
#include "ui_chatpage.h"
#include <QPainter>
#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QScrollBar>

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
    ui->voiceButton->setFixedSize(120, 120);

    voiceButtonStyles[0] = "QPushButton#voiceButton {"
                     "border-image: url(:/img/voice_unstarted.png) 0 0 0 0 stretch stretch;"
                     "border-radius: 60px;"
                     "background-repeat: no-repeat;"
                     "background-position: center;"
                           "border: none; }";

    voiceButtonStyles[1] = "QPushButton#voiceButton {"
                     "border-image: url(:/img/voice_taking.png) 0 0 0 0 stretch stretch;"
                     "border-radius: 60px;"
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
    ui->backButton->setFixedSize(120, 80);
    ui->certainButton->setFixedSize(120, 80);
    ui->clearButton->setFixedSize(120, 80);

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

    /* TODO: Call LLM */
    ChatBubbleWidget *resp = new ChatBubbleWidget{ChatRole::AI};
    resp->setMaxLineSize(maxLineSize);
    ui->chatWindow->layout()->addWidget(resp);
    msgRecords_.push_back(resp);

    QString text =     "### 一、Qt 聊天界面开发核心组件\n"
                       "$$E=mc^2$$"
                       "在基于 C++ Qt 开发 AI 聊天系统时，核心组件的选型与配置直接影响界面交互体验，以下是关键组件的设计要点：\n"
                       "- **QScrollArea**：作为聊天内容容器，需设置 `setWidgetResizable(true)` 确保内部部件自适应大小，同时通过 `verticalScrollBar()->setValue(maximum())` 实现新消息自动滚动到底部；\n"
                       "- **自定义 ChatBubbleWidget**：承载单条聊天记录，内部需包含头像（`QtMaterialAvatar`）、消息文本（`QLabel`）和布局（`QHBoxLayout`），头像建议固定尺寸（如 120x120px），文本标签需开启 `setWordWrap(true)` 支持自动换行；\n"
                       "- **QTimer**：用于实现流式消息效果，通过 `singleShot(100, ...)` 每隔 100ms 追加一个字符，需注意定时器资源的释放（`deleteLater()`）避免内存泄漏。\n"
                       "\n"
                       "## 二、聊天气泡动态大小调整方案\n"
                       "聊天气泡需根据文本长度自适应宽度，同时限制最大宽度防止拉伸过度，具体实现步骤如下：\n"
                       "1. **计算理想宽度**：通过 `QLabel::fontMetrics().boundingRect(text).width()` 获取文本单行显示所需宽度；\n"
                       "2. **限制最大宽度**：设置最大宽度为聊天区域宽度的 2/3（如 `ui->chatArea->viewport()->width()*2/3`），使用 `qMin(理想宽度, 最大宽度)` 确定最终标签宽度；\n"
                       "3. **更新尺寸**：调用 `QLabel::setFixedWidth(最终宽度)` 和 `adjustSize()` 调整文本标签大小，同时触发父部件 `ChatBubbleWidget` 的尺寸更新。\n"
                       "\n"
                       "# 三、流式消息与 Markdown 格式支持扩展\n"
                       "为提升 AI 回复的可读性与交互性，可扩展两项核心功能：\n"
                       "- **流式消息优化**：逐字输出时需将 `QChar` 转为 `QString`（通过 `QString(currentChar)`），每次追加后调用 `appendText()` 更新标签内容，同时确保 `adjustBubbleSize()` 实时调整气泡大小；\n"
                       "- **Markdown 基础支持**：若需解析简单 Markdown 语法（如标题、列表），可借助 `QTextDocument` 或第三方库（如 `MarkdownQt`），将 Markdown 文本转为富文本后通过 `QLabel::setHtml()` 显示，需注意样式表与富文本格式的兼容性，避免冲突导致显示异常；\n"
                       "此外，需测试多场景下的稳定性，包括纯中文、中英文混排、超长无空格文本（如 URL），确保换行逻辑正常、无文本截断问题，同时验证窗口缩放时气泡布局的自适应效果。";

    /* QString text = "在Qt中开发聊天界面时，除了之前提到的聊天气泡动态大小调整，还有几个细节需要重点处理：首先是消息的时间戳显示，建议用QLabel单独承载，设置较小的字体（如12px）和浅灰色（#888888），放在气泡下方并与气泡左对齐（接收消息）或右对齐（发送消息），同时通过QHBoxLayout控制时间戳与气泡的间距（建议8px），避免视觉拥挤；其次是消息状态标识，比如“已读”“发送中”，可使用小图标（如QToolButton加载svg资源）搭配文字，放在气泡右侧，注意图标尺寸（建议16x16px）与文本基线对齐，防止布局偏移；\n"
        "另外，处理流式消息时，除了用定时器批量更新文本，还需考虑文本编码问题，确保特殊字符（如表情符号、中英文混排）正常显示，可通过QString的toUtf8()方法统一编码格式，同时在append文本后，不仅要调用ChatBubbleWidget的adjustSizeToContent()，还要检查父容器QScrollArea的滚动条状态，通过verticalScrollBar()->setValue(verticalScrollBar()->maximum())确保最新内容始终可见；\n"
                   "还有消息的选中与复制功能，默认QLabel不支持文本选中，需通过设置setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByMouse)开启，同时调整样式表避免选中时背景色与气泡色冲突，可设置selection-background-color为半透明蓝色（rgba(100,149,237,0.3)）；最后，测试时需覆盖多种场景，包括纯文本、含换行符文本、中英文混排文本，以及超长长句（如连续无空格的英文单词或URL链接），确保QTextLayout能正确处理换行逻辑，避免文本溢出气泡，同时验证窗口缩放时，所有消息气泡能跟随布局自适应，不会出现重叠或空白异常，这些细节处理好后，才能让聊天界面的用户体验更接近主流聊天软件。"; */

    QTimer *timer = new QTimer{};
    int idx = 0;
    int len = text.length();

    connect(timer,
            &QTimer::timeout,
            this,
            [=]() mutable {
                if (idx < len) {
                    resp->appendText(QString(text.at(idx)));
                    idx ++;
                } else {
                    timer->stop();
                    timer->deleteLater();
                    status_ = ChatStatus::INIT;
                }
    });

    timer->start(50);
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
    if (content_->text().length() % 10 == 0) {
        adjustBubbleSize();
    }
}

void ChatBubbleWidget::appendText(const QString &msg)
{
    content_->setText(content_->text()+msg);
    if (content_->text().length() % 10 == 0) {
        adjustBubbleSize();
    }
}

void ChatBubbleWidget::setMaxLineSize(int maxLineSize)
{
    maxLineSize_ = maxLineSize;
    content_->setMaximumWidth(maxLineSize_);
}
