#include "config.h"
#include <QFile>
#include <QJsonDocument>

Config::Config(const QString &fileName,
               QObject *parent):
    QObject(parent)
{
    QFile file{fileName};
    QString fileContent;
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        fileContent = QLatin1String(file.readAll());
        file.close();
    }

    QJsonDocument doc = QJsonDocument::fromJson(fileContent.toUtf8());
    configure_ = doc.object();
}

Config::~Config()
{

}

QString Config::getAPIKey(const QString &provider)
{
    QJsonObject obj = configure_.value(provider).toObject();
    return obj.value("APIKEY").toString();
}

QString Config::getChatUrl(const QString &provider)
{
    QJsonObject obj = configure_.value(provider).toObject();
    QJsonObject chatConfig = obj.value("Chat").toObject();
    return chatConfig.value("Url").toString();
}

QString Config::getChatModel(const QString &provider)
{
    QJsonObject obj = configure_.value(provider).toObject();
    QJsonObject chatConfig = obj.value("Chat").toObject();
    return chatConfig.value("Model").toString();
}

