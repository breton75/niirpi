#ifndef SV_DEV_DEFS_H
#define SV_DEV_DEFS_H

#include <QtWidgets/QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMap>


struct SerialPortParams {
  QString portname = "COM1";
  quint32 baudrate = 9600;
  QSerialPort::DataBits databits = QSerialPort::Data8;
  QSerialPort::Parity parity = QSerialPort::NoParity;
  QSerialPort::StopBits stopbits = QSerialPort::OneStop;
  QSerialPort::FlowControl flowcontrol = QSerialPort::NoFlowControl;
  QString description = "";
};
  
  const QList<int> Baudrates = {75, 115, 134, 150, 300, 600, 1200, 1800, 2400, 4800, 7200, 9600, 14400, 19200, 38400, 57600, 115200, 128000};
  
  const QMap<QSerialPort::DataBits, QString> DataBits = {{QSerialPort::Data5, "5"}, 
                                                         {QSerialPort::Data6, "6"},
                                                         {QSerialPort::Data7, "7"},
                                                         {QSerialPort::Data8, "8"}};
  
  const QMap<QSerialPort::Parity, QString> Parities = {{QSerialPort::NoParity, "Нет"},
                                                       {QSerialPort::EvenParity, "Чет"},
                                                       {QSerialPort::OddParity, "Нечет"},
                                                       {QSerialPort::SpaceParity, "Пробел"},
                                                       {QSerialPort::MarkParity, "Маркер"}};
  
  const QMap<QSerialPort::StopBits, QString> StopBits = {{QSerialPort::OneStop, "1"},
                                                         {QSerialPort::OneAndHalfStop, "1,5"},
                                                         {QSerialPort::TwoStop, "2"}};
  
  const QMap<QSerialPort::FlowControl, QString> FlowControls = {{QSerialPort::NoFlowControl, "Нет"},
                                                                {QSerialPort::HardwareControl, "Аппаратное"},
                                                                {QSerialPort::SoftwareControl, "Программное"}};

#endif // SV_DEV_DEFS_H
