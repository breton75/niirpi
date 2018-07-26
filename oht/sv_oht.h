#ifndef OHT_H
#define OHT_H

#include <QtCore/QCommandLineParser>

#include <QtCore/qglobal.h>

#if defined(OHT_LIBRARY)
#  define OHTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define OHTSHARED_EXPORT Q_DECL_IMPORT
#endif


#include "../global/sv_idevice.h"
#include "../global/dev_defs.h"
#include "../../svlib/sv_exception.h"


#define P_SERIAL_BAUDRATE "baudrate"
#define P_SERIAL_PORTNAME "portname"
#define P_SERIAL_DATABITS "databits"
#define P_SERIAL_PARITY   "parity"
#define P_SERIAL_STOPBITS "stopbits"
#define P_SERIAL_FLOWCTRL "flowcontrol"
#define P_DEVICE_PROTOCOL "protocol"


idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string);

class /*OHTSHARED_EXPORT*/ SvOHT: public idev::SvIDevice
{
  
public:
  SvOHT(idev::DeviceConfig deviceConfig, SerialPortParams serialParams);
  ~SvOHT();
  
  
  idev::DeviceTypes type() const { return idev::sdtOHT; }
  
  bool readyRead() { return _ready_read; }
  
  bool open();
  void close();
  
  bool write(const QByteArray* data);
  QByteArray read();
  
  void setSerialPortParams(const SerialPortParams& params);
  
private:
  QSerialPort _serial;
  
  SerialPortParams _serial_params;
  bool _ready_read = false;
  
  
};

#endif // OHT_H
