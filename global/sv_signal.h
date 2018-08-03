#ifndef SV_SIGNAL_H
#define SV_SIGNAL_H

#include <QObject>
#include <QDateTime>
#include <QVariant>
#include <QTimer>

enum SignalDataTypes {
  dtInt = 0,
  dtFloat
};

struct SignalParams
{
  SignalParams(SignalDataTypes dataType = dtInt) { data_type = dataType; }
  
  int     id = -1;
  qint64 cob_id = -1;
  QString name = "";
  quint32 data_offset = 0;
  quint32 data_length = 0;
  QString description = "";
  quint32 timeout = 3000;
  int timeout_value = -3;
  SignalDataTypes data_type = dtInt;
  
  
  
  int major_repository_id = -1;
  int minor_repository1_id = -1;
  int minor_repository2_id = -1;
  int minor_repository3_id = -1;
  
  int device_id = -1;
  int device_data_length = -1;
  QString device_name = "";
  QString kts_name = "";
  
//  QString _device_ifc_name = "";
//  QString _device_protocol_name = "";
//  QString _device_data_type_name = "";
//  QString _device_data_length = "";
//  QString _device_driver_path = "";
};

class SvSignal: public QObject
{
  Q_OBJECT
  
public:
  explicit SvSignal(SignalParams& params);
  ~SvSignal();
  
  void setParams(SignalParams& params) { _params = params; }
  const SignalParams* params() { return &_params; }
  
  const QDateTime* lastUpdate()  { return &_last_update; }
  auto value() { return _params.data_type == dtInt ? _value.toInt() : _value.toFloat();  }
  bool isLost() { return _is_lost; }
  
private:
  SignalParams _params;
  
  QDateTime _last_update;
  bool _is_lost = false;
  QVariant _value = QVariant();
  
  QTimer _timer;
  
public slots:
  void setValue(QVariant value);
  
private slots:
  void timeout();
  
};

#endif // SV_SIGNAL_H
