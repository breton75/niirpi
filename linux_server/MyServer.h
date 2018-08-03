#ifndef _MyServer_h_
#define _MyServer_h_

#include <QObject>
#include <QTcpServer>

class QTcpServer;
class QTcpSocket;


// ======================================================================
class MyServer : public QObject {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    quint16     m_nNextBlockSize;

private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);

public:
    MyServer(int nPort, QObject* pwgt = 0);

public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();
};
#endif  //_MyServer_h_
