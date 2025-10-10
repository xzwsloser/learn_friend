#ifndef PROMPTPAGE_H
#define PROMPTPAGE_H

#include <QWidget>

namespace Ui {
class PromptPage;
}

class PromptPage : public QWidget
{
    Q_OBJECT

public:
    explicit PromptPage(QWidget *parent = nullptr);
    ~PromptPage();
    void setMarkdownInfo(const QString &msg);

signals:
    void save();
    void cancel();

protected:
    void paintEvent(QPaintEvent *ev) override;

private:
    Ui::PromptPage *ui;
};

#endif // PROMPTPAGE_H
