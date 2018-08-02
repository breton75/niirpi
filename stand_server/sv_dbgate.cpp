#include "sv_dbgate.h"

extern SvPGDB* PGDB;

SvDBGate::SvDBGate(DBGateParams &params, clog::SvCLog &log)
{
  _params = params;
  _log = log;
  
  _signals.clear();
  
  connect(this, &SvDBGate::writeToLog, &_log, &clog::SvCLog::write);
  
}

SvDBGate::~SvDBGate()
{
  stop();
  deleteLater();  
}

bool SvDBGate::init()
{
  try {
    
    PGDB = new SvPGDB();
    PGDB->setConnectionParams(_params.db_name, _params.host, _params.port,
                              _params.login, _params.pass);
    
    QSqlError err = PGDB->connectToDB();
    
    if(err.type() != QSqlError::NoError) _exception.raise(err.text());
       
    return true;
    
  }
  
  catch(SvException& e) {
    
    _last_error = e.error;
    
    return false;
    
  }
}

void SvDBGate::run()
{
  _started = true;
  _finished = false;
    
  while(_started) {
    
    for(SvSignal* signal: _signals) {
      
      bool signal_alive = (signal->lastUpdate()->isValid() &&
                          (signal->lastUpdate()->toMSecsSinceEpoch() + signal->params()->timeout < QDateTime::currentMSecsSinceEpoch()));
      
      try {
        
        if( !signal_alive ) {
          
          signal->setValue(signal->params()->timeout_value);
          
        }
        
        QSqlError serr = PGDB->execSQL(QString(SQL_UPDATE_REPOSITORY)
                                       .arg(_params.table_name)
                                       .arg(signal->value())
                                       .arg(signal->lastUpdate()->toString("dd/MM/yyyy hh:mm:ss.zzz"))
                                       .arg(signal->params()->cob_id));
        
        if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
        
      }
      catch(SvException& e) {
        
        emit writeToLog(e.error);
        
      }
      
    }
    
  }
  
  _finished = true;
  
}

void SvDBGate::stop()
{
  _started = false; 
  while(!_finished) QCoreApplication::instance()->processEvents();
}
