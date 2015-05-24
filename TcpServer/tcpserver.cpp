//#include <QtWidgets>
#include <QDebug>
#include <QtNetwork>

#include <stdlib.h>

#include "tcpserver.h"

TcpServer::TcpServer(QObject* parent)
   :   QObject(parent), m_tcpServer(0), m_networkSession(0)
{
//   m_statusLabel = new QLabel;
//   m_quitButton = new QPushButton(tr("Quit"));
//   m_quitButton->setAutoDefault(false);

   QNetworkConfigurationManager manager;
   if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
      // Get saved network configuration
      QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
      settings.beginGroup(QLatin1String("QtNetwork"));
      const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
      settings.endGroup();

      // If the saved network configuration is not currently discovered use the system default
      QNetworkConfiguration config = manager.configurationFromIdentifier(id);
      if ((config.state() & QNetworkConfiguration::Discovered) !=
          QNetworkConfiguration::Discovered) {
         config = manager.defaultConfiguration();
      }

      m_networkSession = new QNetworkSession(config, this);
      connect(m_networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

//      m_statusLabel->setText(tr("Opening network session."));
      m_networkSession->open();
   } else {
      sessionOpened();
   }

   m_fortunes << tr("You've been leading a dog's life. Stay off the furniture.")
            << tr("You've got to think about tomorrow.")
            << tr("You will be surprised by a loud noise.")
            << tr("You will feel hungry again in another hour.")
            << tr("You might have mail.")
            << tr("You cannot kill time without injuring eternity.")
            << tr("Computers are not intelligent. They only think they are.");

//   connect(m_quitButton, SIGNAL(clicked()), this, SLOT(close()));
   connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(sendFortune()));

//   QHBoxLayout *buttonLayout = new QHBoxLayout;
//   buttonLayout->addStretch(1);
//   buttonLayout->addWidget(m_quitButton);
//   buttonLayout->addStretch(1);

//   QVBoxLayout *mainLayout = new QVBoxLayout;
//   mainLayout->addWidget(m_statusLabel);
//   mainLayout->addLayout(buttonLayout);
//   setLayout(mainLayout);

//   setWindowTitle(tr("Fortune Server"));
}

void TcpServer::sessionOpened()
{
   // Save the used configuration
   if (m_networkSession) {
      QNetworkConfiguration config = m_networkSession->configuration();
      QString id;
      if (config.type() == QNetworkConfiguration::UserChoice)
         id = m_networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
      else
         id = config.identifier();

      QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
      settings.beginGroup(QLatin1String("QtNetwork"));
      settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
      settings.endGroup();
   }

   m_tcpServer = new QTcpServer(this);
   if (!m_tcpServer->listen()) {
      qWarning() << QString("Unable to start the server: %1.")
                          .arg(m_tcpServer->errorString());
//      QMessageBox::critical(this, tr("Fortune Server"),
//                            tr("Unable to start the server: %1.")
//                            .arg(m_tcpServer->errorString()));
//      close();
      QCoreApplication::quit();
      return;
   }
   QString ipAddress;
   QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
   qDebug() << "All addresses:";
   for (int i = 0; i < ipAddressesList.size(); ++i) {
      if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
          ipAddressesList.at(i).toIPv4Address()) {
         qDebug() << "  :" << ipAddressesList.at(i);
      }
   }

   // use the first non-localhost IPv4 address
   for (int i = 0; i < ipAddressesList.size(); ++i) {
      if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
          ipAddressesList.at(i).toIPv4Address()) {
         ipAddress = ipAddressesList.at(i).toString();
         qDebug() << "non-localhost address:" << ipAddress;
         break;
      }
   }
   // if we did not find one, use IPv4 localhost
   if (ipAddress.isEmpty())
      ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
   qDebug() << QString("The server is running on\n\nIP: %1\nport: %2\n\n"
                       "Run the Fortune Client example now.")
                    .arg(ipAddress).arg(m_tcpServer->serverPort());
//   m_statusLabel->setText(tr("The server is running on\n\nIP: %1\nport: %2\n\n"
//                           "Run the Fortune Client example now.")
//                        .arg(ipAddress).arg(m_tcpServer->serverPort()));
}

void TcpServer::sendFortune()
{
   QByteArray block;
   QDataStream out(&block, QIODevice::WriteOnly);
   out.setVersion(QDataStream::Qt_4_0);
   out << (quint16)0;
   out << m_fortunes.at(qrand() % m_fortunes.size());
   out.device()->seek(0);
   out << (quint16)(block.size() - sizeof(quint16));

   QTcpSocket *clientConnection = m_tcpServer->nextPendingConnection();
   connect(clientConnection, SIGNAL(disconnected()),
           clientConnection, SLOT(deleteLater()));

   clientConnection->write(block);
   clientConnection->disconnectFromHost();
}
