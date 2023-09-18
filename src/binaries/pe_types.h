#ifndef CERBERUS_PE_TYPES_H
#define CERBERUS_PE_TYPES_H

#include <cstdint>

struct EXPORT_DIRECTORY_TABLE {
    uint32_t export_flags = 0;
    uint32_t time_date_stamp = 0;
    uint16_t major_version = 0;
    uint16_t minor_version = 0;
    uint32_t name_rva;
    uint32_t ordinal_base = 1;
    uint32_t address_table_entries;
    uint32_t number_of_name_pointers;
    uint32_t export_address_table_rva;
    uint32_t name_pointer_rva;
    uint32_t ordinal_table_rva;
};

#endif //CERBERUS_PE_TYPES_H
