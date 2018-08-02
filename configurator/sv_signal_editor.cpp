#include "sv_signal_editor.h"

SvSignalEditor* SIGNALEDITOR_UI;
extern SvDeviceEditor *DEVICE_UI; 
extern SvSQLITE *SQLITE;
extern SvRepositoryEditor* REPOSITORYEDITOR_UI;

SvSignalEditor::SvSignalEditor(QWidget *parent, int deviceId, int id) :
  QDialog(parent),
  ui(new Ui::SvSignalEditorDialog)
{
  ui->setupUi(this); 

  _showMode = id == -1 ? smNew : smEdit;
  
  _signal_params.device_id = deviceId;
  
  loadRepositories();
  loadDataTypes();
  
  QSqlQuery* q;
  QSqlError serr;
  
  /** читаем данные об устройстве **/
  q = new QSqlQuery(SQLITE->db);
  serr = SQLITE->execSQL(QString(SQL_SELECT_ONE_DEVICE).arg(_signal_params.device_id), q);
  
  if(QSqlError::NoError != serr.type()) {  

    delete q;
    
    _last_error = serr.text(); 
    QDialog::done(Error);
    return;
    
  }
  
  q->first();
  
  _signal_params.device_name = q->value("device_name").toString();
  _signal_params.kts_name = q->value("device_kts_name").toString();
//    _device_ifc_name = q->value("device_ifc_name").toString();
//    _device_protocol_name = q->value("device_protocol_name").toString();
//    _device_connection_params = q->value("connection_params").toString();
//    _device_data_type_name = q->value("device_data_type_name").toString();
//    _device_data_length = q->value("device_data_length").toString(); 
//    _device_driver_path = q->value("device_driver_lib_path").toString();
  
  q->finish();
  
  /** читаем данные о сигнале **/
  if(_showMode == smEdit) {
    
    QSqlQuery* q = new QSqlQuery(SQLITE->db);
    QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_ONE_SIGNAL).arg(id), q);
    
    if(QSqlError::NoError != serr.type()) {  

      delete q;
      
      _last_error = serr.text(); 
      QDialog::done(Error);
      return;
      
    }
    
    q->first();
    
    _signal_params.id = q->value("signal_id").toInt();
    _signal_params.name = q->value("signal_name").toString();
    _signal_params.timeout = q->value("signal_timeout").toInt();
    _signal_params.data_type = q->value("signal_data_type").toInt();
    _signal_params.data_offset = q->value("signal_data_offset").toUInt();
    _signal_params.data_length = q->value("signal_data_length").toUInt();
    _signal_params.description = q->value("signal_description").toString(); 
    _signal_params.major_repository_id = q->value("major_repository_id").toInt();
    _signal_params.minor_repository1_id = q->value("minor_repository1_id").toInt();
    _signal_params.minor_repository2_id = q->value("minor_repository2_id").toInt();
    _signal_params.minor_repository3_id = q->value("minor_repository3_id").toInt();
    
    q->finish();

    
  }
  delete q;
  
  setCurrentRepositories();
  
  ui->cbDataType->setCurrentIndex(ui->cbDataType->findData(_signal_params.data_type));
  
  if(_showMode == smNew) this->setWindowTitle("Новый сигнал");
  else this->setWindowTitle(QString("Сигнал: %1").arg(_signal_params.name));
  
  if(_showMode == smNew) ui->editID->setText("<Новый>");
  else  ui->editID->setText(QString::number(_signal_params.id));
  
  ui->editName->setText(_signal_params.name);
//  ui->cbDevicePortName->setCurrentIndex(ui->cbDevicePortName->findText(_device_port_name));
  ui->editDevice->setText(_signal_params.device_name);
  ui->editKTS->setText(_signal_params.kts_name);
  ui->spinDataOffset->setValue(_signal_params.data_offset);
  ui->spinDataLength->setValue(_signal_params.data_length);
  ui->textDescription->setText(_signal_params.description);
  
  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->bnCancel, &QPushButton::clicked, this, &QDialog::reject);
  
  connect(ui->bnNewRepository, &QPushButton::clicked, this, &SvSignalEditor::newRepoisitory);
//  connect(ui->bnConfig, SIGNAL(clicked()), this, SLOT(config()));
  
  setModal(true);
  show();
  
}

SvSignalEditor::~SvSignalEditor()
{
  close();
  delete ui;
}


bool SvSignalEditor::loadRepositories()
{
  ui->cbMajorRepository->clear();
  ui->cbMinorRepository1->clear();
  ui->cbMinorRepository2->clear();
  ui->cbMinorRepository3->clear();
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_REPOSITORIES_LIST), q); 
  
  if(QSqlError::NoError != serr.type()) {
    
    _last_error = serr.text();
    delete q;
    return false;
  }

  while(q->next()) {
    
    ui->cbMajorRepository->addItem(q->value("repository_name").toString(), q->value("repository_id").toInt());
    ui->cbMinorRepository1->addItem(q->value("repository_name").toString(), q->value("repository_id").toInt());
    ui->cbMinorRepository2->addItem(q->value("repository_name").toString(), q->value("repository_id").toInt());
    ui->cbMinorRepository3->addItem(q->value("repository_name").toString(), q->value("repository_id").toInt());
    
  }
  
  q->finish();
  delete q;
  
//  connect(ui->cbDeviceType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateKTSInfo(int)));
  
  return true;
  
}

bool SvSignalEditor::loadDataTypes()
{
  ui->cbDataType->clear();
  
  ui->cbDataType->addItem("Integer", SignalDataTypes::dtInt);
  ui->cbDataType->addItem("Float", SignalDataTypes::dtFloat);
}

void SvSignalEditor::setCurrentRepositories()
{
  ui->cbMajorRepository->setCurrentIndex(ui->cbMajorRepository->findData(_signal_params.major_repository_id));
  ui->cbMinorRepository1->setCurrentIndex(ui->cbMinorRepository1->findData(_signal_params.minor_repository1_id));
  ui->cbMinorRepository2->setCurrentIndex(ui->cbMinorRepository2->findData(_signal_params.minor_repository2_id));
  ui->cbMinorRepository3->setCurrentIndex(ui->cbMinorRepository3->findData(_signal_params.minor_repository3_id));
}

void SvSignalEditor::accept()
{
  try {
    
    /* делаем всякие проверки вводимых данных */
    if(ui->editName->text() == "") {
      
      QMessageBox::critical(0, "Ошибка", "Имя сигнала не указано");
      ui->editName->setFocus();
      return;
    }
  
//  /* конец проверок */
  
    _signal_params.name = ui->editName->text();
    _signal_params.timeout = ui->spinTimeout->value();
    _signal_params.data_type = ui->cbDataType->currentData().toUInt();
    _signal_params.data_offset = ui->spinDataOffset->value();
    _signal_params.data_length = ui->spinDataLength->value();
    _signal_params.major_repository_id = ui->cbMajorRepository->currentData().toInt();
    _signal_params.minor_repository1_id = ui->cbMinorRepository1->currentData().toInt();
    _signal_params.minor_repository2_id = ui->cbMinorRepository2->currentData().toInt();
    _signal_params.minor_repository3_id = ui->cbMinorRepository3->currentData().toInt();
    _signal_params.description = ui->textDescription->toPlainText();
  
    switch (_showMode) {
      
      case smNew: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_NEW_SIGNAL)
                                         .arg(_signal_params.device_id)
                                         .arg(_signal_params.name)
                                         .arg(_signal_params.timeout)
                                         .arg(_signal_params.data_type)
                                         .arg(_signal_params.data_offset)
                                         .arg(_signal_params.data_length)
                                         .arg(_signal_params.description)
                                         .arg(_signal_params.major_repository_id)
                                         .arg(_signal_params.minor_repository1_id)
                                         .arg(_signal_params.minor_repository2_id)
                                         .arg(_signal_params.minor_repository3_id));
        
        if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());
        
        break;
      }
        
      case smEdit: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_UPDATE_SIGNAL)
                                         .arg(_signal_params.device_id)
                                         .arg(_signal_params.name)
                                         .arg(_signal_params.timeout)
                                         .arg(_signal_params.data_type)
                                         .arg(_signal_params.data_offset)
                                         .arg(_signal_params.data_length)
                                         .arg(_signal_params.description)
                                         .arg(_signal_params.major_repository_id)
                                         .arg(_signal_params.minor_repository1_id)
                                         .arg(_signal_params.minor_repository2_id)
                                         .arg(_signal_params.minor_repository3_id)
                                         .arg(_signal_params.id));
        
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

void SvSignalEditor::newRepoisitory()
{
  REPOSITORYEDITOR_UI = new SvRepositoryEditor(this);
  
  int result = REPOSITORYEDITOR_UI->exec();
  
  switch (result) {
    
    case SvRepositoryEditor::Error:
      QMessageBox::critical(this, "Ошибка", REPOSITORYEDITOR_UI->lastError(), QMessageBox::Ok);
      break;
      
    case SvRepositoryEditor::Accepted:
      loadRepositories();
      setCurrentRepositories();
      break;
    
  }
  
  delete REPOSITORYEDITOR_UI;
  
}
