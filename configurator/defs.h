#ifndef DEFS_H
#define DEFS_H

#define CR "\n"

#define TREE_HEADERS "|Датчик/Сигнал;|Размер данных (бит);|Смещение (бит);<Параметры;<Примечания" // <Тип интерфейса;<Протокол;<Интерфес;<Тип данных;"

#define SQL_SELECT_FROM_SENSORS "SELECT sensors.id as sensor_id, " CR \
                                "sensors.designation as sensor_name, " CR \
                                "sensors.ifc_type as sensor_ifc_type, " CR \
                                "ifc_types.designation as sensor_ifc_type_name, " CR \
                                "sensors.ifc_protocol as sensor_ifc_protocol, " CR \
                                "ifc_protocols.designation as sensor_ifc_protocol_name, " CR \
                                "sensors.ifc_port_name as sensor_ifc_port_name, " CR \
                                "(select case when sensors.data_type=1 then 'Analog' else 'Digital' end) as sensor_data_type, " CR \
                                "sensors.data_length as sensor_data_length, " CR \
                                "sensors.driver_lib_path as sensor_driver_lib_path, " CR \
                                "sensors.description as sensor_description " CR \
                                "FROM sensors " CR \
                                "LEFT JOIN ifc_types ON sensors.ifc_type = ifc_types.id " CR \
                                "LEFT JOIN ifc_protocols ON sensors.ifc_protocol = ifc_protocols.id " CR
                                
#define SQL_SELECT_SENSORS_LIST (SQL_SELECT_FROM_SENSORS "ORDER BY sensor_name ASC")
#define SQL_SELECT_ONE_SENSOR (SQL_SELECT_FROM_SENSORS " WHERE sensor_id = %1")



#define SQL_SELECT_FROM_SIGNALS "SELECT signals.id as signal_id, " CR \
                                "signals.designation as signal_name, " CR \
                                "signals.data_offset as signal_data_offset, " CR \
                                "signals.data_length as signal_data_length, " CR \
                                "signals.description as signal_description " CR \
                                "FROM signals " CR \
                                "WHERE sensor_id = %1 "

#define SQL_NEW_SENSOR "INSERT INTO sensors (designation, ifc_type, " CR \
                       "ifc_port_name, ifc_protocol, data_type, data_length, description)" CR \
                       "VALUES ('%1', %2, '%3', %4, %5, %6, '%7');"

#define SQL_UPDATE_SENSOR "UPDATE sensors SET designation='%1', ifc_type=%2, " CR \
                       "ifc_port_name='%3', ifc_protocol=%4, data_type=%5, data_length=%6, description='%7')"


#define SQL_SELECT_IFC_TYPES "SELECT id, designation FROM ifc_types ORDER BY designation"

#define SQL_SELECT_IFC_PROTOCOLS "SELECT id, designation FROM ifc_protocols ORDER BY designation"



#endif // DEFS_H
