#include "oht.h"


/** *****************   ************************* **/

idev::SvIDevice* OHTSHARED_EXPORT create_device(const QString& params_string)
{  
//  //! обязателен первый аргумент!! парсер считает, что там находится путь к программе
//  QStringList params_list;
//  params_list << "dumb_path_to_app" << params_string.split(" ");
  
//  QCommandLineParser parser;
//  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  
//  parser.addOption(QCommandLineOption(P_SERIAL_BAUDRATE, "BAUDRATE", "19200", "19200"));
//  parser.addOption(QCommandLineOption(P_SERIAL_PORTNAME, "PORTNAME", "0", "0"));
//  parser.addOption(QCommandLineOption(P_SERIAL_DATABITS, "DATABITS", "8", "8"));
//  parser.addOption(QCommandLineOption(P_SERIAL_PARITY,   "PARITY",   "0", "0"));
//  parser.addOption(QCommandLineOption(P_SERIAL_STOPBITS, "STOPBITS", "2", "2"));
//  parser.addOption(QCommandLineOption(P_SERIAL_FLOWCTRL, "FLOWCTRL", "0", "0"));
//  parser.addOption(QCommandLineOption(P_SERIAL_PROTOCOL, "PROTOCOL", "1", "1"));
  
  
//  SerialPortParams serial_params;
//  idev::DeviceConfig device_config;
  
//  if (parser.parse(params_list)) {
    
//    serial_params.baudrate =    parser.value(P_SERIAL_BAUDRATE);
//    serial_params.databits =    parser.value(P_SERIAL_DATABITS);
//    serial_params.flowcontrol = parser.value(P_SERIAL_FLOWCTRL);
//    serial_params.portname =    parser.value(P_SERIAL_PORTNAME);
//    serial_params.parity =      parser.value(P_SERIAL_PARITY);
//    serial_params.stopbits =    parser.value(P_SERIAL_STOPBITS);
////    serial_params. = parser.value(P_SERIAL_);
////    serial_params. = parser.value(P_SERIAL_);
////    serial_params. = parser.value(P_SERIAL_);
    
//    device_config.ifc_protocol_name = parser.value(P_DEVICE_PROTOCOL);
    
    
//  }
//  else
//    return 0;
    
  
  SvOht* oht = new SvOht(/*device_config, serial_params*/);
  
  return oht;
  
}


/** *****************   ************************* **/

SvOht::SvOht(/*const idev::DeviceConfig *config, const idev::SerialPortParams* params*/)
{
  
}

SvOht::~SvOht()
{
  _serial.close();
  
  deleteLater();
}


bool SvOht::open()
{
  if(_serial.isOpen())
    _serial.close();
  
//  _serial->setPortName();
  
  
  
}

void SvOht::close()
{
  
}

bool SvOht::write(const QByteArray* data)
{
  _serial.write((const char*)data->data(), data->size());
  
  return _serial.waitForBytesWritten(500);
}

QByteArray SvOht::read()
{
  QByteArray data = _serial->readAll();
  return data;
}






void SvOht::setSerialPortParams(const SerialPortParams& params)
{ 
  _serial.setPortName(params.name);
  _serial.setBaudRate(params.baudrate);
  _serial.setDataBits(params.databits);
  _serial.setFlowControl(params.flowcontrol);
  _serial.setParity(params.parity);
  _serial.setStopBits(params.stopbits);
}
