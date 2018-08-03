#ifndef IOTHREAD_H
#define IOTHREAD_H

#include <QThread>

class IOThread : public QThread
{
    Q_OBJECT
  public:
    IOThread(QObject *parent = 0);
  
  protected:
    void run();    
  
  signals:
    void haveData(QString data);  
  
  public slots:
    void newData(QString data);
    void threadFinished();
    
};

#endif // IOTHREAD_H
