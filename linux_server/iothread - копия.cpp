#include <QString>
#include "iothread.h"
#include "consoleapplication.h"
#include <QTextStream>

using namespace std;

QTextStream qtin1(stdin, QIODevice::ReadOnly);
QTextStream qtout1(stdout, QIODevice::WriteOnly);

void IOThread::run()
{
  connect(this, SIGNAL(haveData(QString)), this, SLOT(newData(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(finished()), this, SLOT(threadFinished()), Qt::QueuedConnection);
  
  QString line = "";
  while (line != "q")
  {
    qtin1 >> line;
    emit haveData(line);
  }
}

void IOThread::newData(QString data)
{
  qtout1 << data << endl << ">> ";
}

void IOThread::threadFinished()
{
  QEvent *e = new QEvent(AppExitEventType);
  QCoreApplication::postEvent(QCoreApplication::instance(), e);
}
