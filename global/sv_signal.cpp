#include "sv_signal.h"

SvSignal::SvSignal(SignalParams& params)
{
  setParams(params);
  
//  connect(&_timer, &QTimer::timeout, this, &SvSignal::timeout);
  
}

SvSignal::~SvSignal()
{
  deleteLater();
}

void SvSignal::setValue(QVariant value)
{
  _value = value;
  _last_update = QDateTime::currentDateTime();
  
//  _is_lost = false;
//  _timer.start(_params.timeout);
}

void SvSignal::timeout()
{
//  _is_lost = true;
}
