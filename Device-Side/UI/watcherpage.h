#ifndef WATCHERPAGE_H
#define WATCHERPAGE_H

#include <QWidget>

namespace Ui {
class WatcherPage;
}

class WatcherPage : public QWidget
{
    Q_OBJECT

public:
    explicit WatcherPage(QWidget *parent = nullptr);
    ~WatcherPage();

protected:
    void paintEvent(QPaintEvent *ev) override;

private:
    Ui::WatcherPage *ui;
};

#endif // WATCHERPAGE_H
