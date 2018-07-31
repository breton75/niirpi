#include "sv_repository_editor.h"
#include "ui_sv_repository_editor.h"

SvRepositoryEditor* REPOSITORYEDITOR_UI;
extern SvSQLITE *SQLITE;

SvRepositoryEditor::SvRepositoryEditor(QWidget *parent, int id) :
  QDialog(parent),
  ui(new Ui::SvRepositoryEditorDialog)
{
  ui->setupUi(this);
  
  _showMode = id == -1 ? smNew : smEdit; 
  
  if(_showMode == smEdit) {
    
    QSqlQuery* q = new QSqlQuery(SQLITE->db);
    QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_ONE_REPOSITORY).arg(id), q);
    
    if(QSqlError::NoError != serr.type()) {  

      delete q;
      
      _last_error = serr.text();
      QDialog::done(Error);
      return;
      
    }
    
    q->first();
    
    _params.id = q->value("repository_id").toInt(); 
    _params.repository_name = q->value("repository_name").toString();
    _params.host = q->value("repository_host").toString();
    _params.port = q->value("repository_port").toInt();
    _params.login = q->value("repository_login").toString();
    _params.pass = q->value("repository_pass").toString();
    _params.db_name = q->value("repository_dbname").toString();
    _params.table_name = q->value("repository_table_name").toString();
    
    delete q;
    
  }
  
  ui->editName->setText(_params.repository_name);
  ui->editIP->setText(_params.host);
  ui->spinPort->setValue(_params.port);
  ui->editLogin->setText(_params.login);
  ui->editPass->setText(_params.pass);
  ui->editDBName->setText(_params.db_name);
  ui->editTableName->setText(_params.table_name);

  
  connect(ui->bnSave, SIGNAL(clicked()), this, SLOT(accept()));
  connect(ui->bnCancel, SIGNAL(clicked()), this, SLOT(reject()));
  
  this->setModal(true);
  this->show();
}

SvRepositoryEditor::~SvRepositoryEditor()
{
  delete ui;
}

void SvRepositoryEditor::accept()
{
  bool ok;
  QHostAddress(ui->editIP->text().trimmed()).toIPv4Address(&ok);
  if(!ok) {
    
    QMessageBox::critical(this, "Ошибка", "Неверный адрес сервера", QMessageBox::Ok);
    ui->editIP->setFocus();
    return;    
  }
  
  if(ui->editName->text().isEmpty()) {
    
    QMessageBox::critical(this, "Ошибка", "Не указано имя репозитория", QMessageBox::Ok);
    ui->editName->setFocus();
    return;    
  }
  
  
  _params.repository_name = ui->editName->text();
  _params.host = ui->editIP->text();
  _params.port = ui->spinPort->value();
  _params.login = ui->editLogin->text();
  _params.pass = ui->editPass->text();
  _params.db_name = ui->editDBName->text();
  _params.table_name = ui->editTableName->text();
  
  try {
    
    switch (_showMode) {
      
      case smNew: {
    
        QSqlError serr = SQLITE->execSQL(QString(SQL_NEW_REPOSITORY)
                              .arg(_params.repository_name)
                              .arg(_params.host)
                              .arg(_params.port)
                              .arg(_params.login)
                              .arg(_params.pass)
                              .arg(_params.db_name)
                              .arg(_params.table_name));
        
        if(QSqlError::NoError != serr.type()) _exception.raise(serr.databaseText());
        
        break;
        
      }
        
      case smEdit: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_UPDATE_REPOSITORY)
                              .arg(_params.repository_name)
                              .arg(_params.host)
                              .arg(_params.port)
                              .arg(_params.login)
                              .arg(_params.pass)
                              .arg(_params.db_name)
                              .arg(_params.table_name)
                              .arg(_params.id));
        
        if(QSqlError::NoError != serr.type()) _exception.raise(serr.databaseText());
        
        break;  
      
      }
    }
    
    QDialog::done(Accepted);
    
  }
  
  catch(SvException& e) {
    
    _last_error = e.error;
//        qDebug() << _last_error;
    QDialog::done(Error);
  }
  
}
