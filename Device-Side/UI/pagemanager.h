#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H
#include <QObject>
#include "loginpage.h"
#include "mainpage.h"
#include "watcherpage.h"
#include "knowledgepage.h"
#include "chatpage.h"
#include "network/httpclient.h"
#include <QStackedWidget>

enum PageStatus: int {
    LOGIN = 0,
    MAIN = 1,
    WATCHER = 2,
    KNOWLEDGE = 3,
    CHAT = 4
};

class PageManager: public QWidget {
   Q_OBJECT
public:
   PageManager();
   ~PageManager();

   void setHttpClient(HttpClient *client) { client_ = client; }
   HttpClient* getHttpClient() const { return client_; }

   void setAllHttpClient(HttpClient *client) {
       client_ = client;
       loginpage_->setHttpClient(client);
       mainpage_->setHttpClient(client);
       watcher_->setHttpClient(client);
       knowledge_->setHttpClient(client);
       chatpage_->setHttpClient(client);
   }

private:
   loginpage *loginpage_;
   MainPage *mainpage_;
   WatcherPage *watcher_;
   KnowledgePage *knowledge_;
   chatPage *chatpage_;
   QString username_;
   QStackedWidget *stackedWidget_;
   PageStatus curpage_;
   HttpClient *client_;

private slots:
   void transferToMainPage(const QString &username);
   void backToLoginPage();
   void fromMainToWatcher();
   void watcherBackToMain();
   void fromMainToKnowledge();
   void knowledgeBackToMain();
   void fromMainToChat();
   void chatBackToMain();

};

#endif // PAGEMANAGER_H
