#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H
#include <QObject>
#include "loginpage.h"
#include "mainpage.h"
#include "watcherpage.h"
#include "knowledgepage.h"
#include <QStackedWidget>

enum PageStatus: int {
    LOGIN = 0,
    MAIN = 1,
    WATCHER = 2,
    KNOWLEDGE = 3
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
   KnowledgePage *knowledge_;
   QString username_;
   QStackedWidget *stackedWidget_;
   PageStatus curpage_;

private slots:
   void transferToMainPage(const QString &username);
   void backToLoginPage();
   void fromMainToWatcher();
   void watcherBackToMain();
   void fromMainToKnowledge();
   void knowledgeBackToMain();
};

#endif // PAGEMANAGER_H
