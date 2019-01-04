#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "bcsar.h"

void print_strg(STRG_header *strg) {
  
}

int main(int argc, char const **argv)
{
  if (argc <= 1){
    printf("Specify filename.\n");
  }
  else {
    FILE *bcsar_file;
    BCSAR_header *header;

    bcsar_file = fopen(argv[1], "rb");
    if (!bcsar_file) {
      fprintf(stderr, "Something failed when trying to open %s.\n", argv[1]);
      return EXIT_FAILURE;
    }

    header = malloc(sizeof(*header));
    fread(header, sizeof(*header), 1, bcsar_file);

    // Preparing the 4-byte file magic as a zero-terminated string
    char *magic_buffer = calloc(1,5);
    *((uint32_t *)magic_buffer) = header->magic;

    printf("MAGIC: %s\n", magic_buffer);
    printf("%s endian\n", header->bom == 0xFEFF ? "little" : "big");
    printf("size (as reported in header): 0x%"PRIx32"\n", header->size);
    printf("Location of STRG: 0x%"PRIx32"\n", header->strg_loc);
    printf("Location of INFO: 0x%"PRIx32"\n", header->info_loc);
    printf("Location of FILE: 0x%"PRIx32"\n", header->file_loc);

    puts("\n== STRG partition info ==");
    STRG_header *strg = malloc(header->strg_length);
    fseek(bcsar_file, header->strg_loc, SEEK_SET);
    fread(strg, header->strg_length, 1, bcsar_file);
    printf("Filename count: %"PRIu32"\n", strg->filename_count);


    free(magic_buffer);
    free(strg);
    free(header);
    fclose(bcsar_file);
  }
  return 0;
}
