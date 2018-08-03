#ifndef IOTHREAD_H
#define IOTHREAD_H

#include <QThread>
#include <QCoreApplication>
#include "consoleapplication.h"


class IOThread : public QThread
{
//  Q_OBJECT
    
  public:
    explicit IOThread(): QThread(0) {} 
    
  public slots:
    void newData(QString data);
    void threadFinished();
    
  signals:
    void haveData(QString data);
    
  protected:
    void run();

};



#endif // IOTHREAD_H
