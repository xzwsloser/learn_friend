#ifndef PROPMTPAGE_H
#define PROPMTPAGE_H

#include <QWidget>

namespace Ui {
class propmtPage;
}

class propmtPage : public QWidget
{
    Q_OBJECT

public:
    explicit propmtPage(QWidget *parent = nullptr);
    ~propmtPage();

private:
    Ui::propmtPage *ui;
};

#endif // PROPMTPAGE_H
