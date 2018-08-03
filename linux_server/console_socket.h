#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QAbstractSocket>
//#include <QTime>

namespace connection_errors {
  const int CE_BAD_CONNECTION = -1;
  const int CE_WRONG_IP = 1;
  const int CE_WRONG_PORT = 2;
  const int CE_CONNECTION_FAULT = 3;
  const int CE_ALREADY_CONNECTED = 4;
  const int CE_NOT_CONNECTED = 5;
       
}


class SocketConnection : public QObject
{
  Q_OBJECT
  
  
  
  
public:
  
  QTcpSocket *socket;
  
  bool connected;
  QString ip;
  quint16 port;
  QString command;
  
  explicit SocketConnection(QObject *parent = 0, 
                            QString sip = "", 
                            quint16 sport = 0,
                            QString scommand = "");
      
  int _connect();
  
  int disconnect();
  
//  connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), , SLOT(socketStateChanged()));
  
signals:
//  void socketStateChanged(QAbstractSocket::SocketState) { ;
  
public slots:
  void slotConnected();
  void slotError(QAbstractSocket::SocketError err);
  void slotReadyRead();
  void slotStateChanged(QAbstractSocket::SocketState state);
  int send_command(QString command, bool wait_for_answer, QString* answer);
  int send_command();
  
};

#endif // SOCKETCONNECTION_H
