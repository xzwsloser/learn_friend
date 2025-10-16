#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include "network/httpclient.h"

namespace Ui {
class MainPage;
}

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = nullptr);

    void setUsername(const QString &username);
    ~MainPage();

    void setHttpClient(HttpClient *client) { client_ = client; }

signals:
    void backToLoginPage();
    void toWatcher();
    void toKnowledge();
    void toChat();

protected:
    void paintEvent(QPaintEvent *ev) override;
    bool eventFilter(QObject *watched, QEvent *ev) override;

private:
    void updateTime();
    Ui::MainPage *ui;
    QString username_{"xxx"};
    HttpClient *client_;
};

#endif // MAINPAGE_H
