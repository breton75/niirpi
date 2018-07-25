#include "sv_oht.h"

SvOHT::SvOHT()
{
  
}



bool SvOHT::open()
{
  if(!_port.open(QSerialPort::ReadWrite)) { 
    
    setLastError(_port.errorString());
    return false;
  }
  
  _isOpened = _port.isOpen();
  
  connect(&_port, &QSerialPort::readyRead, this, &SvOHT::read);
  
  return _isOpened;
}

bool SvOHT::close()
{
  _port.close();
  disconnect(&_port, &QSerialPort::readyRead, this, &SvOHT::read);
  _isOpened = false;
}

void SvOHT::write(QByteArray &message)
{
//  _log << svlog::Time << svlog::Data << message << svlog::endl;
  _port->write(message);
  _port->waitForBytesWritten(100);
}

void SvOHT::read()
{
  QByteArray b = _port.readAll();
  
  /** parse **/
  
  /** acknowledgment **/
  
  
  /** writing to DB **/
  
  
}


void SvOHT::setSerialPortParams(const idev::SerialPortParams& params)
{ 
  _port.setPortName(params.name);
  _port.setBaudRate(params.baudrate);
  _port.setDataBits(params.databits);
  _port.setFlowControl(params.flowcontrol);
  _port.setParity(params.parity);
  _port.setStopBits(params.stopbits);
}
