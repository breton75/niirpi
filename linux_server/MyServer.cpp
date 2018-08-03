#include "MyServer.h"
#include <ios>
#include <iostream>
#include <QtNetwork>

using namespace std;

// ----------------------------------------------------------------------
MyServer::MyServer(int nPort, QObject *pwgt /*=0*/) : QObject(pwgt), m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this); 
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        
      cout << "Unable to start the server: " + m_ptcpServer->errorString().toStdString();    
//      qDebug() << "Unable to start the server:" + m_ptcpServer->errorString();
        

        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()), 
            this,         SLOT(slotNewConnection())
           );
     cout << "server created";
//    qDebug() << "server created";
}

// ----------------------------------------------------------------------
/*virtual*/ void MyServer::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()), 
            this,          SLOT(slotReadClient())
           );

    sendToClient(pClientSocket, "Server Response: Connected!");
}

// ----------------------------------------------------------------------
void MyServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_3);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < (int)sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> time >> str;

        QString strMessage = 
            time.toString() + " " + "Client has sent - " + str;
        qDebug() << strMessage;
        
//        m_ptxt->append(strMessage);

        m_nNextBlockSize = 0;

        sendToClient(pClientSocket, 
                     "Server Response: Received \"" + str + "\""
                    );
    }
}

// ----------------------------------------------------------------------
void MyServer::sendToClient(QTcpSocket* pSocket, const QString& str)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << quint16(0) << QTime::currentTime() << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}
