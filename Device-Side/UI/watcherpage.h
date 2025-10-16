#ifndef WATCHERPAGE_H
#define WATCHERPAGE_H

#include <QWidget>
#include "softkeyboard.h"
#include "network/httpclient.h"
#include "promptpage.h"

namespace Ui {
class WatcherPage;
}

typedef struct WatcherParams {
    int watcherTime_{0};
    int passedTime_{0};
    bool attention_{false};
    bool distracted_{false};
    bool other_{false};
} WatcherParams;

enum WatcherStatus {
    UNSTARTED,
    WATCHING,
    STOPED,
    ENDED
};

class WatcherPage : public QWidget
{
    Q_OBJECT

public:
    explicit WatcherPage(QWidget *parent = nullptr);
    ~WatcherPage();
    void clearAllText();

    void setHttpClient(HttpClient *client) { client_ = client; }

signals:
    void backToMain();

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    bool eventFilter(QObject *watched, QEvent *ev) override;

private:
    Ui::WatcherPage *ui;
    WatcherParams *params_;
    SoftKeyboard *keyboard_;
    WatcherStatus status_{WatcherStatus::UNSTARTED};
    bool showCamera_{false};
    QLabel *frame;
    PromptPage *promptPage_;
    QTimer *timer_;
    QString msgTemplate_{""};
    HttpClient *client_;

    void updateTime();
    void showMessage(const QString &prompt, bool normal_, int interval = 2000);
    void updateStatus();
    void showPrompt(const QString &attentionMsg,
                    const QString &distractedMsg,
                    const QString &otherMsg);

private slots:
    void toggleScreen();
    void startWatcher();
    void stopWatcher();
    void endWatcher();
    void save();
    void cancel();
};

#endif // WATCHERPAGE_H
