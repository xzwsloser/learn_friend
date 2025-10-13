#include "knowledgeretrievepage.h"
#include "ui_knowledgeretrievepage.h"
#include <QPainter>
#include <QFile>
#include <QDebug>

KnowledgeRetrievePage::KnowledgeRetrievePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KnowledgeRetrievePage)
{
    ui->setupUi(this);

    /* init */
    keyboard_ = new SoftKeyboard;

    /* voice Button */
    ui->voiceButton->setFixedSize(120, 120);

    /* qss */
    QFile file{":/qss/knowledge_retrieved.qss"};
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

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

    ui->voiceButton->setStyleSheet(voiceButtonStyles[static_cast<int>(curstatus_)]);
    ui->voiceButton->setText("");

    ui->buttonLayout->setAlignment(ui->voiceButton, Qt::AlignCenter);
    ui->buttonLayout->setAlignment(ui->certainButton, Qt::AlignCenter);
    ui->buttonLayout->setAlignment(ui->clearButton, Qt::AlignCenter);

    ui->certainButton->setFixedSize(120, 80);
    ui->clearButton->setFixedSize(120, 80);

    ui->inputEdit->installEventFilter(this);

    ui->zoomIn->setFixedSize(120, 80);
    ui->zoomOut->setFixedSize(120, 80);

    connect(
        ui->voiceButton,
        &QPushButton::clicked,
        this,
        &KnowledgeRetrievePage::voiceRecord
    );

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
        &KnowledgeRetrievePage::sendAndRetrieve
    );

    /* document scroll area */
    QVBoxLayout *vlayout = new QVBoxLayout{ui->documentMainWidget};
    vlayout->setSpacing(30);
    vlayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    /* show area */
    showLabel_ = new QLabel{ui->showContent};
    QVBoxLayout *showlayout = new QVBoxLayout{ui->showContent};
    showlayout->addWidget(showLabel_);

    connect(
        ui->zoomOut,
        &QPushButton::clicked,
        this,
        [=]() -> void {
            zoom(true);
        }
    );

    connect(
        ui->zoomIn,
        &QPushButton::clicked,
        this,
        [=]() -> void {
            zoom(false);
        }
    );

}

KnowledgeRetrievePage::~KnowledgeRetrievePage()
{
    delete ui;
}

void KnowledgeRetrievePage::paintEvent(QPaintEvent *ev)
{
    QPainter painter{this};
    painter.setRenderHint(QPainter::Antialiasing, true);

    /* background */
    QColor bgColor{240, 249, 232, 180};
    QRectF rect = this->rect();
    rect.adjust(4, 4, -4, -4);

    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, 16, 16);

    /* frame */
    QPen borderPen{QColor{30, 144, 255}, 3};
    borderPen.setStyle(Qt::SolidLine);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect, 16, 16);

    QWidget::paintEvent(ev);
}

void KnowledgeRetrievePage::voiceRecord()
{
    if (curstatus_ == VoiceStatus::RAW) {
        curstatus_ = VoiceStatus::TAKING;
        ui->voiceButton->setStyleSheet(voiceButtonStyles[static_cast<int>(curstatus_)]);
        /* TODO: 启动语音模型 */

    } else if (curstatus_ == VoiceStatus::TAKING) {
        curstatus_ = VoiceStatus::RAW;
        ui->voiceButton->setStyleSheet(voiceButtonStyles[static_cast<int>(curstatus_)]);
        /* TODO: 收集语音模型结果 */

    }
}

bool KnowledgeRetrievePage::eventFilter(QObject *watched, QEvent *ev)
{
    if (watched == ui->inputEdit) {
        if (ev->type() == QEvent::FocusIn) {
                /* show button */
                keyboard_->hideInputBufferArea(ui->inputEdit);
                keyboard_->resize(this->width()/2,
                                  this->height()/2);
                int x = (this->width() - keyboard_->width())/2;
                int y = (this->height() - keyboard_->height())/2;
                keyboard_->move(x,y);
                keyboard_->show();
        }
    }

    for (DocumentWidget *document: documents_) {
        if (watched == document) {
            if (ev->type() == QEvent::MouseButtonRelease) {
                QMouseEvent *mev = static_cast<QMouseEvent*>(ev);
                if (mev->button() == Qt::LeftButton) {
                    /* render document */
                    qInfo() << "clicked";
                    renderDocument(document);
                    qInfo() << "after";
                    break;
                }
            }
        }
    }

    return QWidget::eventFilter(watched, ev);
}

void KnowledgeRetrievePage::renderDocument(const DocumentWidget *document)
{
    /* clear */
    showLabel_ -> clear();

    QFont font = showLabel_->font();
    font.setPointSize(20);
    showLabel_->setFont(font);

    if (document->type() == ResourceType::TEXT) {
        showLabel_->setAlignment(Qt::AlignCenter);
        showLabel_->setText(document->getContent());
        initPixmap_ = QPixmap{};
    } else if (document->type() == ResourceType::IMAGE) {
        showLabel_->setPixmap(document->getImage());
        initPixmap_ = document->getImage();
    } else if (document->type() == ResourceType::MARKDOWN) {
        showLabel_->setTextFormat(Qt::MarkdownText);
        showLabel_->setAlignment(Qt::AlignCenter);
        showLabel_->setText(document->getMarkdown());
        initPixmap_ = QPixmap{};
    }

}

void KnowledgeRetrievePage::sendAndRetrieve()
{

    /* delete the document  */
    for (DocumentWidget *document: documents_) {
        if (document != nullptr) {
            ui->documentMainWidget->layout()->removeWidget(document);
            delete document;
        }
    }

    documents_.clear();


    /* send request */

    /* Mark Documents and Insert Into QScrollArea */
    DocumentWidget *textWidget = new DocumentWidget{};
    DocumentWidget *imageWidget = new DocumentWidget{};
    DocumentWidget *markdownWidget = new DocumentWidget{};
    DocumentWidget *imageWidget1 = new DocumentWidget{};

    /* Test */
    QString text{"春天是一个充满生机与希望的季节，万物复苏，百花争艳。\n"
             "当第一缕春风拂过大地，沉睡的种子便在土壤中苏醒，开始孕育新的生命。\n"
             "柳树抽出嫩绿的新芽，随风摇曳，仿佛在向人们招手示意；桃花绽放出粉红的笑脸，\n"
             "散发着淡淡的清香，吸引着蜜蜂前来采蜜。公园里，人们脱下厚重的冬装，换上轻便的春装，\n"
             "有的在散步聊天，有的在放风筝，孩子们的欢声笑语回荡在空气中，充满了活力与喜悦。\n"
             "\n"
             "夏天是一个热情似火的季节，骄阳似火，蝉鸣阵阵。\n"
             "正午时分，太阳像一个巨大的火球，炙烤着大地，柏油马路都仿佛要融化一般。\n"
             "但这丝毫阻挡不了大自然的生机，荷花在池塘中亭亭玉立，碧绿的荷叶像一把把撑开的大伞，\n"
             "为鱼儿遮挡阳光。傍晚时分，暑气渐消，人们纷纷来到广场上乘凉，\n"
             "老人们摇着蒲扇讲述着过去的故事，年轻人则随着音乐翩翩起舞，享受着夏夜的美好。\n"
             "\n"
             "秋天是一个硕果累累的季节，金风送爽，五谷丰登。\n"
             "田野里，金黄色的稻穗沉甸甸地弯下了腰，仿佛在向辛勤劳作的农民伯伯鞠躬致谢；\n"
             "果园中，红彤彤的苹果、黄澄澄的梨子挂满枝头，散发着诱人的果香，让人垂涎欲滴。\n"
             "树叶也换上了五彩斑斓的盛装，红色的枫叶、黄色的银杏叶随风飘落，\n"
             "像一只只翩翩起舞的蝴蝶，给大地铺上了一层厚厚的地毯。\n"
             "\n"
             "冬天是一个银装素裹的季节，寒风呼啸，雪花纷飞。\n"
             "一夜之间，大地就变成了粉妆玉砌的世界，房屋戴上了洁白的帽子，树木披上了晶莹的铠甲，\n"
             "整个世界都变得洁白无瑕。孩子们兴奋地跑到户外，堆雪人、打雪仗，\n"
             "欢声笑语打破了冬日的宁静。虽然冬天寒冷，但也孕育着希望，\n"
             "因为它告诉我们，春天已经不远了。\n"
             "\n"
             "四季的轮回，就像人生的起伏，各有各的精彩，各有各的韵味。\n"
             "春天教会我们播种希望，夏天教会我们热情奔放，秋天教会我们收获感恩，冬天教会我们积蓄力量。\n"
             "我们应该珍惜每一个季节，感受每一个季节的美好，\n"
             "在四季的更迭中感悟生命的真谛，让自己的人生也像四季一样丰富多彩。\n"
             "\n"
             "除了四季的美景，生活中还有许多值得我们珍惜的瞬间。\n"
             "清晨醒来，看到窗外第一缕阳光洒进房间，心中便会涌起一丝温暖；\n"
             "傍晚回家，闻到厨房里传来的饭菜香味，便会感受到家的温馨；\n"
             "和朋友相聚，谈天说地，分享喜怒哀乐，便会体会到友谊的珍贵；\n"
             "和家人相伴，共度时光，互相关爱，便会明白亲情的可贵。\n"
             "\n"
             "生活就像一本书，每一页都记录着不同的故事；生活又像一幅画，\n"
             "每一笔都描绘着不同的色彩。我们每个人都是自己生活的作者和画家，\n"
             "可以用自己的方式书写精彩的故事，描绘绚丽的画卷。\n"
             "无论遇到什么困难和挫折，都要保持乐观的心态，相信风雨过后总会有彩虹，\n"
             "只要心中有阳光，生活就会充满希望。\n"
             "\n"
             "让我们以积极的态度面对生活，用热情拥抱每一个崭新的日子，\n"
             "用真诚对待身边的每一个人，用努力追求自己的梦想。\n"
             "相信在不久的将来，我们都会收获属于自己的幸福和成功，\n"
             "让生命之花绽放出最绚丽的光彩，让人生之路铺满鲜花和掌声。\n"
             "这就是生活的意义，这就是我们应该追求的人生。"};

    QString MarkdownStr{"# 计算机科学基础\n"
                    "\n"
                    "## 1. 数据结构\n"
                    "\n"
                    "### 1.1 线性结构\n"
                    "- 数组（Array）：连续内存空间，随机访问效率高\n"
                    "- 链表（Linked List）：非连续存储，插入删除灵活\n"
                    "- 栈（Stack）：后进先出（LIFO）结构\n"
                    "- 队列（Queue）：先进先出（FIFO）结构\n"
                    "\n"
                    "### 1.2 非线性结构\n"
                    "- 树（Tree）\n"
                    "  - 二叉树：每个节点最多有两个子节点\n"
                    "  - 红黑树：自平衡二叉查找树\n"
                    "  - 堆：完全二叉树的一种实现\n"
                    "- 图（Graph）\n"
                    "  - 有向图：边有方向的图结构\n"
                    "  - 无向图：边无方向的图结构\n"
                    "\n"
                    "## 2. 算法基础\n"
                    "\n"
                    "### 2.1 排序算法\n"
                    "| 算法名称 | 时间复杂度 | 空间复杂度 | 稳定性 |\n"
                    "| :------- | :--------- | :--------- | :----- |\n"
                    "| 冒泡排序 | O(n²)      | O(1)       | 稳定   |\n"
                    "| 快速排序 | O(nlogn)   | O(logn)    | 不稳定 |\n"
                    "| 归并排序 | O(nlogn)   | O(n)       | 稳定   |\n"
                    "\n"
                    "### 2.2 查找算法\n"
                    "- 顺序查找：逐个比对，适用于无序数据\n"
                    "- 二分查找：要求数据有序，时间复杂度O(logn)\n"
                    "- 哈希查找：通过哈希函数直接定位，平均O(1)\n"
                    "\n"
                    "## 3. 计算机网络\n"
                    "\n"
                    "> OSI七层模型是网络通信的基础框架，从上到下分别为：应用层、表示层、会话层、传输层、网络层、数据链路层、物理层。\n"
                    "\n"
                    "### 3.1 TCP/IP协议族\n"
                    "- TCP：面向连接的可靠传输协议\n"
                    "- UDP：无连接的不可靠传输协议\n"
                    "- IP：负责网络层的寻址和路由\n"
                    "- HTTP：应用层超文本传输协议\n"
                    "\n"
                    "## 4. 编程语言分类\n"
                    "\n"
                    "1. 编译型语言\n"
                    "   - C/C++：执行效率高，适合系统开发\n"
                    "   - Go：并发性能优异，适合后端开发\n"
                    "\n"
                    "2. 解释型语言\n"
                    "   - Python：语法简洁，适合数据分析\n"
                    "   - JavaScript：网页交互核心语言\n"
                    "\n"
                    "## 5. 代码示例\n"
                    "\n"
                    "```cpp\n"
                    "// 计算斐波那契数列\n"
                    "int fibonacci(int n) {\n"
                    "    if (n <= 1) return n;\n"
                    "    return fibonacci(n-1) + fibonacci(n-2);\n"
                    "}\n"
                    "```\n"
                    "\n"
                    "## 6. 学习资源推荐\n"
                    "\n"
                    "- [算法导论](https://mitpress.mit.edu/books/introduction-algorithms)：算法领域经典教材\n"
                    "- [计算机网络：自顶向下方法](https://www.pearson.com/us/higher-education/program/Kurose-Computer-Networking-A-Top-Down-Approach-7th-Edition/PGM321323.html)：网络学习首选\n"
                    "- [数据结构与算法分析](https://www.amazon.com/Introduction-Algorithms-3rd-MIT-Press/dp/0262033844)：深入讲解数据结构\n"
                    "\n"
                    "## 7. 总结\n"
                    "\n"
                    "计算机科学是一门不断发展的学科，核心包括：\n"
                    "- 扎实的理论基础\n"
                    "- 良好的编程习惯\n"
                    "- 解决问题的能力\n"
                    "\n"
                    "持续学习和实践是掌握这门学科的关键。"};

    QString url{":/img/avatar.png"};
    QString url1{":/img/login-background.png"};

    textWidget->setContent(text);
    markdownWidget->setMarkdown(MarkdownStr);
    imageWidget->setImage(url);
    imageWidget1->setImage(url1);

    textWidget->setPrompt("Document 1 [TEXT]");
    markdownWidget->setPrompt("Document 2 [MARKDOWN]");
    imageWidget->setPrompt("Document 3 [IMAGE]");
    imageWidget1->setPrompt("Document 4 [IMAGE]");

    documents_.push_back(textWidget);
    documents_.push_back(markdownWidget);
    documents_.push_back(imageWidget);
    documents_.push_back(imageWidget1);

    /* render document */
    for (DocumentWidget *document: documents_) {
        document->installEventFilter(this);
        document->setFixedSize(ui->documentScrollArea->viewport()->width()*5/6,
                            260);
        ui->documentMainWidget->layout()->addWidget(document);
    }
}

void KnowledgeRetrievePage::resizeEvent(QResizeEvent *ev)
{
    if (!documents_.empty()) {
        for (DocumentWidget *document: documents_) {
            document->setFixedSize(ui->documentScrollArea->viewport()->width()*5/6,
                                   260);
        }
    }

    QWidget::resizeEvent(ev);
}

DocumentWidget::DocumentWidget(QWidget *parent):
    QWidget(parent)
{
    label_ = new QLabel{this};
    QHBoxLayout *layout = new QHBoxLayout{this};
    layout->addWidget(label_);

    label_->setAlignment(Qt::AlignCenter);
    QFont font = label_->font();
    font.setBold(true);
    font.setPointSize(15);
    label_->setFont(font);
}

DocumentWidget::~DocumentWidget()
{

}

void DocumentWidget::setContent(const QString &msg)
{
    content_ = msg;
    type_ = ResourceType::TEXT;
}

void DocumentWidget::setImage(const QString &url)
{
    pix_ = QPixmap{url};
    type_ = ResourceType::IMAGE;
}

void DocumentWidget::setMarkdown(const QString &text)
{
    content_ = text;
    type_ = ResourceType::MARKDOWN;
}

void DocumentWidget::setPrompt(const QString &prompt)
{
    label_->setText(prompt);
}

void DocumentWidget::paintEvent(QPaintEvent *ev)
{

    QPainter painter(this);
    // 启用抗锯齿，使边缘更平滑
    painter.setRenderHint(QPainter::Antialiasing);

    // 获取部件的矩形区域
    QRect widgetRect = rect();

    // 绘制背景 - 浅黄绿色
    QBrush bgBrush(QColor(245, 255, 240));
    painter.setBrush(bgBrush);
    painter.setPen(Qt::NoPen); // 背景不需要边框
    painter.drawRect(widgetRect);

    // 绘制边框 - 深绿色，宽度为2px
    QPen borderPen(QColor(0, 100, 0), 2);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush); // 边框不需要填充

    // 调整矩形大小，避免边框超出部件范围
    QRect borderRect = widgetRect.adjusted(0, 0, -1, -1);
    painter.drawRect(borderRect);

    QWidget::paintEvent(ev);
}

void KnowledgeRetrievePage::zoom(bool type)
{
    if (showLabel_->text().isEmpty() &&
        showLabel_->pixmap(Qt::ReturnByValue).isNull()) return ;

    bool showImage = showLabel_->text().isEmpty();

    if (type) {
        if (showImage) {
            /* Zoom Out Image */
            QPixmap curPixmap = showLabel_->pixmap(Qt::ReturnByValue);
            QSize curImageSize = curPixmap.size();
            QSize newImageSize{curImageSize.width()*6/5,
                               curImageSize.height()*6/5};
            QPixmap newPixmap = initPixmap_.scaled(newImageSize,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation);
            showLabel_->clear();
            showLabel_->setPixmap(newPixmap);
        } else {
            /* Zoom Out Text */
            QFont font = showLabel_->font();
            int fontSize;
            if ((fontSize = font.pointSize()) >= 50) return ;
            font.setPointSize(fontSize+1);
            showLabel_->setFont(font);
        }
    } else {
        if (showImage) {
            QPixmap curPixmap = showLabel_->pixmap(Qt::ReturnByValue);
            QSize curImageSize = curPixmap.size();
            QSize newImageSize{curImageSize.width()*5/6,
                               curImageSize.height()*5/6};
            QPixmap newPixmap = initPixmap_.scaled(newImageSize,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation);
            showLabel_->clear();
            showLabel_->setPixmap(newPixmap);
        } else {
            /* Zoom In Text */
            QFont font = showLabel_->font();
            int fontSize;
            if ((fontSize = font.pointSize()) <= 5) return ;
            font.setPointSize(fontSize-5);
            showLabel_->setFont(font);
        }
    }
}




