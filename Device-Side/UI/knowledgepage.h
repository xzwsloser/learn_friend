#ifndef KNOWLEDGEPAGE_H
#define KNOWLEDGEPAGE_H

#include <QWidget>
#include "knowledgeretrievepage.h"
#include "network/httpclient.h"

namespace Ui {
class KnowledgePage;
}

enum KnowledgePageStatus {
    RETRIEVED = 0,
    PHOTO
};

class KnowledgePage : public QWidget
{
    Q_OBJECT

public:
    explicit KnowledgePage(QWidget *parent = nullptr);
    ~KnowledgePage();

    void clearAllText();

    void setHttpClient(HttpClient *client) { client_ = client; }

signals:
    void backToMain();

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;

private:
    Ui::KnowledgePage *ui;
    KnowledgeRetrievePage *retrievedPage_;
    KnowledgePageStatus curpage_{KnowledgePageStatus::RETRIEVED};
    HttpClient *client_;

    void updateTime();
};

#endif // KNOWLEDGEPAGE_H
