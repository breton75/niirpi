#ifndef DEFS_H
#define DEFS_H

#define CR "\n"

#define TREE_HEADERS "|Датчик/Сигнал;|Размер данных (бит);|Смещение (бит);<Параметры;<Примечания" // <Тип интерфейса;<Протокол;<Интерфес;<Тип данных;"

#define SQL_SELECT_FROM_SENSORS "SELECT sensors.id as sensor_id, " CR \
                                "sensors.designation as sensor_name, " CR \
                                "ifc_types.designation as sensor_ifc_type_name, " CR \
                                "ifc_protocols.designation as sensor_ifc_protocol_name, " CR \
                                "sensors.ifc_name as sensor_ifc_name, " CR \
                                "(select case when sensors.data_type=1 then 'Analog' else 'Digital' end) as sensor_data_type, " CR \
                                "sensors.data_length as sensor_data_length, " CR \
                                "sensors.description as sensor_description " CR \
                                "FROM sensors " CR \
                                "LEFT JOIN ifc_types ON sensors.ifc_type = ifc_types.id " CR \
                                "LEFT JOIN ifc_protocols ON sensors.ifc_protocol = ifc_protocols.id " CR \
                                "ORDER BY sensor_name ASC"


#define SQL_SELECT_FROM_SIGNALS "SELECT signals.id as signal_id, " CR \
                                "signals.designation as signal_name, " CR \
                                "signals.data_offset as signal_data_offset, " CR \
                                "signals.data_length as signal_data_length, " CR \
                                "signals.description as signal_description " CR \
                                "FROM signals " CR \
                                "WHERE sensor_id = %1 "


#endif // DEFS_H
