#ifndef SV_DEVICETYPEEDITOR_H
#define SV_DEVICETYPEEDITOR_H

#include <QDialog>
#include <QString>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

#include "ui_sv_device_type_editor.h"
//#include "sv_dev_model.h"
//#include "sv_select_model_dialog.h"
//#include "sv_select_brand_dialog.h"
//#include "sv_select_class_dialog.h"
//#include "sv_device_config.h"

#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_exception.h"

#include "../global/sql_defs.h"


namespace Ui {
  class SvDeviceTypeDialog;
}

class SvDeviceTypeEditor : public QDialog
{
    Q_OBJECT
    
  public:
    enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
    enum ShowMode { smNew = 0, smEdit = 1 };
                    
    explicit SvDeviceTypeEditor(QWidget *parent, int typeId = -1);
    ~SvDeviceTypeEditor();
    
    QString lastError() { return _last_error; }
    int id() { return _id; }
    
    int showMode;
    
     
  private slots:
    void selectDriver();
    
  public slots:
    void accept() Q_DECL_OVERRIDE;
    
    
  private:
    Ui::SvDeviceTypeDialog *ui;
    
    SvException _exception;
    
    int     _device_type_id = -1;
    QString _device_type_name = "";
    int     _device_type_ifc_type_id = -1;
    QString _device_type_ifc_type_name = "";
    int     _device_type_ifc_protocol_id = -1;
    QString _device_type_ifc_protocol_name = "";
    QString _device_type_ifc_port_name = "";
    int     _device_type_data_type = -1;
    quint32 _device_type_data_length = 0;
    QString _device_type_driver_path = "";
    QString _device_type_description = "";
    
    QString _last_error = "";
    
    
    void loadIfcTypes();    
    void loadIfcProtocols();
    void loadDataTypes();
    

};

#endif // SV_DEVICETYPEEDITOR_H
