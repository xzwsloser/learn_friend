#include "pagemanager.h"
#include "network/httpclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HttpClient *client = new HttpClient;
    PageManager pm;
    pm.setAllHttpClient(client);
    pm.show();
    return a.exec();
}

