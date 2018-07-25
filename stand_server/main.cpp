#include <QCoreApplication>
#include <QList>
#include <QLibrary>

#include "../devlib/sv_idevice.h"
#include "../devlib/sv_oht.h"

#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_exception.h"

QList<idev::SvISensor*> DEVICES;
extern SvSQLITE* SQLITE;

svlog::SvLog* log;
SvException exception;

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  
  log = new svlog::SvLog();
  
  /** открываем БД, читаем конфигурацию **/
  if(!init()) return -1;
  
  if(!readConfig()) return -1;
  
  return a.exec();
}


bool init()
{
  try {
    
    SQLITE = new SvSQLITE(this, "config.db");
    QSqlError serr = SQLITE->connectToDB();
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.databaseText());
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    log << e.error << svlog::endl;
    return false;
  }
  
  
}

bool readConfig()
{
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  try {
    
    /** заполняем список датчиков **/
    // rootItem создается при создании объекта модели
    // кол-во столбцов задается TREE_HEADERS и должно соответствовать кол-ву полей в таблице sensors
    TreeItem *root = _model->rootItem();

    int child_count = root->childCount();
    int column_count = root->columnCount();
    
    for(int i = 0; i < column_count; i++)
      root->setInfo(i, ItemInfo());
    
    
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_FROM_SENSORS, q);
    
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
    
    // заполняем модель выбранными данными
    while(q->next()) { 
      
      DEVICES.append(new );
      root->child(child_count)->setData(0, q->value("sensor_name"));
      root->child(child_count)->setInfo(0, ItemInfo(itSensorName, "sensor_name"));
      
      root->child(child_count)->setData(1, q->value("sensor_data_length"));
      root->child(child_count)->setInfo(1, ItemInfo(itSensorDataLength, "sensor_data_length"));
      
      root->child(child_count)->setData(2, QVariant());
      root->child(child_count)->setInfo(2, ItemInfo(itUndefined, ""));
      
      root->child(child_count)->setData(3, QString("\nИнтерфейс:\t%1\nПротокол:\t%2\nПорт:\t%3\nТип данных:\t%4")
                                          .arg(q->value("sensor_ifc_type_name").toString())
                                          .arg(q->value("sensor_ifc_protocol_name").toString())
                                          .arg(q->value("sensor_ifc_port_name").toString())
                                          .arg(q->value("sensor_data_type").toString()));
      root->child(child_count)->setInfo(3, ItemInfo(itSensorParams, ""));
      
      root->child(child_count)->setData(4, q->value("sensor_description").toString());
      root->child(child_count)->setInfo(4, ItemInfo(itSensorDescription, "sensor_description"));
      
//      for (int column = 0; column < column_count; column++) {
        
//        QString field_name = q->record().fieldName(column);
        
//        ItemInfo inf;
        
//        if(field_name == "sensor_id")                     inf.type = itSensorId;
//        else if(field_name == "sensor_name")              inf.type = itSensorName;
//        else if(field_name == "sensor_ifc_type_name")     inf.type = itSensorIfcType;
//        else if(field_name == "sensor_ifc_protocol_name") inf.type = itSensorIfcProtocol;
//        else if(field_name == "sensor_ifc_name")          inf.type = itSensorIfcName;
//        else if(field_name == "sensor_data_type")         inf.type = itSensorDataType;
//        else if(field_name == "sensor_data_length")       inf.type = itSensorDataLength;
//        else if(field_name == "sensor_description")       inf.type = itSensorDescription;
//        else                                              inf.type = itUndefined;
        
//        inf.fieldName = field_name;
        
//        root->child(child_count)->setInfo(column, inf);
        
//      }
    }
    q->finish();
    
    
    /** заполняем список сигналов **/
    TreeItem *sensor;
    
    for(int i = 0; i < root->childCount(); i++) {
      
      sensor = root->child(i);
      
      serr = SQLITE->execSQL(QString(SQL_SELECT_FROM_SIGNALS).arg(sensor->id), q);
      
      if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
      
      // заполняем модель выбранными данными
      while(q->next()) {
        
        child_count = sensor->childCount();
        
        sensor->insertChildren(child_count, 1, column_count);
        
        sensor->child(child_count)->id = q->value("signal_id").toInt();
        sensor->child(child_count)->parent_id = sensor->id;
        sensor->child(child_count)->is_main_row = false; //q->value("task_type").toInt() == 1; //  q->value("parent_task_id").toInt() == -1;
  //        root->child(child_count)->item_state = q->value("last_state").toInt();
        sensor->child(child_count)->item_type = itSignal;
  
        sensor->child(child_count)->setData(0, q->value("signal_name"));
        sensor->child(child_count)->setData(1, q->value("signal_data_length"));
        sensor->child(child_count)->setData(2, q->value("signal_data_offset"));
        sensor->child(child_count)->setData(4, q->value("signal_description"));
        
        for (int column = 0; column < column_count; column++) {
          
          QString field_name = q->record().fieldName(column);
          
          ItemInfo inf;
          
          if(field_name == "signal_id")                     inf.type = itSignalId;
          else if(field_name == "signal_name")              inf.type = itSignalName;
          else if(field_name == "signal_data_length")       inf.type = itSignalDataLenght;
          else if(field_name == "signal_data_offset")       inf.type = itSignalOffset;
          else if(field_name == "signal_description")       inf.type = itSignalDescription;
          else                                              inf.type = itUndefined;
          
          inf.fieldName = field_name;
          
          sensor->child(child_count)->setInfo(column, inf);
        
        }
      }
      
      q->finish();
      
    }
      
    delete q;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    log << svlog::Time << svlog::Critical << e.error << svlog::endl;
    return false;
    
  }

}
