#ifndef KNOWLEDGEPAGE_H
#define KNOWLEDGEPAGE_H

#include <QWidget>
#include "knowledgeretrievepage.h"

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

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;

private:
    Ui::KnowledgePage *ui;
    KnowledgeRetrievePage *retrievedPage_;
    KnowledgePageStatus curpage_{KnowledgePageStatus::RETRIEVED};

    void updateTime();
};

#endif // KNOWLEDGEPAGE_H
