#ifndef CONSOLEAPPLICATION_H
#define CONSOLEAPPLICATION_H

#include <QCoreApplication>
#include <QTextStream>#include <ios>
#include <iostream>

const QEvent::Type AppExitEventType = static_cast<QEvent::Type>(QEvent::User + 1);

class ConsoleApplication : public QCoreApplication
{
  public:
    ConsoleApplication(int argc, char ** argv):QCoreApplication(argc, argv) {}
    
  protected:
    void customEvent (QEvent *event)
    {
      if (event->type() == AppExitEventType)
        QCoreApplication::quit();
      else
        QCoreApplication::customEvent(event);
    }
};

#endif // CONSOLEAPPLICATION_H
