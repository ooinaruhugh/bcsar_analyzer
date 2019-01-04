#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
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

    printf("MAGIC: %s\n", magic);
    printf("size (as reported in header): 0x%"PRIx32"\n", header->size);
    printf("Location of STRG: 0x%"PRIx32"\n", header->strg_loc);
    printf("Location of INFO: 0x%"PRIx32"\n", header->info_loc);
    printf("Location of FILE: 0x%"PRIx32"\n", header->file_loc);

  }
  return 0;
}
