#ifndef SV_SENSOR_H
#define SV_SENSOR_H

#include <QDialog>
#include <QString>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

#include "ui_sv_sensor.h"
//#include "sv_dev_model.h"
//#include "sv_select_model_dialog.h"
//#include "sv_select_brand_dialog.h"
//#include "sv_select_class_dialog.h"
//#include "sv_device_config.h"

#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_exception.h"

#include "defs.h"

//#include "sv_zone_list.h"


namespace Ui {
  class SvDeviceDialog;
}

class SvSensor : public QDialog
{
    Q_OBJECT
    
  public:
    enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
    enum ShowMode { smNew = 0, smEdit = 1 };
                    
    explicit SvSensor(QWidget *parent,
                      int deviceId = -1);
 
    
    ~SvSensor();
    
    QString lastError() { return _last_error; }
    
    int showMode;
    
     
  private slots:
    void selectDriver();
    
  public slots:
    void accept() Q_DECL_OVERRIDE;
    
    
  private:
    Ui::SvSensorDialog *ui;
    
    SvException _exception;
    
    int     _id = -1;
    QString _sensor_name = "";
    qint32 _data_length = 0;
    int _ifc_type = -1;
    int _ifc_protocol = -1;
    int _data_type = -1;
    QString _ifc_port = "";
    QString _class_name = "";
    QString _brand_name = "";
    QString _driver_path = "";
    QString _description = "";
    
    QString _last_error = "";
    
    
    void loadIfcTypes();    
    void loadIfcProtocols();
    void loadDataTypes();
    

};

#endif // SV_SENSOR_H
