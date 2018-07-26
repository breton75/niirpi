#ifndef SV_IDEVICE_H
#define SV_IDEVICE_H

#include <QObject>
#include <QMutex>
#include <QMap>
#include <QDateTime>
#include <QMetaType>
#include <QtSerialPort/QSerialPort>

namespace idev {

  enum DeviceTypes {
    sdtUndefined = -1,
    sdtOHT_Gamma12700 = 1
  };

  struct DeviceConfig
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
  
  class SvIDevice;

}


/** ----------- SvIDevice ------------ **/
class idev::SvIDevice : public QObject
{
    Q_OBJECT
    
public:
  SvIDevice() { }
  
  virtual ~SvIDevice() { }
  
  virtual idev::DeviceTypes type() const { return idev::sdtUndefined; }
  
  
  void setLastError(const QString& lastError) { _last_error = lastError; }
  QString lastError() { return _last_error; }
  
  void setDeviceType(idev::DeviceTypes type) { _type = type; }
  idev::DeviceTypes deviceType() { return _type; }
  
  void setOpened(bool isOpened) { _isOpened = isOpened; }
  bool isOpened() { return _isOpened; }
  
  void setActive(bool isActive) { _isActive = isActive; }
  bool isActive() { return _isActive; }
  
  bool isReadyRead() { return _is_ready_read; }
  
  virtual bool write(const QByteArray* data) = 0;
  virtual QByteArray read() = 0;
  
  

protected:
  quint32 _id;
  idev::DeviceTypes _type;
  
  QString _last_error;
  
  bool _isOpened = false;
  bool _isActive = true;
  bool _is_ready_read = false;
  
public slots:
  virtual bool open() = 0;
  virtual void close() = 0;
  
  virtual bool start(quint32 msecs) { }
  virtual void stop() { }
  
    
//signals:
//  void new_data(const svidev::mdata_t& data);
      
};

#endif // SV_IDEVICE_H
