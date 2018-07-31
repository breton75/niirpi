#ifndef CORE_H
#define CORE_H


#include <QtCore/qglobal.h>

#if defined(CORE_LIBRARY)
#  define CORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define CORESHARED_EXPORT Q_DECL_IMPORT
#endif


struct CORESHARED_EXPORT SensorConfig
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

class CORESHARED_EXPORT Core
{
  
public:
  Core();
  
  virtual bool open();
  virtual void close();
  
  virtual bool write();
  
};

#endif // CORE_H
