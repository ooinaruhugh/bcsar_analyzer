#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "bcsar.h"

STRG_header *read_strg(BCSAR_header *header, FILE *bcsar)
{
  /* STRG_header *strg = calloc(1, sizeof(*strg));
  fseek(bcsar, header->strg_loc, SEEK_SET);
  fread(strg, 0x1C, 1, bcsar);
  strg->offset_table = malloc(strg->length - 0x1C);
  fread(strg->offset_table, strg->length - 0x1C, 1, bcsar);
  return strg; */
  STRG_header *strg = calloc(1, header->strg_length);
  fseek(bcsar, header->strg_loc, SEEK_SET);
  fread(strg, header->strg_length, 1, bcsar);
  return strg; 
}

void *read_partition(FILE *bcsar, uint64_t offset, uint64_t size) {
  void *partition = calloc(1,size);
  fseek(bcsar, offset, SEEK_SET);
  fread(partition, size, 1, bcsar);
  return partition; 
}

void print_strg(STRG_header *strg)
{
}

int main(int argc, char const **argv)
{
  if (argc <= 1)
  {
    printf("Specify filename.\n");
  }
  else
  {
    FILE *bcsar_file;
    BCSAR_header *header;

    bcsar_file = fopen(argv[1], "rb");
    if (!bcsar_file)
    {
      fprintf(stderr, "Something failed when trying to open %s.\n", argv[1]);
      return EXIT_FAILURE;
    }

    header = malloc(sizeof(*header));
    fread(header, sizeof(*header), 1, bcsar_file);

    // Preparing the 4-byte file magic as a zero-terminated string
    char *magic_buffer = calloc(1, 5);
    *((uint32_t *)magic_buffer) = header->magic;

    printf("MAGIC: %s\n", magic_buffer);
    printf("%s endian\n", header->bom == 0xFEFF ? "little" : "big");
    printf("size (as reported in header): 0x%" PRIx32 "\n", header->size);
    printf("Location of STRG: 0x%" PRIx32 "\n", header->strg_loc);
    printf("Length of STRG: 0x%" PRIx32 "\n", header->strg_length);
    printf("Location of INFO: 0x%" PRIx32 "\n", header->info_loc);
    printf("Length of INFO: 0x%" PRIx32 "\n", header->info_length);
    printf("Location of FILE: 0x%" PRIx32 "\n", header->file_loc);
    printf("Length of FILE: 0x%" PRIx32 "\n", header->file_length);

    puts("\n== STRG partition info ==");
    STRG_header *strg = read_strg(header, bcsar_file);
    printf("Filename count: %" PRIu32 "\n", strg->filename_count);
    printf("Lookup table offset: 0x%" PRIx32 "\n", strg->lookup_table + header->strg_loc + 0x8);

    STRG_Lookup_table *lookup_table = (uintptr_t) strg + strg->lookup_table + 0x8;
    printf("Index of the root entry: %"PRIu32"\n", lookup_table->root_index);
    printf("Entry count: %"PRIu32"\n", lookup_table->count);

    puts("\n== INFO partition info ==");
    Info_table *info = read_partition(bcsar_file, header->info_loc, header->info_length);
    printf("Length of partition: %x", info->length);
    /* puts("# File list");

    // char *filelist_name = calloc(strlen(argv[1])+10, 1);
    // sprintf(filelist_name, "%s.list.txt", argv[1]);
    // FILE *filelist = fopen(filelist_name, "a"); 
    for (int i = 0; i < strg->filename_count; i++) {
      char *filename = (uintptr_t)strg + STRG_HEADER_END + strg->offset_table[i].data_offset;
      fprintf(stdout, "%s\n", filename);
    } */

    free(magic_buffer);
    free(strg);
    free(header);
    fclose(bcsar_file);
  }
  return 0;
}
