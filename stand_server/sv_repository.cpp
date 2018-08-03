#include "sv_repository.h"

extern SvPGDB* PGDB;

SvRepository::SvRepository(RepositoryParams &params, clog::SvCLog &log)
{
  _params = params;
  _log = log;
  
  _signals.clear();
  _cobs.clear();
  
  connect(this, &SvRepository::writeToLog, &_log, &clog::SvCLog::write);
  
}

SvRepository::~SvRepository()
{
  stop();
  deleteLater();  
}

bool SvRepository::init()
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

void SvRepository::run()
{
  _started = true;
  _finished = false;
    
  while(_started) {
    
    for(SvSignal* signal: _signals) {
      
      bool signal_alive = (signal->lastUpdate()->isValid() &&
                          (signal->lastUpdate()->toMSecsSinceEpoch() + signal->params()->timeout < QDateTime::currentMSecsSinceEpoch()));
        
      if(!signal_alive)
        signal->setValue(signal->params()->timeout_value);
      
      _cobs.value(signal->params()->cob_id)->setSignal(signal);
        
    }
    
    QString upd_query  ="";
    
    for(int key: _cobs.keys()) {
      
      upd_query += QString(SQL_UPDATE_VALUE)
                   .arg(_params.table_name)
                   .arg(_cobs.value(key)->value)
                   .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss.zzz"))
                   .arg(key);
      
    }
    
      
    try {
      
      PGDB->transaction();
      
      QSqlError serr = PGDB->execSQL(upd_query);
      
      if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
      
      PGDB->commit();
      
    }
    
    catch(SvException& e) {
      
      PGDB->rollback();
      emit writeToLog(e.error);
      
    }
    
  }
  
  _finished = true;
  
}

void SvRepository::stop()
{
  _started = false; 
  while(!_finished) QCoreApplication::instance()->processEvents();
}
