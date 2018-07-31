#include <QCoreApplication>
#include <QList>
#include <QLibrary>

#include "../global/sv_idevice.h"
#include "../global/sql_defs.h"
#include "../global/dev_defs.h"

#include "../oht/sv_oht.h"

#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_clog.h"
#include "../../svlib/sv_exception.h"

QList<idev::SvIDevice*> DEVICES;
extern SvSQLITE* SQLITE;

clog::SvCLog lout;
SvException exception;

bool init();
bool readConfig();
idev::SvIDevice* create_device(const QSqlQuery* q);
bool openDevices();

QString config_path;


int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  
//  clog::SvCLog log = clog::SvCLog();
  
  /** открываем БД, читаем конфигурацию **/
  if(!init()) return -1;
  
  if(!readConfig()) return -1;
  
  if(!openDevices()) return -1;
  
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

bool readConfig()
{
  lout << clog::Time << "Читаем конфигурацию" << clog::endl;
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  try {
    
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_DEVICES_LIST, q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    while(q->next()) { 
//      if(!create_device(q)) exception.raise("Ошибка при загрузке конфигурации");
      create_device(q);
    }
    q->finish();
    
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
    
    DEVICES.append(newdev);
    
    lout << QString("\t%1 (%2): OK").arg(newdev->config()->name).arg(newdev->config()->kts_name)
         << clog::endl;
    
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
      
    }
    else {
      
      lout << QString("\t%1 (%2): OK").arg(device->config()->name).arg(device->config()->kts_name)
           << clog::endl;
      
    }
    
  }
}
