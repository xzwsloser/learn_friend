#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QThreadPool>
#include "config.h"

#define MAXTHREADNUM 5

class ReplyHandler: public QRunnable {
public:
    ReplyHandler(std::function<void(QNetworkReply*)> handler):
        handler_(handler) {}

    ~ReplyHandler() {}

    void setReply(QNetworkReply *reply) { reply_ = reply; }
    QNetworkReply* getReply() const { return reply_; }

    void run() override;

private:
    std::function<void(QNetworkReply*)> handler_;
    QNetworkReply *reply_;
};

class HttpClient: public QObject {
    Q_OBJECT
public:
    HttpClient(const QString &configFileName = ":/config/config.json",
                        QObject *parent = nullptr);
    ~HttpClient();

    using ReplyHandlerType = std::function<void(QNetworkReply*)>;

    void Get(const QString &url,
             std::unordered_map<QString, QString> pairs = {},
             std::unordered_map<QString, QString> options = {},
             ReplyHandlerType callback = nullptr);

    void PostWithJson(const QString &url,
                      const QJsonObject &body,
                      std::unordered_map<QString, QString> options = {},
                      ReplyHandlerType callback = nullptr);

    void chat(const QString &question,
              ReplyHandlerType callback = nullptr,
              std::unordered_map<QString, QString> options = {});

    void stream(const QString &question,
                ReplyHandlerType onready,
                ReplyHandlerType onfinish,
                std::unordered_map<QString, QString> options = {});

    static QString getContentFormReply(QJsonObject reply);
    static QString getWordFromStream(QByteArray data);


private:
    QNetworkAccessManager *sender_;
    QThreadPool *pool_;
    Config *config_;
};

#endif
