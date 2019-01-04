#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "bcsar.h"

STRG_header *read_strg(BCSAR_header *header, FILE *bcsar)
{
  STRG_header *strg = calloc(1, sizeof(*strg));
  fseek(bcsar, header->strg_loc, SEEK_SET);
  fread(strg, 0x1C, 1, bcsar);
  strg->offset_table = malloc(strg->length - 0x1C);
  fread(strg->offset_table, strg->length - 0x1C, 1, bcsar);
  return strg;
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

    printf("Offset: 0x%x Length: 0x%x\n", strg->offset_table[0].data_offset, strg->offset_table[0].data_length);
    printf("Offset: 0x%x Length: 0x%x\n", strg->offset_table[1].data_offset, strg->offset_table[1].data_length);
    printf("Offset: 0x%x Length: 0x%x\n", strg->offset_table[strg->filename_count-1].data_offset, strg->offset_table[strg->filename_count-1].data_length);

    puts("# File list");

    char *filename;
    /* char *filelist_name = calloc(strlen(argv[1])+10, 1);
    sprintf(filelist_name, "%s.list.txt", argv[1]);
    FILE *filelist = fopen(filelist_name, "a"); */
    for (int i = 0; i < strg->filename_count; i++) {
      filename = calloc(strg->offset_table[i].data_length + 1, 1);
      fseek(bcsar_file, header->strg_loc + 0x18 + strg->offset_table[i].data_offset, SEEK_SET);
      fread(filename, strg->offset_table[i].data_length, 1, bcsar_file);
      fprintf(stdout, "%s\n", filename);
      free(filename);
    }
    /* printf("0x%x\n", (uintptr_t)&strg->offset_table - (uintptr_t)strg);
    char *test = calloc(1, 0xD);
    memcpy(test, strg->offset_table, 0xC); */
    // char *test = calloc(1, strg->offset_table[0].data_length);

    // puts(test);

    free(magic_buffer);
    free(strg);
    free(header);
    fclose(bcsar_file);
  }
  return 0;
}
