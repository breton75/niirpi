#ifndef DEFS_H
#define DEFS_H

#define CR "\n"

#define DEVICES_TREE_HEADERS "|Датчик/Сигнал;|Размер данных (бит);|Смещение (бит);<Параметры;<Примечания" // <Тип интерфейса;<Протокол;<Интерфес;<Тип данных;"

#define REPOSITORIES_TREE_HEADERS "|Репозиторий;|Сервер;|Порт;|Логин;|База данных;|Таблица"


/** *********** SQLITE *************** **/

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
  "   devices.connection_params as device_connection_params, " CR \
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
  "with " CR \
  " minor1 AS ( " CR \
  "    SELECT id, repository_name, host, port, " CR \
  "            login, pass, dbname,table_name " CR \
  "    FROM repositories), " CR \
  " minor2 AS ( " CR \
  "    SELECT id, repository_name, host, port, " CR \
  "            login, pass, dbname,table_name " CR \
  "    FROM repositories), " CR \
  " minor3 AS ( " CR \
  "    SELECT id, repository_name, host, port, " CR \
  "            login, pass, dbname,table_name " CR \
  "    FROM repositories), " CR \
  "d AS (" SQL_SELECT_FROM_DEVICES ") " CR \
  "SELECT signals.id as signal_id, " CR \
  "   signals.device_id as signal_device_id, " CR \
  "   signals.cob_id as signal_cob_id, " CR \
  "   d.device_name as signal_device_name, " CR \
  "   d.device_kts_name as signal_kts_name, " CR \
  "   signals.signal_name as signal_name, " CR \
  "   signals.timeout as signal_timeout, " CR \
  "   signals.timeout_value as signal_timeout_value, " CR \
  "   signals.data_offset as signal_data_offset, " CR \
  "   signals.data_length as signal_data_length, " CR \
  "   signals.data_type as signal_data_type, " CR \
  "   signals.description as signal_description, " CR \
  "   repositories.id as              major_repository_id, " CR \
  "   repositories.repository_name as major_repository_name, " CR \
  "   repositories.host as            major_repository_host, " CR \
  "   repositories.port as            major_repository_port, " CR \
  "   repositories.login as           major_repository_login, " CR \
  "   repositories.pass as            major_repository_pass, " CR \
  "   repositories.dbname as          major_repository_dbname, " CR \
  "   repositories.table_name as      major_repository_table_name, " CR \
  "   minor1.id as              minor_repository1_id, " CR \
  "   minor1.repository_name as minor_repository1_name, " CR \
  "   minor1.host as            minor_repository1_host, " CR \
  "   minor1.port as            minor_repository1_port, " CR \
  "   minor1.login as           minor_repository1_login, " CR \
  "   minor1.pass as            minor_repository1_pass, " CR \
  "   minor1.dbname as          minor_repository1_dbname, " CR \
  "   minor1.table_name as      minor_repository1_table_name, " CR \
  "   minor2.id as              minor_repository2_id, " CR \
  "   minor2.repository_name as minor_repository2_name, " CR \
  "   minor2.host as            minor_repository2_host, " CR \
  "   minor2.port as            minor_repository2_port, " CR \
  "   minor2.login as           minor_repository2_login, " CR \
  "   minor2.pass as            minor_repository2_pass, " CR \
  "   minor2.dbname as          minor_repository2_dbname, " CR \
  "   minor2.table_name as      minor_repository2_table_name, " CR \
  "   minor3.id as              minor_repository3_id, " CR \
  "   minor3.repository_name as minor_repository3_name, " CR \
  "   minor3.host as            minor_repository3_host, " CR \
  "   minor3.port as            minor_repository3_port, " CR \
  "   minor3.login as           minor_repository3_login, " CR \
  "   minor3.pass as            minor_repository3_pass, " CR \
  "   minor3.dbname as          minor_repository3_dbname, " CR \
  "   minor3.table_name as      minor_repository3_table_name " CR \
  "FROM signals " CR \
  "LEFT JOIN d ON d.device_id = signals.device_id " CR \
  "LEFT JOIN repositories ON repositories.id = signals.major_repository_id " CR \
  "LEFT JOIN minor1 ON minor1.id = signals.minor_repository1_id " CR \
  "LEFT JOIN minor2 ON minor2.id = signals.minor_repository2_id " CR \
  "LEFT JOIN minor3 ON minor3.id = signals.minor_repository3_id "

#define SQL_SELECT_DEVICE_SIGNALS (SQL_SELECT_FROM_SIGNALS " WHERE signals.device_id = %1")
#define SQL_SELECT_SIGNALS_LIST (SQL_SELECT_FROM_SIGNALS " ORDER BY signals.data_offset ASC")
#define SQL_SELECT_ONE_SIGNAL (SQL_SELECT_FROM_SIGNALS " WHERE signals.id = %1")


#define SQL_SELECT_FROM_REPOSITORIES \
  "SELECT repositories.id as repository_id, " CR \
  "   repositories.repository_name as repository_name, " CR \
  "   repositories.host as repository_host, " CR \
  "   repositories.port as repository_port, " CR \
  "   repositories.login as repository_login, " CR \
  "   repositories.pass as repository_pass, " CR \
  "   repositories.dbname as repository_dbname, " CR \
  "   repositories.table_name as repository_table_name " CR \
  "FROM repositories"

#define SQL_SELECT_REPOSITORIES_LIST (SQL_SELECT_FROM_REPOSITORIES " ORDER BY repositories.host ASC")
#define SQL_SELECT_ONE_REPOSITORY (SQL_SELECT_FROM_REPOSITORIES " WHERE repositories.id in (%1)")


#define SQL_NEW_KTS \
  "INSERT INTO ktss (kts_name, ifc_id, protocol_id, data_type, " CR \
  "data_length, driver_lib_path, description)" CR \
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


#define SQL_NEW_REPOSITORY \
  "INSERT INTO repositories ( " CR \
  "     repository_name, host, port, login, pass, dbname, table_name) " CR \
  "VALUES ('%1', '%2', %3, '%4', '%5', '%6', '%7');"

#define SQL_UPDATE_REPOSITORY \
  "UPDATE repositories SET " CR \
  "   repository_name='%1', host='%2', port=%3, login='%4', pass='%5', " \
  "   dbname='%6', table_name='%7' " CR \
  "WHERE id = %8"


#define SQL_NEW_SIGNAL \
  "INSERT INTO signals ( " CR \
  "   device_id, signal_name, timeout, data_type, data_offset, data_length, " \
  "   description, major_repository_id, minor_repository1_id, " \
  "   minor_repository2_id, minor_repository3_id ) " CR \
  "VALUES (%1, '%2', %3, %4, %5, %6, '%7', %8, %9, %10, %11); "

#define SQL_UPDATE_SIGNAL \
  "UPDATE signals SET " CR \
  "   device_id=%1, signal_name='%2', timeout=%3, data_type=%4, data_offset=%5, " \
  "   data_length=%6, description='%7', major_repository_id=%8, " \
  "   minor_repository1_id=%9, minor_repository2_id=%10, minor_repository3_id=%11 " CR \
  "WHERE id = %12 "


#define SQL_SELECT_IFCES "SELECT id, ifc_name FROM ifces ORDER BY ifc_name"

#define SQL_SELECT_PROTOCOLS "SELECT id, protocol_name FROM protocols ORDER BY protocol_name"



/** *********** POSTGRES *************** **/

#define SQL_UPDATE_REPOSITORY \
  "UPDATE %1 SET value=%2, last_update='%3' " CR \
  "WHERE cob_id = %4"



#endif // DEFS_H

