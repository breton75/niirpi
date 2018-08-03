#include <QCoreApplication>
#include "MyServer.h"

#include "../common/tthread.h"
#include <QObject>
#include <console_socket.h>
#include "iothread.h"
#include "consoleapplication.h"

using namespace std;

void threadProgress();

int main(int argc, char *argv[])
{
  
  ConsoleApplication a(argc, argv);
  
  QString cmd;
  char c[20];
  int i;
  i = 100;

  IOThread iothr;// = new inThread;
  iothr.start();
  
//  TThread *thr = new TThread(1000);
////  MyServer *serv = new MyServer(35580);
//  SocketConnection *sock = new SocketConnection(0, "192.168.0.113", 35580, "GET:PARAMS");
//  sock->moveToThread(thr);
  
////  QObject::connect(thr, SIGNAL(progressed()), serv, SLOT(slotReadClient()));
//  QObject::connect(thr, SIGNAL(progressed()), sock, SLOT(send_command()));
//  QObject::connect(thr, SIGNAL(progressed()), thr, SLOT(ggg()));
// thr->start();

//  return a.exec();
}


















