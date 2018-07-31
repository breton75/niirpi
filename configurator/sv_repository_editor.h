#ifndef SV_NETWORKEDITOR_H
#define SV_NETWORKEDITOR_H

#include <QDialog>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QMessageBox>

#include "../../global/sql_defs.h"
#include "../../global/sv_idevice.h"
#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_exception.h"


namespace Ui {
class SvRepositoryEditorDialog;
}

struct RepositoryParams {
  
  int id = -1;
  bool is_main = true;
  QString repository_name = "";
  QString host = "";
  quint32 port = 5432;
  QString login = "postgres";
  QString pass = "postgres";
  QString db_name = "can_db";
  QString table_name = "can_db_option";
  
};

class SvRepositoryEditor : public QDialog
{
  Q_OBJECT
  
public:
  enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
  enum ShowMode { smNew = 0, smEdit = 1 };
  
  explicit SvRepositoryEditor(QWidget *parent, int id = -1);
  ~SvRepositoryEditor();
  
  
  QString lastError() { return _last_error; }
    
private:
  Ui::SvRepositoryEditorDialog *ui;
  
  int _showMode;
  RepositoryParams _params;
  
  SvException _exception;
  QString _last_error = "";
  
  void accept();
  
};

#endif // SV_NETWORKEDITOR_H
