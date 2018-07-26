#ifndef DEFS_H
#define DEFS_H

#define CR "\n"

#define TREE_HEADERS "|Датчик/Сигнал;|Размер данных (бит);|Смещение (бит);<Параметры;<Примечания" // <Тип интерфейса;<Протокол;<Интерфес;<Тип данных;"

#define SQL_SELECT_FROM_DEVICES \
  "with s as ( " CR \
  "SELECT device_types.id as devtype_id, " CR \
  "   device_types.designation as devtype_designation, " CR \
  "   device_types.ifc_type_id as devtype_ifc_type_id, " CR \
  "   ifc_types.designation as devtype_ifc_type_name, " CR \
  "   device_types.ifc_protocol_id as devtype_ifc_protocol_id, " CR \
  "   ifc_protocols.designation as devtype_ifc_protocol_name, " CR \
  "   device_types.ifc_port_name as devtype_ifc_port_name, " CR \
  "   (select case when device_types.data_type=1 then 'Analog' else 'Digital' end) as device_data_type, " CR \
  "   device_types.data_length as devtype_data_length, " CR \
  "   device_types.driver_lib_path as devtype_driver_lib_path, " CR \
  "   device_types.description as devtype_description " CR \
  "FROM device_types " CR \
  "LEFT JOIN ifc_types ON ifc_types.id = device_types.ifc_type_id " CR \
  "LEFT JOIN ifc_protocols  ON ifc_protocols.id = device_types.ifc_protocol_id ) " CR \
  "SELECT devices.id as device_id, " CR \
  "   devices.device_name as device_name, " CR \
  "   devices.device_type_id as device_type_id, " CR \
  "   devices.description as device_description, " CR \
  "   s.devtype_ifc_type_id as device_ifc_type_id, " CR \
  "   s.devtype_ifc_type_name as device_ifc_type_name, " CR \
  "   s.devtype_ifc_protocol_id as device_ifc_protocol_id, " CR \
  "   s.devtype_ifc_protocol_name as device_ifc_protocol_name, " CR \
  "   s.devtype_ifc_port_name as device_ifc_port_name, " CR \
  "   s.device_data_type as device_data_type, " CR \
  "   s.devtype_data_length as device_data_length, " CR \
  "   s.devtype_driver_lib_path as device_driver_lib_path, " CR \
  "   s.devtype_description as device_type_description " CR \
  "FROM devices " CR \
  "LEFT JOIN s ON s.devtype_id = devices.device_type_id "
                                
#define SQL_SELECT_DEVICES_LIST (SQL_SELECT_FROM_DEVICES CR " ORDER BY device_ifc_type_name ASC")
#define SQL_SELECT_ONE_DEVICE (SQL_SELECT_FROM_DEVICES CR " WHERE device_id = %1")


#define SQL_SELECT_FROM_DEVICE_TYPES \
  "SELECT device_types.id as device_type_id, " CR \
  "   device_types.designation as device_type_designation, " CR \
  "   device_types.ifc_type_id as device_type_ifc_type_id, " CR \
  "   ifc_types.designation as device_type_ifc_type_name, " CR \
  "   device_types.ifc_protocol_id as device_type_ifc_protocol_id, " CR \
  "   ifc_protocols.designation as device_type_ifc_protocol_name, " CR \
  "   device_types.ifc_port_name as device_type_ifc_port_name, " CR \
  "   (select case when device_types.data_type=1 then 'Analog' else 'Digital' end) as device_type_data_type, " CR \
  "   device_types.data_length as device_type_data_length, " CR \
  "   device_types.driver_lib_path as device_type_driver_lib_path, " CR \
  "   device_types.description as device_type_description " CR \
  "FROM device_types " CR \
  "LEFT JOIN ifc_types ON ifc_types.id = device_types.ifc_type_id " CR \
  "LEFT JOIN ifc_protocols  ON ifc_protocols.id = device_types.ifc_protocol_id " CR \

#define SQL_SELECT_DEVICE_TYPES_LIST (SQL_SELECT_FROM_DEVICE_TYPES "ORDER BY device_type_designation ASC")
#define SQL_SELECT_ONE_DEVICE_TYPE (SQL_SELECT_FROM_DEVICE_TYPES " WHERE device_type_id = %1")


#define SQL_SELECT_FROM_SIGNALS \
  "SELECT signals.id as signal_id, " CR \
  "signals.designation as signal_name, " CR \
  "signals.data_offset as signal_data_offset, " CR \
  "signals.data_length as signal_data_length, " CR \
  "signals.description as signal_description " CR \
  "FROM signals " CR \
  "WHERE device_id = %1 "

#define SQL_NEW_DEVICE_TYPE \
  "INSERT INTO device_types (designation, ifc_type, " CR \
  "ifc_port_name, ifc_protocol, data_type, data_length, description)" CR \
  "VALUES ('%1', %2, '%3', %4, %5, %6, '%7');"

#define SQL_UPDATE_DEVICE_TYPE \
  "UPDATE device_types SET designation='%1', ifc_type=%2, ifc_port_name='%3', " CR \
  "ifc_protocol=%4, data_type=%5, data_length=%6, description='%7' " CR \
  "WHERE id = %8;"
  

#define SQL_NEW_DEVICE \
  "INSERT INTO devices (device_name, device_type_id, description)" CR \
  "VALUES ('%1', %2, '%3');"

#define SQL_UPDATE_DEVICE \
  "UPDATE devices SET device_name='%1', device_type_id=%2, description='%3' )" CR \
  "WHERE id = %4;"

#define SQL_SELECT_IFC_TYPES "SELECT id, designation FROM ifc_types ORDER BY designation"

#define SQL_SELECT_IFC_PROTOCOLS "SELECT id, designation FROM ifc_protocols ORDER BY designation"



#endif // DEFS_H
