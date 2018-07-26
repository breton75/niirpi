#include "ui_sv_device_editor.h"
#include "sv_device_editor.h"

SvDeviceEditor *DEVICE_UI;

extern SvDeviceTypeEditor *DEVICE_TYPE_UI;  
extern SvSQLITE *SQLITE;


SvDeviceEditor::SvDeviceEditor(QWidget *parent, int deviceId) :
  QDialog(parent),
  ui(new Ui::SvDeviceDialog)
{
  ui->setupUi(this); 

  showMode = deviceId == -1 ? smNew : smEdit;
  
  if(!loadDeviceTypes())
    QMessageBox::critical(this, "Error", _last_error, QMessageBox::Ok);
  
  if(showMode == smEdit) {
    
    QSqlQuery* q = new QSqlQuery(SQLITE->db);
    QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_ONE_DEVICE).arg(deviceId), q);
    
    if(QSqlError::NoError != serr.type()) {  

      delete q;
      
      _last_error = serr.text();
      QDialog::done(Error);
      return;
      
    }
    
    q->first();
    
    _id = q->value("device_id").toInt();
    _device_name = q->value("device_name").toString();
    _device_type_id = q->value("device_type_id").toInt();
    _device_ifc_type = q->value("device_ifc_type_name").toString();
    _device_ifc_protocol = q->value("device_ifc_protocol_name").toString();
    _device_ifc_port = q->value("device_ifc_port_name").toString();
    _device_data_type = q->value("device_data_type").toString();
    _device_data_length = q->value("device_data_length").toString(); 
    _device_driver_path = q->value("device_driver_lib_path").toString();
    _device_description = q->value("device_description").toString(); 
    
    q->finish();
    delete q;
    
    ui->cbDeviceType->setCurrentIndex(ui->cbDeviceType->findData(_device_type_id));
    
  }
   
  
  if(showMode == smNew) this->setWindowTitle("Новое устройство");
  else this->setWindowTitle(QString("Устройство - %1").arg(_device_name));
  
  if(showMode == smNew) ui->editID->setText("<Новый>");
  else  ui->editID->setText(QString::number(_id));
  
  ui->editDeviceName->setText(_device_name);
  ui->editDeviceIfc->setText(_device_ifc_type);
  ui->editDeviceProtocol->setText(_device_ifc_protocol);
  ui->editDevicePort->setText(_device_ifc_port);
  ui->editDeviceDataType->setText(_device_data_type);
  ui->editDeviceDataLength->setText(_device_data_length);
  ui->editDeviceDriverPath->setText(_device_driver_path);
  ui->textDeviceDescription->setText(_device_description);
  
  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->bnCancel, &QPushButton::clicked, this, &QDialog::reject);
  connect(ui->bnNewDeviceType, &QPushButton::clicked, this, &SvDeviceEditor::newDeviceType);
  connect(ui->bnEditDeviceType, &QPushButton::clicked, this, &SvDeviceEditor::editDeviceType);
//  connect(ui->bnConfig, SIGNAL(clicked()), this, SLOT(config()));
  
  this->setModal(true);
  this->show();
}

SvDeviceEditor::~SvDeviceEditor()
{
  this->close();
  delete ui;
}

bool SvDeviceEditor::loadDeviceTypes()
{
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_DEVICE_TYPES_LIST), q);
  if(QSqlError::NoError != serr.type()) {
    
    _last_error = serr.text();
    delete q;
    return false;
  }

  while(q->next())
    ui->cbDeviceType->addItem(q->value("device_type_designation").toString(), q->value("device_type_id").toUInt());
  
  q->finish();
  delete q;
  
  if(ui->cbDeviceType->count()) ui->cbDeviceType->setCurrentIndex(0);
  ui->bnSave->setEnabled(!ui->cbDeviceType->currentData().isNull());
  
  connect(ui->cbDeviceType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDeviceTypeInfo(int)));
  
  return true;
  
}

void SvDeviceEditor::accept()
{
  try {
    
    /* делаем всякие проверки вводимых данных */
    if(ui->editDeviceName->text() == "") {
      
      QMessageBox::critical(0, "Ошибка", "Имя устройства не указано");
      ui->editDeviceName->setFocus();
      return;
    }
  
//  if(t_model_id == -1)
//  {
//    log_ns::log(m_Critical, "Необходимо указать модель устройства");
//    ui->bnSelectModel->setFocus();
//    return;
//  }
  
//  /* конец проверок */
  
    _device_name = ui->editDeviceName->text();
    _device_type_id = ui->cbDeviceType->currentData().toInt();
    _device_description = ui->textDeviceDescription->toPlainText();
  
    switch (this->showMode) {
      
      case smNew: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_NEW_DEVICE)
                                         .arg(_device_name)
                                         .arg(_device_type_id)
                                         .arg(_device_description));
        
        if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());
        
        break;
      }
        
      case smEdit: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_UPDATE_DEVICE)
                                         .arg(_device_name)
                                         .arg(_device_type_id)
                                         .arg(_device_description)
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

void SvDeviceEditor::newDeviceType()
{
  DEVICE_TYPE_UI = new SvDeviceTypeEditor(this);
  
  int result = DEVICE_TYPE_UI->exec();
  
  switch (result) {
    
    case SvDeviceTypeEditor::Error:
      QMessageBox::critical(this, "Ошибка", DEVICE_TYPE_UI->lastError(), QMessageBox::Ok);
      break;
      
    case SvDeviceTypeEditor::Accepted:
      int indx = ui->cbDeviceType->currentIndex();
      
      disconnect(ui->cbDeviceType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDeviceTypeInfo(int)));
      
      ui->cbDeviceType->clear();
      loadDeviceTypes();
      
      ui->cbDeviceType->setCurrentIndex(indx);
      
      break;
    
  }
  delete DEVICE_TYPE_UI;
}


void SvDeviceEditor::editDeviceType()
{
  DEVICE_TYPE_UI = new SvDeviceTypeEditor(this, ui->cbDeviceType->currentData().toInt());
  
  int result = DEVICE_TYPE_UI->exec();
  
  switch (result) {
    
    case SvDeviceEditor::Error:
      QMessageBox::critical(this, "Ошибка", DEVICE_TYPE_UI->lastError(), QMessageBox::Ok);
      break;
      
    case SvDeviceEditor::Accepted:
      int indx = ui->cbDeviceType->currentIndex();
      
      disconnect(ui->cbDeviceType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDeviceTypeInfo(int)));
      
      ui->cbDeviceType->clear();
      loadDeviceTypes();
      
      ui->cbDeviceType->setCurrentIndex(indx);
      break;
    
  }
  delete DEVICE_TYPE_UI;
}

void SvDeviceEditor::updateDeviceTypeInfo(int index)
{
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  if(QSqlError::NoError != SQLITE->execSQL(QString(SQL_SELECT_ONE_DEVICE_TYPE).arg(ui->cbDeviceType->itemData(index).toUInt()), q).type()) {
    
    delete q;
    return;
  }
  
  if(q->next()) {
    
    ui->editDeviceIfc->setText(q->value("device_type_ifc_type_name").toString());
    ui->editDeviceProtocol->setText(q->value("device_type_ifc_protocol_name").toString());
    ui->editDevicePort->setText(q->value("device_type_ifc_port_name").toString());
    ui->editDeviceDataType->setText(q->value("device_data_type").toString());
    ui->editDeviceDataLength->setText(q->value("device_type_data_length").toString());
    ui->editDeviceDriverPath->setText(q->value("device_type_driver_lib_path").toString());
    
  }
  
  q->finish();
  delete q;
  
}
