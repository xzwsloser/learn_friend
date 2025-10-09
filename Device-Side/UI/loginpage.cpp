#include "loginpage.h"
#include "./ui_loginpage.h"
#include <QFile>
#include <QPainter>
#include <qtmaterialtextfield.h>
#include <QDebug>
#include <QTimer>
#include <QPropertyAnimation>
#include <QLineEdit>
#include <QInputMethod>
#include <QGuiApplication>

loginpage::loginpage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::loginpage)
{
    ui->setupUi(this);

    // init the keyboard
    this->keyboard = new SoftKeyboard;

    // set the style of the page
    QFile file{":/qss/loginpage.qss"};
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString loginpage_style = QLatin1String(file.readAll());
        ui->titleLabel->setStyleSheet(loginpage_style);
        ui->loginWidget->setStyleSheet(loginpage_style);
        ui->loginTitleLabel->setStyleSheet(loginpage_style);
        file.close();
    }

    // set the style of the qt-material
    ui->username_editor->setLabel("用户名");
    // ui->username_editor->setStyleSheet("font-size: 80px");
    QFont font = ui->username_editor->font();
    font.setPointSize(40);
    ui->username_editor->setFont(font);

    ui->username_editor->setLabelFontSize(15);
    ui->username_editor->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->password_editor->setLabel("密码");
    ui->password_editor->setEchoMode(QLineEdit::Password);
    // ui->password_editor->setStyleSheet("font-size: 80px");
    font = ui->password_editor->font();
    font.setPointSize(40);
    ui->password_editor->setFont(font);

    ui->password_editor->setLabelFontSize(15);
    ui->password_editor->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->login_button->setBackgroundColor(QColor{"#388E3C"});
    ui->register_button->setBackgroundColor(QColor{"#FF9800"});
    ui->login_button->setFontSize(30);
    ui->register_button->setFontSize(30);

    auto callback = [=]() -> void {
            QString username = ui->username_editor->text();
            QString password = ui->password_editor->text();
            verifyPassword(username, password);
    };

    connect(ui->login_button,
            &QPushButton::clicked,
            this,
            callback);

    connect(ui->register_button,
            &QPushButton::clicked,
            this,
            callback);

    this->ui->username_editor->installEventFilter(this);
    this->ui->password_editor->installEventFilter(this);

    this->frame = new QLabel(this);
}

loginpage::~loginpage()
{
    delete ui;
    if (this->keyboard != nullptr) {
        delete this->keyboard;
        this->keyboard = nullptr;
    }
}

void loginpage::paintEvent(QPaintEvent *ev)
{
    QPainter painter{this};
    QPixmap pix{":/img/login-background.png"};
    painter.drawPixmap(this->rect(), pix);
    QWidget::paintEvent(ev);
}

bool loginpage::eventFilter(QObject *watched, QEvent *ev)
{
    if (ev->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mev = static_cast<QMouseEvent *>(ev);
        if (mev->button() == Qt::LeftButton) {
            QPoint pos = this->pos() +
                         ui->loginWidget->pos() +
                         ui->fillwidget->pos();
            QPoint delta = QPoint(100, 250);
            if (watched == ui->username_editor) {
                keyboard->hideInputBufferArea(ui->username_editor);
                keyboard->resize(this->width()/2, this->height()/2);
                keyboard->move(pos +
                               ui->usernamefillwidget->pos() +
                               ui->username_editor->pos() +
                               delta);
                keyboard->show();
            } else if (watched == ui->password_editor) {
                connect(
                    keyboard,
                    &SoftKeyboard::sendInputBufferAreaText,
                    this,
                    [=](QString text) -> void {
                        this->ui->password_editor->setText(text);
                    }
                );
                keyboard->showInputBufferArea("请输入你的密码:");
                keyboard->resize(this->width()/2, this->height()/2);
                keyboard->move(pos +
                               ui->passwordfillwidget->pos() +
                               ui->password_editor->pos() +
                               delta);
                keyboard->show();
            }
        }
    }
    return QWidget::eventFilter(watched, ev);
}

void loginpage::showMessage(const QString &msg, bool normal, int interval)
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

void loginpage::verifyPassword(const QString &username, const QString &password)
{
    if (username == "狂热的学zha" &&
        password == "123321abccba")
    {
        showMessage("登录成功", true);
        emit transferToMainPage(username);
    } else {
        showMessage("用户名或密码错误", false);
    }
}

void loginpage::clearText()
{
    ui->username_editor->clear();
    ui->password_editor->clear();
}
