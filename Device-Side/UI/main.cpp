// #include "pagemanager.h"
#include "knowledgepage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // PageManager pm;
    // pm.show();
    KnowledgePage page;
    page.show();
    return a.exec();
}
