#ifndef SV_IDEVICE_H
#define SV_IDEVICE_H

#include <QObject>
#include <QMutex>
#include <QDialog>
#include <QMap>
#include <QDateTime>
#include <QTextEdit>
#include <QMetaType>
#include <QSerialPort>

#include "../../svlib/sv_log.h"

namespace idev {

  enum SensorTypes {
    sdtUndefined = -1,
    sdtOHT
  };

  struct SensorConfig
  {
    int id = -1;
    QString name = "";
    int ifc_type = -1;
    QString ifc_type_name = "";
    int ifc_protocol = -1;
    QString ifc_protocol_name = "";
    int data_type = 1;  // digital
    QString driver_lib_path = "";
    QString description = "";
    
  };
  
  struct SerialPortParams {
    SerialPortParams() {  }
    SerialPortParams(idev::SensorTypes type) { dev_type = type; }
    idev::SensorTypes dev_type;
    QString description = "";
    QString name = "COM1";
    quint32 baudrate = 9600;
    QSerialPort::DataBits databits = QSerialPort::Data8;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stopbits = QSerialPort::OneStop;
    QSerialPort::FlowControl flowcontrol = QSerialPort::NoFlowControl;
  };
  
  class SvISensor;

}


/** ----------- SvISensor ------------ **/
class idev::SvISensor : public QObject
{
    Q_OBJECT
    
public:
  SvISensor() { }
  
  virtual ~SvIDevice() { }
  
  virtual idev::SensorTypes type() const { return idev::sdtUndefined; }
  
  
  void setLastError(const QString& lastError) { _last_error = lastError; }
  QString lastError() { return _last_error; }
  
  void setSensorType(idev::SensorTypes type) { _type = type; }
  idev::SensorTypes sensorType() { return _type; }
  
  void setOpened(bool isOpened) { _isOpened = isOpened; }
  bool isOpened() { return _isOpened; }
  
  void setActive(bool isActive) { _isActive = isActive; }
  bool isActive() { return _isActive; }

protected:
  quint32 _id;
  idev::SensorTypes _type;
  
  bool _isOpened = false;
  QString _last_error;
  
  bool _isActive = true;
  
public slots:
  virtual bool open() = 0;
  virtual void close() = 0;
  
  virtual bool start(quint32 msecs) { }
  virtual void stop() { }
  
    
//signals:
//  void new_data(const svidev::mdata_t& data);
      
};

#endif // SV_IDEVICE_H
