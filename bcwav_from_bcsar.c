/* 
  Quick and dirty program to extract bcwav sound files from
  a bcsar sound archive.

  31.12.18 by ooinaruhugh
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bcsar.h"
#include "bcwav.h"

#define VERBOSE 1
#define DEBUG 1
#define ONLY_WHAT_IF 1

int main(int argc, char const *argv[])
{
  if (argc <= 1)
  {
    printf("Usage: %s infile.\n", argv[0]);
  }
  else
  {
#if VERBOSE
    printf("Opening %s\n", argv[1]);
#endif
    // Open the input file
    FILE *bcsar_file = fopen(argv[1], "rb");
    if (!bcsar_file)
    {
      fprintf(stderr, "Something failed when trying to open %s.\n", argv[1]);
      fflush(stderr);
      return EXIT_FAILURE;
    }

    // Retrieve the header of the bcsar
    BCSAR_header *header = malloc(sizeof(*header));
    fread(header, sizeof(*header), 1, bcsar_file);

    // Check whether we actually have a bcsar
    if (header->magic != CSAR_MAGIC)
    {
      fprintf(stderr, "%s doesn't seem to be a valid BCSAR file.\n", argv[1]);
      fclose(bcsar_file);
      free(header);
      fflush(stderr);
      return EXIT_FAILURE;
    }

    // Preparing a buffer to contain four bytes from our input
    char *buffer;
    uint32_t *magic_buffer;;
    uint32_t next_char;

    magic_buffer = buffer = calloc(5, sizeof(*buffer));
    fread(buffer, 4, 1, bcsar_file);

    unsigned int file_count = 1;
    do
    {
#if VERBOSE
      puts("Searching for BCWAV...");
#endif
      while (*magic_buffer != CWAV_MAGIC)
      {
        buffer[0] = buffer[1];
        buffer[1] = buffer[2];
        buffer[2] = buffer[3];
        buffer[3] = next_char;

        if ((next_char = getc(bcsar_file)) == EOF)
          return EXIT_SUCCESS;
      }

      fseek(bcsar_file, -5, SEEK_CUR);
#if VERBOSE
      printf("Found BCWAV no.%i at Offset 0x%x\n", file_count, ftell(bcsar_file));

      puts("Retrieving BCWAV header");
#endif
      // Now get the header of the recently found bcwav
      BCWAV_header *cwav_header = malloc(sizeof(*cwav_header));
      fread(cwav_header, sizeof(*cwav_header), 1, bcsar_file);
      fseek(bcsar_file, -sizeof(*cwav_header), SEEK_CUR);

#if VERBOSE
      printf("Now at %x\n", ftell(bcsar_file));

      char *magic = calloc(1, 5);
      *((uint32_t *)magic) = cwav_header->magic;

      printf("BCWAV of size: %i Magic: %s\n", cwav_header->file_size, magic);
      free(magic);

      printf("Retrieving BCWAV file of size %i\n", cwav_header->file_size);
#endif

#if !ONLY_WHAT_IF
      // Read the entire bcwav into memory
      void *cwav = malloc(cwav_header->file_size);
      fread(cwav, cwav_header->file_size, 1, bcsar_file);

      // Generate the output file name
      char *outfile_name = calloc(strlen(argv[1]) + 20, 1);
      sprintf(outfile_name, "%s_%08d.bcwav", argv[1], file_count);

      // Open the output file
      FILE *cwav_out = fopen(outfile_name, "wb");
      fwrite(cwav, cwav_header->file_size, 1, cwav_out);

      fclose(cwav_out);
      free(cwav);
      free(outfile_name);
#else
      // Simulate a read operation
      fseek(bcsar_file, cwav_header->file_size, SEEK_CUR);
#endif

      free(cwav_header);

#if DEBUG
      if (++file_count > 10)
        return 0;
#endif

      ++file_count;
      fread(buffer, 4, 1, bcsar_file);
    } while ((next_char = getc(bcsar_file)) != EOF);

    fclose(bcsar_file);
    free(buffer);
    free(header);
  }
  return 0;
}
