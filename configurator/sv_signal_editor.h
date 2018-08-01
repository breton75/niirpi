#ifndef SV_SIGNAL_EDITOR_H
#define SV_SIGNAL_EDITOR_H

#include <QDialog>

#include "ui_sv_signal_editor.h"

#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_exception.h"
#include "../global/sql_defs.h"

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
  
  int     _signal_id = -1;
  QString _signal_name = "";
  quint32 _signal_data_offset = 0;
  quint32 _signal_data_length = 0;
  QString _signal_description = "";
  int _signal_major_repository_id = -1;
  int _signal_minor_repository1_id = -1;
  int _signal_minor_repository2_id = -1;
  int _signal_minor_repository3_id = -1;
  
  QString _kts_name = "";
  QString _device_name = "";
  int _signal_device_id = -1;
  
  QString _device_ifc_name = "";
  QString _device_protocol_name = "";
  QString _device_data_type_name = "";
  QString _device_data_length = "";
  QString _device_driver_path = "";
  
  QString _last_error = "";
  
  void setCurrentRepositories();
  
private slots:
  void newRepoisitory();
  
  bool loadRepositories();
  
};

#endif // SV_SIGNAL_EDITOR_H
