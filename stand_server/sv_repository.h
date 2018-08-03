#ifndef SV_DBGATE_H
#define SV_DBGATE_H

#include <QObject>
#include <QThread>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QtMath>

#include "../../svlib/sv_pgdb.h"
#include "../../svlib/sv_clog.h"
#include "../../svlib/sv_exception.h"

#include "../global/sql_defs.h"
#include "../global/sv_signal.h"

struct RepositoryParams
{
  RepositoryParams() {}
  
  int id = -1;
  QString name = "";
  QString host = "";
  quint32 port = 5432;
  QString login = "";
  QString pass = "";
  QString db_name = "";
  QString table_name = "";
};

struct COB
{
  COB(int cobid) { id = cobid; }
  int id;
  quint64 value = 0;
  void setSignal(SvSignal* signal) { value |= (quint64(qPow(2, signal->params()->data_length) - 1) << signal->params()->data_offset); }
  void clear() { value = 0; }
};

class SvRepository: public QThread
{
    Q_OBJECT
  
public:
  SvRepository(RepositoryParams& params, clog::SvCLog& log);
  ~SvRepository();
  
  bool init();
  
  void run() Q_DECL_OVERRIDE;
  void stop();
  
  QString lastError() { return _last_error; }
  RepositoryParams* params() { return &_params; }
  
  void addSignal(SvSignal* signal) { _signals.insert(signal->params()->id, signal); }
  void clearSignals() { _signals.clear(); }
  QMap<int, SvSignal*>* getSignals() { return &_signals; }
  int signalsCount() { return _signals.count(); }
  
  void addCOB(int id) { _cobs.insert(id, new COB(id)); }
  
private:
  RepositoryParams _params;
  clog::SvCLog _log;
  QString _last_error = "";
  
  bool _started = false;
  bool _finished = false;
  
  SvException _exception;
  
  QMap<int, SvSignal*> _signals;
  QMap<int, COB*> _cobs;
  
signals:
  void writeToLog(QString& text);
  
};

#endif // SV_DBGATE_H
