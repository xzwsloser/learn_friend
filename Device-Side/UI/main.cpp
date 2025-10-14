// #include "pagemanager.h"
#include "chatpage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // PageManager pm;
    // pm.show();
    chatPage c;
    c.show();
    return a.exec();
}
