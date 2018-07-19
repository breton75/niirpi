#ifndef DEFS_H
#define DEFS_H

#define CR '\n'

#define SQL_SELECT_FROM_SENSORS "SELECT sensors.id as id, " CR \
                                "sensors.designation as sensor_name, " CR \
                                "ifc_types.designation as ifc_type_name, " CR \
                                "ifc_protocols.designation as ifc_protocol_name, " CR \
                                "sensors.ifc_name as ifc_name, " CR \
                                "(select case when sensors.data_type=1 then 'Analog' else 'Digital' end) as data_type, " CR \
                                "sensors.data_length as data_length, " CR \
                                "sensors.description as description " CR \
                                "FROM sensors " CR \
                                "LEFT JOIN ifc_types ON sensors.ifc_type = ifc_types.id " CR \
                                "LEFT JOIN ifc_protocols ON sensors.ifc_protocol = ifc_protocols.id"





#endif // DEFS_H
