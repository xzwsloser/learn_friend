#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include <QFocusEvent>
#include "softkeyboard.h"
#include "network/httpclient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class loginpage;
}
QT_END_NAMESPACE

class loginpage : public QWidget
{
    Q_OBJECT

public:
    loginpage(QWidget *parent = nullptr);
    ~loginpage();

    void showMessage(const QString &msg, bool normal, int interval = 2000);
    void clearText();

    void setHttpClient(HttpClient *client) { client_ = client; }

signals:
    void transferToMainPage(const QString &username);

protected:
    void paintEvent(QPaintEvent *ev) override;
    bool eventFilter(QObject *watched, QEvent *ev) override;

private:
    Ui::loginpage *ui;
    SoftKeyboard *keyboard;
    QLabel *frame;
    HttpClient *client_;

    void verifyPassword(const QString &username,
                        const QString &password);
};
#endif // LOGINPAGE_H
