#include <stdio.h>
#include <stdlib.h>
#include "bcsar.h"

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
    char *magic = calloc(1,5);
    *((uint32_t *)magic) = header->magic;

    printf("MAGIC: %s\nsize (as reported in header): %lu\nLocation of STRG: 0x%lx\nLocation of INFO: 0x%lx\nLocation of FILE: 0x%lx\n", 
            magic, 
            header->size,
            header->strg_loc,
            header->info_loc,
            header->file_length);
  }
  return 0;
}
