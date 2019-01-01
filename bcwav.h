
#include <stdint.h>

#ifndef BCWAV_H
#define BCWAV_H


#define CWAV_MAGIC 0x56415743

typedef struct sized_ref {
  uint64_t ref; // todo: is a struct itself
  uint32_t size;
} sized_ref;

typedef struct BCWAV_header {
  uint32_t magic;
  uint16_t bom;
  uint16_t header_size;
  uint32_t version;
  uint32_t file_size;
  uint16_t block_count;
  uint16_t reserved;
  sized_ref info_offset;
  sized_ref data_offset;
} BCWAV_header;

#endif // BCWAV_H