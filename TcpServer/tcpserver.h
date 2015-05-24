#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QObject>
#include <QStringList>
//#include <QDialog>

//class QLabel;
//class QPushButton;
class QTcpServer;
class QNetworkSession;

class TcpServer : public QObject// : public QDialog
{
      Q_OBJECT

   public:
      TcpServer(QObject *parent = 0);

   private slots:
      void sessionOpened();
      void sendFortune();

   private:
//      QLabel *m_statusLabel;
//      QPushButton *m_quitButton;
      QTcpServer *m_tcpServer;
      QStringList m_fortunes;
      QNetworkSession *m_networkSession;
};

#endif
