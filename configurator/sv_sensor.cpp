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
  ui->setupUi(this); 

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
      return;
      
    }
    
    q->first();
    
    _id = q->value("sensor_id").toInt();
    _sensor_name = q->value("sensor_name").toString();
    _ifc_type = q->value("sensor_ifc_type").toInt();
    _ifc_port = q->value("sensor_ifc_port_name").toString();
    _ifc_protocol = q->value("sensor_ifc_protocol").toInt();
    _data_type = q->value("sensor_data_type").toInt();
    _data_length = q->value("sensor_data_length").toInt();
    _driver_path = q->value("driver_lib_path").toString();
    _description = q->value("sensor_description").toString(); 
    
    delete q;
    
    ui->cbIfc->setCurrentIndex(_ifc_type);
    ui->cbProtocol->setCurrentIndex(_ifc_protocol);
    
  }
  
   
  
  if(showMode == smNew) this->setWindowTitle("Новое устройство");
  else this->setWindowTitle(QString("Устройство - %1").arg(_sensor_name));
  
  if(showMode == smNew) ui->editID->setText("<Новый>");
  else  ui->editID->setText(QString::number(_id));
  
  ui->editSensorName->setText(_sensor_name);
  ui->editDriver->setText(_driver_path);
//  ui->cbPort->set;
  ui->editDriver->setText(_driver_path);
  ui->textDescription->setText(_description);
  
  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->bnCancel, &QPushButton::clicked, this, &QDialog::reject);
  connect(ui->bnSelectDriver, &QPushButton::clicked, this, &SvSensor::selectDriver);
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
  QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_IFC_TYPES), q);
  
  if(QSqlError::NoError != serr.type()) {
    
    delete q;
    return;
  }
  
  while(q->next())
    ui->cbIfc->addItem(q->value("designation").toString(), q->value("id"));

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
    ui->cbProtocol->addItem(q->value("designation").toString(), q->value("id").toUInt());
  
  delete q;
  
  if(ui->cbProtocol->count()) ui->cbProtocol->setCurrentIndex(0);
  ui->bnSave->setEnabled(!ui->cbProtocol->currentData().isNull());
  
}

void SvSensor::loadDataTypes()
{
  ui->cbDataType->addItem("Аналог", 0);
  ui->cbDataType->addItem("Цифровой", 1);
}

void SvSensor::accept()
{
  try {
    
    /* делаем всякие проверки вводимых данных */
    if(ui->editSensorName->text() == "") {
      
      QMessageBox::critical(0, "Ошибка", "Имя устройства не указано");
      ui->editSensorName->setFocus();
      return;
    }
  
//  if(t_model_id == -1)
//  {
//    log_ns::log(m_Critical, "Необходимо указать модель устройства");
//    ui->bnSelectModel->setFocus();
//    return;
//  }
  
//  /* конец проверок */
  
    _sensor_name = ui->editSensorName->text();
    _ifc_type = ui->cbIfc->currentData().toInt();
    _ifc_protocol = ui->cbProtocol->currentData().toInt();
    _ifc_port = ui->cbPort->currentText();
    _data_type = ui->cbDataType->currentData().toInt();
    _data_length = ui->spinDataLength->value();
    _driver_path = ui->editDriver->text();
    _description = ui->textDescription->toPlainText();
  
    switch (this->showMode) {
      
      case smNew: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_NEW_SENSOR)
                                         .arg(_sensor_name)
                                         .arg(_ifc_type)
                                         .arg(_ifc_port)
                                         .arg(_ifc_protocol)
                                         .arg(_data_type)
                                         .arg(_data_length)
                                         .arg(_description));
        
        if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());
        
        break;
      }
        
      case smEdit: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_NEW_SENSOR)
                                         .arg(_sensor_name)
                                         .arg(_ifc_type)
                                         .arg(_ifc_port)
                                         .arg(_ifc_protocol)
                                         .arg(_data_type)
                                         .arg(_data_length)
                                         .arg(_description));
        
        if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());
  
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

void SvSensor::selectDriver()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open driver library"),
                                       QDir::currentPath(), tr("library files (*.dll)"));
  
  if(!fileName.isEmpty())
    ui->editDriver->setText(fileName);
  
}

