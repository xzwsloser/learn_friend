#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H
#include <QObject>
#include "loginpage.h"
#include "mainpage.h"
#include <QStackedWidget>

enum PageStatus: int {
    LOGIN = 0,
    MAIN = 1
};

class PageManager: public QWidget {
   Q_OBJECT
public:
   PageManager();
   ~PageManager();
private:
   loginpage *loginpage_;
   MainPage *mainpage_;
   QString username_;
   QStackedWidget *stackedWidget_;
   PageStatus curpage_;

private slots:
   void transferToMainPage(const QString &username);
   void backToLoginPage();
};

#endif // PAGEMANAGER_H
