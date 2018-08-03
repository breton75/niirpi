#ifndef SV_SIGNAL_EDITOR_H
#define SV_SIGNAL_EDITOR_H

#include <QDialog>

#include "ui_sv_signal_editor.h"

#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_exception.h"
#include "../global/sql_defs.h"
#include "../global/sv_signal.h"

#include "sv_device_editor.h"
#include "sv_repository_editor.h"

namespace Ui {
class SvSignalEditorDialog;
}

class SvSignalEditor : public QDialog
{
  Q_OBJECT
  
public:
  enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
  enum ShowMode { smNew = 0, smEdit = 1 };
  
  explicit SvSignalEditor(QWidget *parent, int deviceId, int id = -1);
  ~SvSignalEditor();
  
  QString lastError() { return _last_error; }
  
  
public slots:
  void accept() Q_DECL_OVERRIDE;
  
private:
  Ui::SvSignalEditorDialog *ui;
  
  int _showMode;
  
  SvException _exception;
  
  SignalParams _signal_params;
  
  QString _last_error = "";
  
  void setCurrentRepositories();
  
private slots:
  void newRepoisitory();
  bool loadDataTypes();
  bool loadRepositories();
  
};

#endif // SV_SIGNAL_EDITOR_H
