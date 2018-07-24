#ifndef SV_OHT_H
#define SV_OHT_H

#include <QObject>
#include <QSerialPort>

#include "sv_idevice.h"

class SvOHT: public idev::SvISensor
{
public:
  SvOHT();
  ~SvOHT(); 
  
  bool readyRead() { return _ready_read; }
  
  bool open();
  bool close();
  
  bool write(QByteArray& data);
  
  void setSerialPortParams(const idev::SerialPortParams& params);
  
private:
  QSerialPort* _port = nullptr;
  
  bool _ready_read = false;
  
private slots:
  void read();
  
};

#endif // SV_OHT_H
