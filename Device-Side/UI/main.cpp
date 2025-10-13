#include "pagemanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PageManager pm;
    pm.show();
    return a.exec();
}
