#include <QApplication>
#include <QtCore>

#include <stdlib.h>

#include "tcpserver.h"

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   TcpServer server;
   //server.show();
   qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
   return app.exec();
}
