#include "ui_sv_device_editor.h"
#include "sv_device_editor.h"

SvDeviceEditor *DEVICE_UI;

extern SvKTSEditor *KTS_UI;  
extern SvSQLITE *SQLITE;


SvDeviceEditor::SvDeviceEditor(QWidget *parent, int deviceId) :
  QDialog(parent),
  ui(new Ui::SvDeviceDialog)
{
  ui->setupUi(this); 

  showMode = deviceId == -1 ? smNew : smEdit;
  
  if(!loadKTSs())
    QMessageBox::critical(this, "Error", _last_error, QMessageBox::Ok);
  
  loadPorts();
  
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
    _device_kts_id = q->value("device_kts_id").toInt();
    _device_ifc_name = q->value("device_ifc_name").toString();
    _device_protocol_name = q->value("device_protocol_name").toString();
    _device_port_name = q->value("device_port_name").toString();
    _device_data_type_name = q->value("device_data_type_name").toString();
    _device_data_length = q->value("device_data_length").toString(); 
    _device_driver_path = q->value("device_driver_lib_path").toString();
    _device_description = q->value("device_description").toString(); 
    
    q->finish();
    delete q;
    
    ui->cbDeviceType->setCurrentIndex(ui->cbDeviceType->findData(_device_kts_id));
    
  }
   
  
  if(showMode == smNew) this->setWindowTitle("Новое устройство");
  else this->setWindowTitle(QString("Устройство: %1").arg(_device_name));
  
  if(showMode == smNew) ui->editID->setText("<Новый>");
  else  ui->editID->setText(QString::number(_id));
  
  ui->editDeviceName->setText(_device_name);
  ui->cbDevicePortName->setCurrentIndex(ui->cbDevicePortName->findText(_device_port_name));
  ui->editDeviceIfc->setText(_device_ifc_name);
  ui->editDeviceProtocol->setText(_device_protocol_name);
  ui->editDeviceDataType->setText(_device_data_type_name);
  ui->editDeviceDataLength->setText(_device_data_length);
  ui->editDeviceDriverPath->setText(_device_driver_path);
  ui->textDeviceDescription->setText(_device_description);
  
  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->bnCancel, &QPushButton::clicked, this, &QDialog::reject);
  connect(ui->bnNewDeviceType, &QPushButton::clicked, this, &SvDeviceEditor::newKTS);
  connect(ui->bnEditDeviceType, &QPushButton::clicked, this, &SvDeviceEditor::editKTS);
//  connect(ui->bnConfig, SIGNAL(clicked()), this, SLOT(config()));
  
  this->setModal(true);
  this->show();
}

SvDeviceEditor::~SvDeviceEditor()
{
  this->close();
  delete ui;
}

bool SvDeviceEditor::loadKTSs()
{
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_KTSS_LIST), q); 
  if(QSqlError::NoError != serr.type()) {
    
    _last_error = serr.text();
    delete q;
    return false;
  }

  while(q->next())
    ui->cbDeviceType->addItem(q->value("kts_name").toString(), q->value("kts_id").toUInt());
  
  q->finish();
  delete q;
  
  if(ui->cbDeviceType->count()) ui->cbDeviceType->setCurrentIndex(0);
  ui->bnSave->setEnabled(!ui->cbDeviceType->currentData().isNull());
  
  connect(ui->cbDeviceType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateKTSInfo(int)));
  
  return true;
  
}

void SvDeviceEditor::loadPorts()
{
  for(int i = 0; i < QSerialPortInfo::availablePorts().count(); i++)
    ui->cbDevicePortName->addItem(QSerialPortInfo::availablePorts().at(i).portName());
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
  
  if(ui->cbDevicePortName->currentText().isEmpty() | (ui->cbDevicePortName->currentIndex() == -1)) {
    
    QMessageBox::critical(0, "Ошибка", "Необходимо указать порт");
    ui->cbDevicePortName->setFocus();
    return;
  }
  
//  /* конец проверок */
  
    _device_name = ui->editDeviceName->text();
    _device_kts_id = ui->cbDeviceType->currentData().toInt();
    _device_port_name = ui->cbDevicePortName->currentText();
    _device_description = ui->textDeviceDescription->toPlainText();
  
    switch (this->showMode) {
      
      case smNew: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_NEW_DEVICE)
                                         .arg(_device_name)
                                         .arg(_device_kts_id)
                                         .arg(_device_port_name)
                                         .arg(_device_description));
        
        if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());
        
        break;
      }
        
      case smEdit: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_UPDATE_DEVICE)
                                         .arg(_device_name)
                                         .arg(_device_kts_id)
                                         .arg(_device_port_name)
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

void SvDeviceEditor::newKTS()
{
  KTS_UI = new SvKTSEditor(this);
  
  int result = KTS_UI->exec();
  
  switch (result) {
    
    case SvKTSEditor::Error:
      QMessageBox::critical(this, "Ошибка", KTS_UI->lastError(), QMessageBox::Ok);
      break;
      
    case SvKTSEditor::Accepted:
      int indx = ui->cbDeviceType->currentIndex();
      
      disconnect(ui->cbDeviceType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateKTSInfo(int)));
      
      ui->cbDeviceType->clear();
      loadKTSs();
      
      ui->cbDeviceType->setCurrentIndex(indx);
      
      break;
    
  }
  delete KTS_UI;
}


void SvDeviceEditor::editKTS()
{
  KTS_UI = new SvKTSEditor(this, ui->cbDeviceType->currentData().toInt());
  
  int result = KTS_UI->exec();
  
  switch (result) {
    
    case SvDeviceEditor::Error:
      QMessageBox::critical(this, "Ошибка", KTS_UI->lastError(), QMessageBox::Ok);
      break;
      
    case SvDeviceEditor::Accepted:
      int indx = ui->cbDeviceType->currentIndex();
      
      disconnect(ui->cbDeviceType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateKTSInfo(int)));
      
      ui->cbDeviceType->clear();
      loadKTSs();
      
      ui->cbDeviceType->setCurrentIndex(indx);
      break;
    
  }
  delete KTS_UI;
}

void SvDeviceEditor::updateKTSInfo(int index)
{
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  if(QSqlError::NoError != SQLITE->execSQL(QString(SQL_SELECT_ONE_KTS).arg(ui->cbDeviceType->itemData(index).toUInt()), q).type()) {
    
    delete q;
    return;
  }
  
  if(q->next()) {
    
    ui->editDeviceIfc->setText(q->value("kts_ifc_name").toString());
    ui->editDeviceProtocol->setText(q->value("kts_protocol_name").toString());
    ui->editDeviceDataType->setText(q->value("kts_data_type_name").toString());
    ui->editDeviceDataLength->setText(q->value("kts_data_length").toString());
    ui->editDeviceDriverPath->setText(q->value("kts_driver_lib_path").toString());
    
  }
  
  q->finish();
  delete q;
  
}
