
#include <stdint.h>

#ifndef BCSAR_H
#define BCSAR_H

// Format specifications found at 3dbrew.org

#define CSAR_MAGIC 0x52415343
#define STRG_MAGIC 0x47525453
#define INFO_MAGIC 0x4F464E49
#define FILE_MAGIC 0x454C4946


typedef struct BCSAR_header
{
  uint32_t magic;
  uint16_t bom;
  uint16_t header_size;
  uint32_t version;
  uint32_t file_size;
  uint32_t partition_count;
  uint32_t partition_header_size;
  uint32_t strg_loc;
  uint32_t strg_length;
  uint32_t offset_0x20;
  uint32_t info_loc;
  uint32_t info_length;
  uint32_t file_loc;
  uint32_t file_length; // todo: needs rename
  uint32_t offfset_0x38;
  uint32_t offfset_0x3C;
} BCSAR_header;

typedef struct Offset_entry Offset_entry;

typedef struct STRG_header
{
  uint32_t magic;
  uint32_t length;
  uint32_t table_type;
  uint32_t string_table; // maybe, "this + 8 points to the string table (always 0x10)"
  uint32_t lookup_type;
  uint32_t lookup_table; // again, "this + 8"
  uint32_t filename_count;
  Offset_entry *offset_table;
} STRG_header;

struct Offset_entry
{
  uint32_t node_type;
  uint32_t data_offset; // from the end of the string header
  uint32_t data_length;
};

typedef struct Lookup_entry Lookup_entry;

typedef struct Lookup_table
{
  uint32_t root_index;
  uint32_t count;
  Lookup_entry *entries;
} Lookup_table;

struct Lookup_entry
{
  uint16_t has_data;
  uint16_t bit_test;
  uint32_t fail_leaf_index;
  uint32_t success_leaf_index;
  uint32_t lookup_index;

  /* union resid
  {
     char bytes[3];
     uint32_t id:12;
  }; */
  char res_id[3];
  char res_type;
};

typedef struct Info_table
{
  uint32_t magic;
  uint32_t length;
  struct
  {
    uint32_t magic;
    uint32_t offset;
  } header_entry[8];
} Info_table;

typedef struct magic0x2200 {
  uint32_t offset0x0;
  uint32_t snd_player_id;
  uint32_t offset0x8;
  uint32_t ext_info_type;
  uint32_t ext_info_offset;
  char *more;
} magic0x220;

typedef struct magic0x2204 {
  uint32_t snd_id;
  uint32_t another_snd_id;
  uint32_t ext_info_type;
  uint32_t ext_info_offset;
  uint32_t another_ext_info_type;
  uint32_t another_ext_info_offset;
  uint32_t offset0x18;
  uint32_t offset0x1c;
};

typedef struct magic0x2206 {
  uint32_t offset0x0;
  uint32_t ext_info_type;
  uint32_t ext_info_offset;
  uint32_t offset0xC;
  uint32_t offset0x10;
} magic0x2206;

#endif // BCSAR_H