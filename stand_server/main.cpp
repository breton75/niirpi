#include <QCoreApplication>
#include <QList>
#include <QLibrary>

#include "../global/sv_idevice.h"
#include "../global/sql_defs.h"
#include "../global/dev_defs.h"
#include "../global/sv_signal.h"

#include "../oht/sv_oht.h"

#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_clog.h"
#include "../../svlib/sv_exception.h"

#include "sv_dbgate.h"

extern SvSQLITE* SQLITE;

QList<idev::SvIDevice*> DEVICES;
QList<SvSignal*> SIGNALS;
QList<SvDBGate*> DBGATES;

clog::SvCLog lout;
SvException exception;

bool init();

bool readDevices();
bool readSignals();
bool readRepositories();

idev::SvIDevice* create_device(const QSqlQuery* q);
bool openDevices();

SvDBGate* create_dbgate(const QSqlQuery* q);
bool initDBs();

SvSignal* create_signal(const QSqlQuery* q);

QString config_path;


int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  
//  clog::SvCLog log = clog::SvCLog();
  
  /** открываем БД, читаем конфигурацию **/
  if(!init()) return -1;
  
  if(!readDevices()) return -1;
  
  if(!openDevices()) return -1;
  
  if(!readSignals()) return -1;
  
  return a.exec();
}


bool init()
{
  try {
    
#ifdef APP_DEBUG
  config_path = QString("D:/c++/NIIRPI/configurator/config.db");
#else
  config_path = QString("%1/config.db").arg(qApp->applicationDirPath());
#endif
      
    lout << clog::Time << QString("Инициализация БД: ") << config_path << clog::endl;
    
    SQLITE = new SvSQLITE(0, config_path);
    QSqlError serr = SQLITE->connectToDB();
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.databaseText());
    
    lout << "\tOK: " << clog::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    lout << "\tОшибка: " << e.error << clog::endl;
    return false;
  }
  
  
}

bool readDevices()
{
  lout << clog::Time << "Читаем список устройств" << clog::endl;
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  try {
    
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_DEVICES_LIST, q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    while(q->next()) { 
       
      idev::SvIDevice* newdev = create_device(q);
      
      if(newdev)
        DEVICES.append(newdev);
      
//      else
//        exception.raise(QString("Не удалось добавить устройство %1 (id %2)")
//                        .arg(q->value("device_name").toString())
//                        .arg(q->value("device_id").toInt()));
    }
    q->finish();
    
    lout << QString("\tOK") << clog::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    lout << QString("\tОшибка: %1").arg(e.error) << clog::endl;
    return false;
    
  }

}

bool readSignals()
{
  lout << clog::Time << "Читаем список сигналов" << clog::endl;
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  try {
    
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_SIGNALS_LIST, q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    while(q->next()) { 
       
      SvSignal* newsig = create_signal(q);
      
      if(newsig)
        SIGNALS.append(newsig);
      
      else
        exception.raise(QString("Не удалось добавить сигнал %1 (id %2)")
                        .arg(q->value("signal_name").toString())
                        .arg(q->value("signal_id").toInt()));
        
    }
    q->finish();
    
    lout << QString("\tOK") << clog::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    lout << QString("\tОшибка: %1").arg(e.error) << clog::endl;
    return false;
    
  }
  
}

bool readRepositories()
{
  
}


idev::SvIDevice* create_device(const QSqlQuery* q)
{  
  idev::SvIDevice* newdev = nullptr;
  
  idev::DeviceConfig config;
  config.id = q->value("device_id").toInt();
  config.name = q->value("device_name").toString();
  config.kts_id = q->value("device_kts_id").toInt();
  config.kts_name= q->value("device_kts_name").toString();
  config.ifc_id = q->value("device_ifc_id").toInt();
  config.ifc_name = q->value("device_ifc_name").toString();
  config.protocol_id = q->value("device_protocol_id").toInt();
  config.protocol_name = q->value("device_protocol_name").toString();
  config.data_type = q->value("device_data_type").toInt();
  config.data_type_name = q->value("device_data_type_name").toString();
  config.data_length = q->value("device_data_length").toInt();
  config.driver_lib_path = q->value("device_driver_lib_path").toString();
  config.description = q->value("device_kts_description").toString();

  QString params = q->value("device_connection_params").toString();
  try {
    
    switch (idev::DeviceTypes(q->value("device_kts_id").toUInt())) {
      
      case idev::sdtOHT_Gamma12700:
        
        newdev = new SvOHT();

        break;
        
        
      default:
        exception.raise(QString(" %1 (%2): Неизвестное устройство").arg(config.name).arg(config.kts_name));
        break;
    }
    
    if(!newdev->setConfig(config)) exception.raise(newdev->lastError());
    if(!newdev->setParams(params)) exception.raise(newdev->lastError());
    
    return newdev;
    
  }
  
  catch(SvException& e) {
    
    if(newdev)
      delete newdev;
    
    lout << QString("\tОшибка: %1").arg(e.error)
         << clog::endl;
    
    return 0;
    
  }
}

bool openDevices()
{
  lout << clog::Time << "Открываем устройства" << clog::endl;
  
  for(idev::SvIDevice* device: DEVICES) {
    
    if(!device->open()) {
      
      lout << QString("\t%1 (%2): Ошибка: %2")
                                .arg(device->config()->name)
                                .arg(device->lastError())
           << clog::endl;
      
      return false;
      
    }
    else {
      
      lout << QString("\t%1 (%2): OK").arg(device->config()->name).arg(device->config()->kts_name)
           << clog::endl;
      
      return true;
      
    }
  }
}

SvDBGate* create_dbgate(const QSqlQuery* q)
{
  
}

bool initDBs();

SvSignal* create_signal(const QSqlQuery* q)
{
  SvSignal* newsig = nullptr;
  
  SignalParams params;
  params.id = q->value("signal_id").toInt();
  params.cob_id = q->value("signal_cob_id").toInt();
  params.name = q->value("signal_name").toString();
  params.device_id = q->value("signal_device_id").toInt();
  params.device_name = q->value("signal_device_name").toString();
  params.kts_name = q->value("signal_kts_name").toString();
  params.timeout = q->value("signal_timeout").toInt();
  params.timeout_value = q->value("signal_timeout_value").toInt();
  params.data_offset = q->value("signal_data_offset").toInt();
  params.data_length = q->value("signal_data_length").toInt();
  params.data_type = SignalDataTypes(q->value("signal_data_type").toInt());
  params.major_repository_id = q->value("major_repository_id").toInt();
  params.minor_repository1_id = q->value("minor_repository1_id").toInt();
  params.minor_repository2_id = q->value("minor_repository2_id").toInt();
  params.minor_repository3_id = q->value("minor_repository3_id").toInt();
  params.description = q->value("signal_description").toString();

  try {
    
    newsig = new SvSignal(params);
    
    return newsig;
    
  }
  
  catch(SvException& e) {
    
    if(newsig)
      delete newsig;
    
    lout << QString("\tОшибка: %1").arg(e.error)
         << clog::endl;
    
    return 0;
    
  }
}
