// #include "pagemanager.h"
#include "watcherpage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // PageManager pm;
    // pm.show();
    WatcherPage w;
    w.show();
    return a.exec();
}
