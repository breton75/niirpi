#ifndef DEFS_H
#define DEFS_H

#define CR "\n"

#define TREE_HEADERS "|Датчик/Сигнал;|Размер данных (бит);|Смещение (бит);<Параметры;<Примечания" // <Тип интерфейса;<Протокол;<Интерфес;<Тип данных;"

#define SQL_SELECT_FROM_DEVICES \
  "with s as ( " CR \
  "SELECT ktss.id as kts_id, " CR \
  "   ktss.kts_name as kts_name, " CR \
  "   ktss.ifc_id as kts_ifc_id, " CR \
  "   ifces.ifc_name as kts_ifc_name, " CR \
  "   ktss.protocol_id as kts_protocol_id, " CR \
  "   protocols.protocol_name as kts_protocol_name, " CR \
  "   ktss.data_type as kts_data_type, " CR \
  "   (select case when ktss.data_type=1 then 'Analog' else 'Digital' end) as kts_data_type_name, " CR \
  "   ktss.data_length as kts_data_length, " CR \
  "   ktss.driver_lib_path as kts_driver_lib_path, " CR \
  "   ktss.description as kts_description " CR \
  "FROM ktss " CR \
  "LEFT JOIN ifces ON ifces.id = ktss.ifc_id " CR \
  "LEFT JOIN protocols  ON protocols.id = ktss.protocol_id ) " CR \
  "SELECT devices.id as device_id, " CR \
  "   devices.device_name as device_name, " CR \
  "   devices.kts_id as device_kts_id, " CR \
  "   devices.port_name as device_port_name, " CR \
  "   devices.description as device_description, " CR \
  "   s.kts_name as device_kts_name, " CR \
  "   s.kts_ifc_id as device_ifc_id, " CR \
  "   s.kts_ifc_name as device_ifc_name, " CR \
  "   s.kts_protocol_id as device_protocol_id, " CR \
  "   s.kts_protocol_name as device_protocol_name, " CR \
  "   s.kts_data_type as device_data_type, " CR \
  "   s.kts_data_type_name as device_data_type_name, " CR \
  "   s.kts_data_length as device_data_length, " CR \
  "   s.kts_driver_lib_path as device_driver_lib_path, " CR \
  "   s.kts_description as device_kts_description " CR \
  "FROM devices " CR \
  "LEFT JOIN s ON s.kts_id = devices.kts_id "
                                
#define SQL_SELECT_DEVICES_LIST (SQL_SELECT_FROM_DEVICES CR " ORDER BY device_ifc_name ASC")
#define SQL_SELECT_ONE_DEVICE (SQL_SELECT_FROM_DEVICES CR " WHERE device_id = %1")


#define SQL_SELECT_FROM_KTSS \
  "SELECT ktss.id as kts_id, " CR \
  "   ktss.kts_name as kts_name, " CR \
  "   ktss.ifc_id as kts_ifc_id, " CR \
  "   ifces.ifc_name as kts_ifc_name, " CR \
  "   ktss.protocol_id as kts_protocol_id, " CR \
  "   protocols.protocol_name as kts_protocol_name, " CR \
  "   ktss.data_type as kts_data_type, " CR \
  "   (select case when ktss.data_type=1 then 'Analog' else 'Digital' end) as kts_data_type_name, " CR \
  "   ktss.data_length as kts_data_length, " CR \
  "   ktss.driver_lib_path as kts_driver_lib_path, " CR \
  "   ktss.description as kts_description " CR \
  "FROM ktss " CR \
  "LEFT JOIN ifces ON ifces.id = ktss.ifc_id " CR \
  "LEFT JOIN protocols  ON protocols.id = ktss.protocol_id " CR \

#define SQL_SELECT_KTSS_LIST (SQL_SELECT_FROM_KTSS "ORDER BY kts_name ASC")
#define SQL_SELECT_ONE_KTS (SQL_SELECT_FROM_KTSS " WHERE kts_id = %1")


#define SQL_SELECT_FROM_SIGNALS \
  "SELECT signals.id as signal_id, " CR \
  "signals.designation as signal_name, " CR \
  "signals.data_offset as signal_data_offset, " CR \
  "signals.data_length as signal_data_length, " CR \
  "signals.description as signal_description " CR \
  "FROM signals " CR \
  "WHERE device_id = %1 "

#define SQL_NEW_KTS \
  "INSERT INTO ktss (kts_name, ifc_id, " CR \
  "protocol_id, data_type, data_length, driver_lib_path, description)" CR \
  "VALUES ('%1', %2, %3, %4, %5, '%6', '%7');"

#define SQL_UPDATE_KTS \
  "UPDATE ktss SET kts_name='%1', ifc_id=%2, protocol_id=%3, " CR \
  "data_type=%4, data_length=%5, driver_lib_path='%6', description='%7' " CR \
  "WHERE id = %8;"
  

#define SQL_NEW_DEVICE \
  "INSERT INTO devices (device_name, kts_id, port_name, description)" CR \
  "VALUES ('%1', %2, '%3', '%4');"

#define SQL_UPDATE_DEVICE \
  "UPDATE devices SET device_name='%1', kts_id=%2, port_name='%3', description='%4' " CR \
  "WHERE id = %5;"

#define SQL_SELECT_IFCES "SELECT id, ifc_name FROM ifces ORDER BY ifc_name"

#define SQL_SELECT_PROTOCOLS "SELECT id, protocol_name FROM protocols ORDER BY protocol_name"



#endif // DEFS_H
