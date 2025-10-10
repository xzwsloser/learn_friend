#ifndef WATCHERPAGE_H
#define WATCHERPAGE_H

#include <QWidget>
#include "softkeyboard.h"

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

protected:
    void paintEvent(QPaintEvent *ev) override;
    bool eventFilter(QObject *watched, QEvent *ev) override;

private:
    Ui::WatcherPage *ui;
    WatcherParams *params_;
    SoftKeyboard *keyboard_;
    WatcherStatus status_{WatcherStatus::UNSTARTED};
    bool showCamera_{false};
    QLabel *frame;
    QTimer *timer_;

    void updateTime();
    void showMessage(const QString &prompt, bool normal_, int interval = 2000);
    void updateStatus();

private slots:
    void toggleScreen();
    void startWatcher();
    void stopWatcher();
    void endWatcher();
};

#endif // WATCHERPAGE_H
