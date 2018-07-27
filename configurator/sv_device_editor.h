#ifndef SV_DEVICEEDITOR_H
#define SV_DEVICEEDITOR_H

#include <QDialog>
#include <QString>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QSerialPort>
#include <QSerialPortInfo>

//#include "sv_dev_model.h"
//#include "sv_select_model_dialog.h"
//#include "sv_select_brand_dialog.h"
//#include "sv_select_class_dialog.h"
//#include "sv_device_config.h"

#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_exception.h"
#include "sv_kts_editor.h"

#include "../global/sql_defs.h"


namespace Ui {
  class SvDeviceDialog;
}

class SvDeviceEditor : public QDialog
{
    Q_OBJECT
    
  public:
    enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
    enum ShowMode { smNew = 0, smEdit = 1 };
                    
    explicit SvDeviceEditor(QWidget *parent, int deviceId = -1);
    ~SvDeviceEditor();
    
    QString lastError() { return _last_error; }
    
    int showMode;
    
     
  private slots:
    void newKTS();
    void editKTS();
    
    void updateKTSInfo(int index);
    
  public slots:
    void accept() Q_DECL_OVERRIDE;
    
    
  private:
    Ui::SvDeviceDialog *ui;
    
    SvException _exception;
    
    int     _id = -1;
    QString _device_name = "";
    QString _device_port_name = "";
    int _device_kts_id = -1;
    QString _device_type = "";
    QString _device_ifc_name = "";
    QString _device_protocol_name = "";
    QString _device_data_type_name = "";
    QString _device_data_length = "";
    QString _device_driver_path = "";
    QString _device_description = "";
    
    QString _last_error = "";
    
    
    bool loadKTSs();    
    void loadPorts();

    

};

#endif // SV_DEVICEEDITOR_H
