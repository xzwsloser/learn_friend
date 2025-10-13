#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>

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

signals:
    void backToLoginPage();
    void toWatcher();
    void toKnowledge();

protected:
    void paintEvent(QPaintEvent *ev) override;
    bool eventFilter(QObject *watched, QEvent *ev) override;

private:
    void updateTime();
    Ui::MainPage *ui;
    QString username_{"xxx"};
};

#endif // MAINPAGE_H
