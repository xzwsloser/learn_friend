#ifndef CONFIG_H
#define CONFIG_H
#include <QObject>
#include <QJsonObject>

class Config: public QObject {
    Q_OBJECT
public:
    explicit Config(const QString &fileName,
                    QObject *parent = nullptr);
    ~Config();

    QString getAPIKey(const QString &provider = "SiliconFlow");
    QString getChatUrl(const QString &provier = "SiliconFlow");
    QString getChatModel(const QString &provider = "SiliconFlow");

private:
    QJsonObject configure_;
};

#endif
