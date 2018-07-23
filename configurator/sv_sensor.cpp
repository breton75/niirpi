#include "sv_sensor.h"

SvSensor *SENSOR_UI;
//extern SvDevModel* DEVMODEL_UI;
extern SvSQLITE *SQLITE;
//extern SvSelectModelDialog* SELECTMODELDIALOG_UI;
//extern SvDeviceConfiguration* DEVICE_CONFIG_UI;


//extern SvZone* ZONE_UI;
//extern SvNewZone* NEW_ZONE_UI;
//extern SvZonesList *ZONELIST_UI;

SvSensor::SvSensor(QWidget *parent, int deviceId) :
  QDialog(parent),
  ui(new Ui::SvSensorDialog)
{
  
  showMode = deviceId == -1 ? smNew : smEdit;
  
  loadIfcTypes();
  loadIfcProtocols();
  loadDataTypes();  
  
  if(showMode == smEdit) {
    
    QSqlQuery* q = new QSqlQuery(SQLITE->db);
    QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_ONE_SENSOR).arg(deviceId), q);
                     
    if(QSqlError::NoError != serr.type()) {  
      
      delete q;
      
      _last_error = serr.text();
      QDialog::done(Error);
      
    }
    
    q->first();
    
    _id = q->value("id").toInt();
    _sensor_name = q->value("designation").toString();
    _ifc_type = q->value("ifc_type").toInt();
    _ifc_port = q->value("ifc_port").toString();
    _ifc_protocol = q->value("ifc_protocol").toInt();
    _data_type = q->value("data_type").toInt();
    _data_length = q->value("data_length").toInt();
    _driver_path = q->value("model_lib_path").toString();
    _description = q->value("description").toString(); 
    
    delete q;
    
    ui->cbIfc->setCurrentIndex(_ifc_type);
    ui->cbProtocol->setCurrentIndex(_ifc_protocol);
    
  }
  
  ui->setupUi(this);  
  
  if(showMode == smNew) this->setWindowTitle("Новое устройство");
  else this->setWindowTitle(QString("Устройство - %1").arg(t_device_name));
  
  if(showMode == smNew) ui->editID->setText("<Новый>");
  else  ui->editID->setText(QString::number(_id));
  
  ui->editSensorName->setText(_sensor_name);
  ui->editDriver->setText(_driver_path);
//  ui->cbPort->set;
  ui->editDriver->setText(_driver_path);
  ui->textDescription->setText(_description);
  
  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->bnCancel, &QPushButton::clicked, this, &QDialog::reject);
  connect(ui->bnSelectDriver, &QPushButton::clicked, this, );
//  connect(ui->bnConfig, SIGNAL(clicked()), this, SLOT(config()));
  
  this->setModal(true);
  this->show();
}

SvSensor::~SvSensor()
{
  this->close();
  delete ui;
}


void SvSensor::loadIfcTypes()
{
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  if(QSqlError::NoError != SQLITE->execSQL(QString(SQL_SELECT_IFC_TYPES), q).type()) {
    
    delete q;
    return;
  }
  
  while(q->next())
    ui->cbIfc->addItem(q->value("designation").toString(),
                              q->value("id").toUInt());

  q->finish();
  delete q;
  
  if(ui->cbIfc->count()) ui->cbIfc->setCurrentIndex(0);
  
  ui->bnSave->setEnabled(!ui->cbIfc->currentData().isNull());
  
}

void SvSensor::loadIfcProtocols()
{
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  if(QSqlError::NoError != SQLITE->execSQL(QString(SQL_SELECT_IFC_PROTOCOLS), q).type()) {
    
    delete q;
    return;
  }
  
  while(q->next())
    ui->cbProtocol->addItem(q->value("designation").toString(),
                              q->value("id").toUInt());

  q->finish();
  delete q;
  
  if(ui->cbProtocol->count()) ui->cbProtocol->setCurrentIndex(0);
  
  ui->bnSave->setEnabled(!ui->cbProtocol->currentData().isNull());
  
}

void SvSensor::accept()
{
  try {
    
    /* делаем всякие проверки вводимых данных */
    if(ui->editDevName->text() == "") {
      
      QMessageBox.critical(0, "Ошибка", "Имя устройства не указано");
      ui->editDevName->setFocus();
      return;
    }
  
//  if(t_model_id == -1)
//  {
//    log_ns::log(m_Critical, "Необходимо указать модель устройства");
//    ui->bnSelectModel->setFocus();
//    return;
//  }
  
//  /* конец проверок */
  
  _device_name = ui->editDevName->text();
  _ip = QHostAddress(ui->editIP->text()).toIPv4Address(); //  ui->editIP->text();
  _port = ui->editPort->text().toInt();
//  t_model_name = ui->editModelName->text();
//  t_class_name = ui->editClass->text();
//  t_brand_name = ui->editBrand->text();
  _driver_path = ui->editDriver->text();
  _description = ui->textDescription->toPlainText();

    bool result = false;
    switch (this->showMode) {
      
      case smNew: {
        
        if(QSqlError::NoError != SQLITE->execSQL(QString(SQL_NEW_SENSOR)
                                               .arg(_sensor_name)
                                               .arg(_model_id)
                                               .arg(_class_id)
                                               .arg(_brand_id)
                                               .arg(_ip)
                                               .arg(_port)
                                               .arg(_description), q).type()) {
          q->finish();
          delete q;
          QMessageBox::critical(this, "Устройство", "Не удалось добавить новое устройство", QMessageBox::Ok);
          break;
        }
        q->first();
        
        t_id = q->value("new_device_id").toInt();
        
        q->finish();
        delete q;
        
        result = true;
        break;
      }
        
      case smEdit:
      {
        if(QSqlError::NoError != PGDB->execSQL(QString(SQL_UPDATE_DEVICE)
                                               .arg(_sensor_name)
                                               .arg(_model_id)
                                               .arg(_class_id)
                                               .arg(_brand_id)
                                               .arg(_ip)
                                               .arg(_port)
                                               .arg(_description)
                                               .arg(_id)).type())
        {
          QMessageBox::critical(this, "Устройство", "Не удалось обновить данные", QMessageBox::Ok);
          break;
        }
        
        result = true;
        break;
      }
    }

    QDialog::done(Accepted);
  
  }
  
  catch(SvException& e) {
    
    _last_error = e.error;
    QDialog::done(Error);
  }
  
}

void SvSensor::config()
{
//  DEVICE_CONFIG_UI = new SvDeviceConfiguration(t_id);
////  DEVICE_CONFIG_UI->setCurrentIP(ui->editIP->text() == "" ? "192.168.3.31" : ui->editIP->text());
////  DEVICE_CONFIG_UI->setCurrentPort(ui->editPort->text().toInt() == 0 ? 35580 : ui->editPort->text().toInt());
//  DEVICE_CONFIG_UI->setCurrentData(ui->editIP->text(), ui->editPort->text().toInt());
//  if(DEVICE_CONFIG_UI->exec() == Qui::Accepted)
//  {
//    ui->editIP->setText(DEVICE_CONFIG_UI->t_device_new_ip);
//    ui->editPort->setText(QString::number(DEVICE_CONFIG_UI->t_device_new_port));
//  }
//  DEVICE_CONFIG_UI->~SvDeviceConfiguration();
}

void SvSensor::selectDriver()
{
//  SELECTMODELDIALOG_UI = new SvSelectModelDialog(this);
//  if(SELECTMODELDIALOG_UI->exec() == QDialog::Accepted)
//  {
//    t_model_id = SELECTMODELDIALOG_UI->t_id;
//    t_class_id = SELECTMODELDIALOG_UI->t_class_id; 
//    t_brand_id = SELECTMODELDIALOG_UI->t_brand_id; 
//    t_model_name = SELECTMODELDIALOG_UI->t_model_name;
//    t_class_name = SELECTMODELDIALOG_UI->t_class_name;
//    t_brand_name = SELECTMODELDIALOG_UI->t_brand_name;
//    t_driver_path = SELECTMODELDIALOG_UI->t_driver_path;
    
//    ui->editModelName->setText(t_model_name);
//    ui->editClass->setText(t_class_name);
//    ui->editBrand->setText(t_brand_name);
//    ui->editDriver->setText(t_driver_path);
//  }
//  SELECTMODELDIALOG_UI->~SvSelectModelDialog();
  
//  ui->bnConfig->setEnabled(t_model_id != -1);
}

