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

#include "sv_repository.h"

extern SvSQLITE* SQLITE;

QMap<int, idev::SvIDevice*> DEVICES;
QMap<int, SvRepository*> REPOSITORIES;
QMap<int, SvSignal*> SIGNALS;

clog::SvCLog lout;
SvException exception;

bool initConfig();

bool readDevices();
bool readRepositories();
bool readCOBs();
bool readSignals();

idev::SvIDevice* create_device(const QSqlQuery* q);
SvRepository* create_repository(const QSqlQuery* q);
SvSignal* create_signal(const QSqlQuery* q);

bool openDevices();
bool initRepositories();


QString config_path;


int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  
//  clog::SvCLog log = clog::SvCLog();
  
  /** открываем БД, читаем конфигурацию **/
  if(!initConfig()) return -1;
  
  /** читаем устройства, репозитории и сигналы. СИГНАЛЫ В ПОСЛЕДНЮЮ ОЧЕРЕДЬ! **/
  if(!readDevices()) return -1;
  if(!readRepositories()) return -1;
  if(!readCOBs()) return -1;
  if(!readSignals()) return -1;
  
  /** подключаемся к устройствам и к репозиториям и начинаем работу **/
  if(!openDevices()) return -1;
  if(!initRepositories()) return -1;
  
  return a.exec();
}


bool initConfig()
{
  try {
    
#ifdef APP_DEBUG
  config_path = QString("D:/c++/NIIRPI/configurator/config.db");
#else
  config_path = QString("%1/config.db").arg(qApp->applicationDirPath());
#endif
      
    lout << clog::Time << QString("Открываем файл конфигурации: ") << config_path << clog::endl;
    
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
  lout << clog::Time << "Читаем устройства" << clog::endl;
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  try {
    
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_DEVICES_LIST, q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    while(q->next()) { 
       
      idev::SvIDevice* newdev = create_device(q);
      
      if(newdev)
        DEVICES.insert(newdev->config()->id, newdev);
      
      else
        lout << QString("Не удалось добавить устройство %1 (id %2)")
                                        .arg(q->value("device_name").toString())
                                        .arg(q->value("device_id").toInt())
             << clog::endl;
      
//        exception.raise(QString("Не удалось добавить устройство %1 (id %2)")
//                        .arg(q->value("device_name").toString())
//                        .arg(q->value("device_id").toInt()));
    }
    q->finish();
    
    lout << QString("\tOK") << clog::endl;
    
    delete q;
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
  lout << clog::Time << "Читаем репозитории" << clog::endl;
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  try {
    
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_REPOSITORIES_LIST, q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    while(q->next()) { 
       
      SvRepository* newrep = create_repository(q);
      
      if(newrep)
        REPOSITORIES.insert(newrep->params()->id, newrep);
      
      else
        lout << QString("Не удалось добавить репозиторий %1 (id %2)")
                        .arg(q->value("repository_name").toString())
                        .arg(q->value("repository_id").toInt())
             << clog::endl;
        
    }
    q->finish();
    
    lout << QString("\tOK") << clog::endl;
    
    delete q;
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    lout << QString("\tОшибка: %1").arg(e.error) << clog::endl;
    return false;
    
  }
}

bool readCOBs()
{
  lout << clog::Time << "Читаем идентификаторы" << clog::endl;
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  try {
    
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_COB_IDS, q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    while(q->next()) { 
       
      int cob_id = q->value("signal_cob_id").toInt();
      
      foreach (SvRepository* r, REPOSITORIES.values())
        r->addCOB(cob_id);
        
    }
    q->finish();
    
    lout << QString("\tOK") << clog::endl;
    
    delete q;
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
  lout << clog::Time << "Читаем сигналы" << clog::endl;
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  try {
    
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_SIGNALS_LIST, q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    while(q->next()) { 
       
      SvSignal* newsig = create_signal(q);
      
      if(newsig) {
        
        SIGNALS.insert(newsig->params()->id, newsig);
        
        if(DEVICES.find(newsig->params()->device_id) != DEVICES.end())
          DEVICES.value(newsig->params()->device_id)->addSignal(newsig);
          
        if(REPOSITORIES.find(newsig->params()->major_repository_id) != REPOSITORIES.end())      
          REPOSITORIES.value(newsig->params()->major_repository_id)->addSignal(newsig);
        
      }
      
      else
        exception.raise(QString("Не удалось добавить сигнал %1 (id %2)")
                        .arg(q->value("signal_name").toString())
                        .arg(q->value("signal_id").toInt()));
        
    }
    q->finish();
    
    lout << QString("\tOK") << clog::endl;
    
    delete q;
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    lout << QString("\tОшибка: %1").arg(e.error) << clog::endl;
    return false;
    
  }
  
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

SvRepository *create_repository(const QSqlQuery* q)
{
  SvRepository* newrep = nullptr;
  
  RepositoryParams params;
  params.id = q->value("repository_id").toInt();
  params.name = q->value("repository_name").toString();
  params.host = q->value("repository_host").toString();
  params.port = q->value("repository_port").toUInt();
  params.login = q->value("repository_login").toString();
  params.pass = q->value("repository_pass").toString();
  params.db_name = q->value("repository_dbname").toString();
  params.table_name = q->value("repository_table_name").toString();
//  params.description = q->value("signal_description").toString();

  try {
    
    newrep = new SvRepository(params, lout);
    
    return newrep;
    
  }
  
  catch(SvException& e) {
    
    if(newrep)
      delete newrep;
    
    lout << QString("\tОшибка: %1").arg(e.error)
         << clog::endl;
    
    return 0;
    
  }
}

SvSignal* create_signal(const QSqlQuery* q)
{
  SvSignal* newsig = nullptr;
  
  SignalParams params;
  params.id = q->value("signal_id").toInt();
  params.cob_id = q->value("signal_cob_id").toInt();
  params.name = q->value("signal_name").toString();
  params.device_id = q->value("signal_device_id").toInt();
  params.device_data_length = q->value("signal_device_data_length").toInt();
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


bool openDevices()
{
  lout << clog::Time << "Открываем устройства" << clog::endl;
  
  for(int key: DEVICES.keys()) {\
    
    idev::SvIDevice* device = DEVICES.value(key);
    
    if(!device->open()) {
      
      lout << QString("\t%1 (%2): Ошибка: %2")
                                .arg(device->config()->name)
                                .arg(device->lastError())
           << clog::endl;
      
      return false;
      
    }
      
    lout << QString("\t%1 (%2): OK").arg(device->config()->name).arg(device->config()->kts_name)
         << clog::endl;
      
  }
  
  return true;
  
}

bool initRepositories()
{
  lout << clog::Time << "Подключаемся к репозиториям" << clog::endl;
  
  for(int key: REPOSITORIES.keys()) {\
    
    SvRepository* repository = REPOSITORIES.value(key);
    
    if(!repository->init()) {
      
      lout << QString("\t%1 (%2): Ошибка: %2")
                                .arg(repository->params()->name)
                                .arg(repository->lastError())
           << clog::endl;
      
      return false;
      
    }
      
    lout << QString("\t%1 (%2:%3): OK")
            .arg(repository->params()->name)
            .arg(repository->params()->host)
            .arg(repository->params()->port)
         << clog::endl;
      
  }
  
  return true;
  
}
