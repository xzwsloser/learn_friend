#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H
#include <QObject>
#include "loginpage.h"
#include "mainpage.h"
#include "watcherpage.h"
#include <QStackedWidget>

enum PageStatus: int {
    LOGIN = 0,
    MAIN = 1,
    WATCHER = 2
};

class PageManager: public QWidget {
   Q_OBJECT
public:
   PageManager();
   ~PageManager();
private:
   loginpage *loginpage_;
   MainPage *mainpage_;
   WatcherPage *watcher_;
   QString username_;
   QStackedWidget *stackedWidget_;
   PageStatus curpage_;

private slots:
   void transferToMainPage(const QString &username);
   void backToLoginPage();
   void fromMainToWatcher();
   void watcherBackToMain();
};

#endif // PAGEMANAGER_H
