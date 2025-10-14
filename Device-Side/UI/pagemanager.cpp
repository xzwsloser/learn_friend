#include "pagemanager.h"
#include "loginpage.h"
#include "mainpage.h"
#include <QVBoxLayout>

PageManager::PageManager()
{
    stackedWidget_ = new QStackedWidget{this};
    loginpage_ = new loginpage{stackedWidget_};
    mainpage_ = new MainPage{stackedWidget_};
    watcher_ = new WatcherPage{stackedWidget_};
    knowledge_ = new KnowledgePage{stackedWidget_};
    chatpage_ = new chatPage{stackedWidget_};

    stackedWidget_->addWidget(loginpage_);
    stackedWidget_->addWidget(mainpage_);
    stackedWidget_->addWidget(watcher_);
    stackedWidget_->addWidget(knowledge_);
    stackedWidget_->addWidget(chatpage_);

    QVBoxLayout *layout = new QVBoxLayout{this};
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(stackedWidget_);

    connect(
        loginpage_,
        &loginpage::transferToMainPage,
        this,
        &PageManager::transferToMainPage
    );

    connect(
        mainpage_,
        &MainPage::backToLoginPage,
        this,
        &PageManager::backToLoginPage
    );

    connect(
        mainpage_,
        &MainPage::toWatcher,
        this,
        &PageManager::fromMainToWatcher
    );

    connect(
        watcher_,
        &WatcherPage::backToMain,
        this,
        &PageManager::watcherBackToMain
    );

    connect(
        mainpage_,
        &MainPage::toKnowledge,
        this,
        &PageManager::fromMainToKnowledge
    );

    connect(
        knowledge_,
        &KnowledgePage::backToMain,
        this,
        &PageManager::knowledgeBackToMain
    );

    connect(
        chatpage_ ,
        &chatPage::backToMain,
        this,
        &PageManager::chatBackToMain
    );

    connect(
        mainpage_,
        &MainPage::toChat,
        this,
        &PageManager::fromMainToChat
    );

    curpage_ = PageStatus::LOGIN;
    stackedWidget_->setCurrentIndex(static_cast<int>(curpage_));

    showFullScreen();
}

PageManager::~PageManager()
{
    if (loginpage_ != nullptr) {
        delete loginpage_;
        loginpage_ = nullptr;
    }

    if (mainpage_ != nullptr) {
        delete mainpage_;
        mainpage_ = nullptr;
    }
}

void PageManager::transferToMainPage(const QString &username)
{
    username_ = username;
    mainpage_->setUsername(username);
    curpage_ = PageStatus::MAIN;
    stackedWidget_->setCurrentIndex(static_cast<int>(curpage_));
}

void PageManager::backToLoginPage()
{
    username_ = "";
    curpage_ = PageStatus::LOGIN;
    loginpage_->clearText();
    watcher_->clearAllText();
    knowledge_->clearAllText();
    chatpage_->clearMessage();
    stackedWidget_->setCurrentIndex(static_cast<int>(curpage_));
}

void PageManager::fromMainToWatcher()
{
    watcher_->clearAllText();
    curpage_ = PageStatus::WATCHER;
    stackedWidget_->setCurrentIndex(static_cast<int>(curpage_));
}

void PageManager::watcherBackToMain()
{
    watcher_->clearAllText();
    curpage_ = PageStatus::MAIN;
    stackedWidget_->setCurrentIndex(static_cast<int>(curpage_));
}

void PageManager::fromMainToKnowledge()
{
    knowledge_->clearAllText();
    curpage_ = PageStatus::KNOWLEDGE;
    stackedWidget_->setCurrentIndex(static_cast<int>(curpage_));
}

void PageManager::knowledgeBackToMain()
{
    knowledge_->clearAllText();
    curpage_ = PageStatus::MAIN;
    stackedWidget_->setCurrentIndex(static_cast<int>(curpage_));
}

void PageManager::fromMainToChat()
{
    curpage_ = PageStatus::CHAT;
    stackedWidget_->setCurrentIndex(static_cast<int>(curpage_));
}

void PageManager::chatBackToMain()
{
    curpage_ = PageStatus::MAIN;
    stackedWidget_->setCurrentIndex(static_cast<int>(curpage_));
}
