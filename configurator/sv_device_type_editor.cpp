#include "sv_device_type_editor.h"

SvDeviceTypeEditor *DEVICE_TYPE_UI;
extern SvSQLITE *SQLITE;


SvDeviceTypeEditor::SvDeviceTypeEditor(QWidget *parent, int typeId) :
  QDialog(parent),
  ui(new Ui::SvDeviceTypeDialog)
{
  ui->setupUi(this); 

  showMode = deviceId == -1 ? smNew : smEdit;
  
  loadIfcTypes();
  loadIfcProtocols();
  loadDataTypes();  
  
  if(showMode == smEdit) {
    
    QSqlQuery* q = new QSqlQuery(SQLITE->db);
    QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_ONE_DEVICE_TYPE).arg(typeId), q);
    
    if(QSqlError::NoError != serr.type()) {  

      delete q;
      
      _last_error = serr.text();
      QDialog::done(Error);
      return;
      
    }
    
    q->first();
    
    _device_type_id = q->value("device_type_id").toInt(); 
    _device_type_name = q->value("device_name").toString();
    _device_type_ifc_type_id = q->value("device_type_ifc_type_id").toInt();
    _device_type_ifc_type_name = q->value("device_type_ifc_type_name").toString();
    _device_type_ifc_protocol_id = q->value("device_type_ifc_protocol_id").toInt();
    _device_type_ifc_protocol_name = q->value("device_type_ifc_protocol_name").toString();
    _device_type_ifc_port_name = q->value("device_type_ifc_port_name").toString();
    _device_type_data_type = q->value("device_type_data_type").toInt();
    _device_type_data_length = q->value("device_type_data_length").toInt();
    _device_type_driver_path = q->value("device_type_driver_lib_path").toString();
    _device_type_description = q->value("device_type_description").toString(); 
    
    delete q;
    
    ui->cbIfc->setCurrentIndex(_device_type_ifc_type_id);
    ui->cbProtocol->setCurrentIndex(_device_type_ifc_protocol_id);
    
  }
  
   
  
  if(showMode == smNew) this->setWindowTitle("Новое устройство");
  else this->setWindowTitle(QString("Устройство - %1").arg(_device_name));
  
  if(showMode == smNew) ui->editID->setText("<Новый>");
  else  ui->editID->setText(QString::number(_id));
  
  ui->editSensorName->setText(_device_type_name);
  ui->editDriver->setText(_device_type_driver_path);
//  ui->cbPort->set;
  ui->editDriver->setText(_driver_path);
  ui->textDescription->setText(_device_type_description);
  
  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->bnCancel, &QPushButton::clicked, this, &QDialog::reject);
  connect(ui->bnSelectDriver, &QPushButton::clicked, this, &SvDeviceTypeEditor::selectDriver);
//  connect(ui->bnConfig, SIGNAL(clicked()), this, SLOT(config()));
  
  this->setModal(true);
  this->show();
}

SvDeviceTypeEditor::~SvDeviceTypeEditor()
{
  this->close();
  delete ui;
}


void SvDeviceTypeEditor::loadIfcTypes()
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

void SvDeviceTypeEditor::loadIfcProtocols()
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

void SvDeviceTypeEditor::loadDataTypes()
{
  ui->cbDataType->addItem("Аналог", 0);
  ui->cbDataType->addItem("Цифровой", 1);
}

void SvDeviceTypeEditor::accept()
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
  
    _device_name = ui->editSensorName->text();
    _ifc_type = ui->cbIfc->currentData().toInt();
    _ifc_protocol = ui->cbProtocol->currentData().toInt();
    _ifc_port = ui->cbPort->currentText();
    _data_type = ui->cbDataType->currentData().toInt();
    _data_length = ui->spinDataLength->value();
    _driver_path = ui->editDriver->text();
    _description = ui->textDescription->toPlainText();
  
    switch (this->showMode) {
      
      case smNew: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_NEW_DEVICE_TYPE)
                                         .arg(_device_name)
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
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_UPDATE_DEVICE_TYPE)
                                         .arg(_device_name)
                                         .arg(_ifc_type)
                                         .arg(_ifc_port)
                                         .arg(_ifc_protocol)
                                         .arg(_data_type)
                                         .arg(_data_length)
                                         .arg(_description)
                                         .arg(_id));
        
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

void SvDeviceTypeEditor::selectDriver()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open driver library"),
                                       QDir::currentPath(), tr("library files (*.dll)"));
  
  if(!fileName.isEmpty())
    ui->editDriver->setText(fileName);
  
}

