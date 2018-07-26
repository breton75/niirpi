#include "sv_oht.h"


/** *****************   ************************* **/

idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string)
{  
  //! обязателен первый аргумент!! парсер считает, что там находится путь к программе
  QStringList params_list;
  params_list << "dumb_path_to_app" << params_string.split(" ");
  
  QCommandLineParser parser;
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  
  parser.addOption(QCommandLineOption(P_SERIAL_BAUDRATE, "BAUDRATE", "19200", "19200"));
  parser.addOption(QCommandLineOption(P_SERIAL_PORTNAME, "PORTNAME", "0", "0"));
  parser.addOption(QCommandLineOption(P_SERIAL_DATABITS, "DATABITS", "8", "8"));
  parser.addOption(QCommandLineOption(P_SERIAL_PARITY,   "PARITY",   "0", "0"));
  parser.addOption(QCommandLineOption(P_SERIAL_STOPBITS, "STOPBITS", "2", "2"));
  parser.addOption(QCommandLineOption(P_SERIAL_FLOWCTRL, "FLOWCTRL", "0", "0"));
  parser.addOption(QCommandLineOption(P_DEVICE_PROTOCOL, "PROTOCOL", "1", "1"));
  
  
  SerialPortParams serial_params;
  idev::DeviceConfig device_config;
  
  SvException exception;
  quint32 d;
  bool ok;
  
  try {
    
    if(!parser.parse(params_list)) exception.raise("wrong params");      

    serial_params.baudrate = parser.value(P_SERIAL_BAUDRATE).toUInt(&ok);
    if(!ok) exception.raise("wrong params");
    
    d = parser.value(P_SERIAL_DATABITS).toUInt(&ok);
    if(!(ok & DataBits.contains(QSerialPort::DataBits(d)))) exception.raise("wrong params");
    serial_params.databits = QSerialPort::DataBits(d);
        
    d = parser.value(P_SERIAL_FLOWCTRL).toUInt(&ok);
    if(!(ok & FlowControls.contains(QSerialPort::FlowControl(d)))) exception.raise("wrong params");
    serial_params.flowcontrol = QSerialPort::FlowControl(d);
    
    serial_params.portname =    parser.value(P_SERIAL_PORTNAME).toUpper();
    
    d = parser.value(P_SERIAL_PARITY).toUInt(&ok);
    if(!ok) exception.raise("wrong params");
    serial_params.parity = QSerialPort::Parity(d);
        
    d = parser.value(P_SERIAL_STOPBITS).toUInt(&ok);
    if(!ok) exception.raise("wrong params");
    serial_params.stopbits = QSerialPort::StopBits(d);
    
//    serial_params. = parser.value(P_SERIAL_);
//    serial_params. = parser.value(P_SERIAL_);
//    serial_params. = parser.value(P_SERIAL_);
    
    device_config.ifc_protocol_name = parser.value(P_DEVICE_PROTOCOL).toUtf8();
      
  }
  
  catch(SvException& e) {
    
    return 0;
  }
    
  
  SvOHT* oht = new SvOHT(device_config, serial_params);
  
  return oht;
  
}


/** *****************   ************************* **/

SvOHT::SvOHT(idev::DeviceConfig deviceConfig, SerialPortParams serialParams)
{
  
}

SvOHT::~SvOHT()
{
  _serial.close();
  
  deleteLater();
}


bool SvOHT::open()
{
  if(!_serial.open(QSerialPort::ReadWrite)) { 
    
    setLastError(_serial.errorString());
    return false;
  }
  
  _isOpened = _serial.isOpen();
  
  connect(&_serial, &QSerialPort::readyRead, this, &SvOHT::read);
  
  return _isOpened;
}

void SvOHT::close()
{
  _serial.close();
  disconnect(&_serial, &QSerialPort::readyRead, this, &SvOHT::read);
  _isOpened = false;
}

bool SvOHT::write(const QByteArray* data)
{
  _serial.write((const char*)data->data(), data->size());
  
  return _serial.waitForBytesWritten(500);
}

QByteArray SvOHT::read()
{
  QByteArray data = _serial.readAll();
  
  
  /** parse **/
  
  /** acknowledgment **/
  
  
  /** writing to DB **/
  
//  return data;
}






void SvOHT::setSerialPortParams(const SerialPortParams& params)
{ 
  _serial.setPortName(params.portname);
  _serial.setBaudRate(params.baudrate);
  _serial.setDataBits(params.databits);
  _serial.setFlowControl(params.flowcontrol);
  _serial.setParity(params.parity);
  _serial.setStopBits(params.stopbits);
}
