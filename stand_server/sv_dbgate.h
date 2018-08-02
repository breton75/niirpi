#ifndef SV_DBGATE_H
#define SV_DBGATE_H

#include <QObject>
#include <QThread>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>


#include "../../svlib/sv_pgdb.h"
#include "../../svlib/sv_clog.h"
#include "../../svlib/sv_exception.h"

#include "../global/sql_defs.h"
#include "../global/sv_signal.h"

struct DBGateParams
{
  DBGateParams() {}
  
  int id = -1;
  QString name = "";
  QString host = "";
  quint32 port = 5432;
  QString login = "";
  QString pass = "";
  QString db_name = "";
  QString table_name = "";
};

class SvDBGate: public QThread
{
    Q_OBJECT
  
public:
  SvDBGate(DBGateParams& params, clog::SvCLog& log);
  ~SvDBGate();
  
  bool init();
  
  void run() Q_DECL_OVERRIDE;
  void stop();
  
  QString lastError() { return _last_error; }
  
  void addSignal(SvSignal* signal) { _signals.append(signal); }
  
  int signalsCount() { return _signals.count(); }
  
private:
  DBGateParams _params;
  clog::SvCLog _log;
  QString _last_error = "";
  
  bool _started = false;
  bool _finished = false;
  
  SvException _exception;
  
  QList<SvSignal*> _signals;
  
signals:
  void writeToLog(QString& text);
  
};

#endif // SV_DBGATE_H
