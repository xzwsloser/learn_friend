#include "httpclient.h"
#include <QUrlQuery>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

HttpClient::HttpClient(const QString &configFileName,
                       QObject *parent):
    QObject(parent)
{
    sender_ = new QNetworkAccessManager{this};
    pool_ = new QThreadPool{this};
    pool_->setMaxThreadCount(MAXTHREADNUM);
    config_ = new Config{configFileName, this};
}

HttpClient::~HttpClient()
{

}


/*
 * @param url: 请求 URL
 * @param callback: 用于处理相应的回调函数
 * @param params: 键值对参数
 */
void HttpClient::Get(const QString &url,
                     std::unordered_map<QString, QString> params,
                     std::unordered_map<QString, QString> options,
                     ReplyHandlerType callback)
{
    QUrl urlToGet{url};

    QUrlQuery query;
    for (const std::pair<QString, QString>& param: params) {
        query.addQueryItem(param.first, param.second);
    }
    urlToGet.setQuery(query);

    QNetworkRequest req{urlToGet};

    for (const std::pair<QString, QString>& option: options) {
        req.setRawHeader(option.first.toUtf8(), option.second.toUtf8());
    }

    QNetworkReply *reply = sender_->get(req);

    connect(
        reply,
        &QNetworkReply::finished,
        [=]() -> void {
            if (callback != nullptr) {
                callback(reply);
            }
        }
    );
}

/*
 * @param url: 请求 URL
 * @param callback: 处理相应的回调函数
 * @param body: 请求体
 */
void HttpClient::PostWithJson(const QString &url,
                              const QJsonObject &body,
                              std::unordered_map<QString, QString> options,
                              ReplyHandlerType callback)
{
    QUrl urlToPost{url};
    QNetworkRequest req{urlToPost};
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    for (const std::pair<QString, QString>& option: options) {
        req.setRawHeader(option.first.toUtf8(), option.second.toUtf8());
    }

    QJsonDocument doc{body};
    QByteArray bodyJsonStr = doc.toJson();

    QNetworkReply *reply = sender_->post(req, bodyJsonStr);

    connect(
        reply,
        &QNetworkReply::finished,
        [=]() -> void {
            if (callback != nullptr) {
                callback(reply);
            }
        }
    );
}

void HttpClient::chat(const QString &question,
                      ReplyHandlerType handler,
                      std::unordered_map<QString, QString> options)
{
    std::unordered_map<QString, QString> headers;
    headers["Authorization"] = QString("Bearer %1")
                                   .arg(config_->getAPIKey());

    QJsonObject payload;
    QJsonObject message;
    QJsonArray messageLists;

    message.insert("role", "user");
    message.insert("content", question);

    messageLists.append(message);

    payload.insert("model", config_->getChatModel());
    payload.insert("messages", messageLists);

    for (const std::pair<QString, QString>& param: options) {
        payload.insert(param.first, param.second);
    }

    this->PostWithJson(config_->getChatUrl(),
                       payload,
                       headers,
                       handler);
}

void HttpClient::stream(const QString &question,
                        ReplyHandlerType onready,
                        ReplyHandlerType onfinish,
                        std::unordered_map<QString, QString> options)
{
    QUrl url{config_->getChatUrl()};

    QJsonObject payload;
    QJsonObject message;
    QJsonArray messageLists;

    message.insert("role", "user");
    message.insert("content", question);

    messageLists.append(message);

    payload.insert("model", config_->getChatModel());
    payload.insert("messages", messageLists);
    payload.insert("stream", true);

    for (const std::pair<QString, QString>& option: options) {
        payload.insert(option.first, option.second);
    }

    QNetworkRequest req{url};

    std::unordered_map<QString, QString> headers;
    headers["Authorization"] = QString("Bearer %1")
                                   .arg(config_->getAPIKey());

    for (const std::pair<QString, QString>& header: headers) {
        req.setRawHeader(header.first.toUtf8(),
                         header.second.toUtf8());
    }

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc{payload};
    QNetworkReply *reply = sender_->post(req, doc.toJson());

    connect(
        reply,
        &QNetworkReply::readyRead,
        [=]() -> void {
            if (onready != nullptr) {
                onready(reply);
            }
    });

    connect(
        reply,
        &QNetworkReply::finished,
        [=]() -> void {
            if (onfinish != nullptr) {
                onfinish(reply);
            }
    });
}

QString HttpClient::getContentFormReply(QJsonObject reply)
{
    return reply.value("choices").toArray().at(0).toObject()
        .value("message").toObject().value("content").toString();
}

QString HttpClient::getWordFromStream(QByteArray chunk)
{
    chunk = chunk.remove(0, QString("data:").size());

    return QJsonDocument::fromJson(chunk).object().value("choices")
                           .toArray().at(0).toObject()
                           .value("delta").toObject().value("content").toString();
}


void ReplyHandler::run()
{
    if (reply_ != nullptr) {
        handler_(reply_);
    }
}

