#include "propmtpage.h"
#include "ui_propmtpage.h"

propmtPage::propmtPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::propmtPage)
{
    ui->setupUi(this);
}

propmtPage::~propmtPage()
{
    delete ui;
}
