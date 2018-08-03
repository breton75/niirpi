#include "console_socket.h"
#include "../common/console_log.h"
//#include <QApplication>
//#include <QDataStream>
#include <QDateTime>
#include <ios>
#include <iostream>

using namespace clog_ns;
using namespace connection_errors;

SocketConnection::SocketConnection(QObject *parent, 
                                   QString sip, 
                                   quint16 sport,
                                   QString scommand) :
  QObject(parent)
{
  this->ip = sip;
  this->port = sport;
  this->command = scommand;
  this->connected = false;
  
  this->socket = new QTcpSocket(this);

//  connect(this->socket, SIGNAL(connected()), this, SLOT(slotConnected()));
  connect(this->socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
  connect(this->socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
  connect(this->socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotStateChanged(QAbstractSocket::SocketState)));
  
}

int SocketConnection::_connect()
{
  try
  {
    if (this->ip == "") throw CE_WRONG_IP;
    if (this->port == 0) throw CE_WRONG_PORT;
    
    if (!this->connected)
      {    
        this->socket->connectToHost(this->ip, this->port);
        if (socket->waitForConnected(2000))
          {
            this->connected = true;
            return 0;
          }
        else throw CE_BAD_CONNECTION;
      }
  }
 
  catch (int err)
  {
    switch (err)
    {  
      case CE_BAD_CONNECTION:
        clog(m_Err, QString("Can not connect to %1:%2\n").arg(this->ip).arg(this->port));
        break;      
      
      case CE_WRONG_IP:
        clog(m_Err, "Wrong IP adress\n");
        break;
        
      case CE_WRONG_PORT:
        clog(m_Err, "Wrong port\n");
        break;        
        
      case CE_ALREADY_CONNECTED:
        clog(m_Err, QString("Host %1 already connected\n").arg(this->ip));
        break;       
    }
    
    return err;
  }
 
}

void SocketConnection::slotConnected()
{
  this->connected = true;
  clog(m_Data, "Connected to " + this->ip + ':' + this->port);
}

void SocketConnection::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(this->socket->errorString())
                    );
    clog(m_Err, strError);
}

void SocketConnection::slotReadyRead()
{
//    QDataStream in( this->socket);
//    in.setVersion(QDataStream::Qt_5_2);

    QString str = "";

    int i;
    QByteArray  arrBlock;

    arrBlock = this->socket->readAll();
    arrBlock = arrBlock.toHex();

//    log(m_Info, QString("arrBlock.length() = %1").arg(arrBlock.length()));
//    log(m_Err, QString("%1").arg(sizeof(int)));

    for (i=0; i < arrBlock.size(); i = i+2)
      {
          str = str + arrBlock.at(i) + arrBlock.at(i + 1) + ' ';//
      }

    clog(m_Data, "Received data:\n" + str);
}

void SocketConnection::slotStateChanged(QAbstractSocket::SocketState state)
{
  switch (state)
    {
    case QAbstractSocket::UnconnectedState:
      clog(m_Err, "Unconnected");
      this->connected = false;
//      QApplication::processEvents();
      break;
      
    case QAbstractSocket::HostLookupState:
      clog(m_Info, QString("Lokup fo %1").arg(this->ip));
//      QApplication::processEvents();
      break;   

    case QAbstractSocket::ConnectingState:
      clog(m_Info, QString("Connecting fo %1").arg(this->ip));
//      QApplication::processEvents();
      break;
      
    case QAbstractSocket::ConnectedState:
      this->connected = true;
      clog(m_Info, QString("Connected to %1").arg(this->ip));
//      QApplication::processEvents();
      break;

    case QAbstractSocket::BoundState:
      clog(m_Info, QString("Bounded fo %1:%2").arg(this->ip).arg(this->port));
//      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ClosingState:
      clog(m_Info, "Closing connection");
//      QApplication::processEvents();
      break;       
    }
}

int SocketConnection::send_command(QString command, bool wait_for_answer, QString *answer)

{
  try
  {
    if (!this->connected) { throw CE_NOT_CONNECTED; }
 
    clog(m_Data, command);
    
    QByteArray arrBlock;
    arrBlock.append(command);
    this->socket->write(arrBlock);
    
    
  }
  catch (int err)
  {
    
  }
  
}

int SocketConnection::send_command()
{
//  clog(m_Data, command);
  try
  {
    if (!this->connected) { 
      this->_connect();
      if (!this->connected) throw CE_NOT_CONNECTED;
    }
 
    clog(m_Data, command);
    
    QByteArray arrBlock;
    arrBlock.append(command);
    this->socket->write(arrBlock);
    
    
  }
  catch (int err)
  {
    
  }
  
}
