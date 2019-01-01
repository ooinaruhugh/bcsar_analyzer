#include <stdio.h>
#include <stdlib.h>
#include "bcsar.h"
#include "bcwav.h"

int main(int argc, char const *argv[])
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

    if (header->magic != CSAR_MAGIC)
    {
      fprintf(stderr, "%s doesn't seem to be a valid BCSAR file.\n", argv[1]);
      fclose(bcsar_file);
      free(header);
      return EXIT_FAILURE;
    }

    char *buffer = calloc(5, sizeof(*buffer));
    uint32_t *magic_buffer = buffer;
    fread(buffer, 4, 1, bcsar_file);
    char next_char;
    unsigned int file_count = 1;
    do
    {
      puts("Searching for BCWAV");
      while (*magic_buffer != CWAV_MAGIC)
      {
        buffer[0] = buffer[1];
        buffer[1] = buffer[2];
        buffer[2] = buffer[3];
        buffer[3] = next_char;
        fread(&next_char, 1,1,bcsar_file);
        /* if (next_char == EOF)
        // if ((next_char = getc(bcsar_file)) == EOF)
        {
          puts("EOF");
          return EXIT_SUCCESS;
        } */
      }
      fseek(bcsar_file, -5, SEEK_CUR);
      printf("Found BCWAV no.%i at Offset 0x%x\n", file_count, ftell(bcsar_file));

      puts("Retrieving BCWAV header");
      BCWAV_header *cwav_header = malloc(sizeof(*cwav_header));
      fread(cwav_header, sizeof(*cwav_header), 1, bcsar_file);
      fseek(bcsar_file, -sizeof(*cwav_header), SEEK_CUR);
      // printf("Now at %x\n", ftell(bcsar_file));

      char *magic = calloc(1, 5);
      *((uint32_t *)magic) = cwav_header->magic;

      printf("BCWAV of size: %i Magic: %s\n", cwav_header->file_size, magic);
      
      printf("Retrieving BCWAV file of size %i\n", cwav_header->file_size);
      void *cwav = malloc(cwav_header->file_size);
      fread(cwav, cwav_header->file_size, 1, bcsar_file);

      char *outfile_name = calloc(20, 1);
      sprintf(outfile_name, "%08d.bcwav", file_count);

      FILE *cwav_out = fopen(outfile_name, "wb");
      fwrite(cwav, cwav_header->file_size, 1, cwav_out);
      fclose(cwav_out);

      if(++file_count > 10)return 0;
      fread(buffer, 4, 1, bcsar_file);
    // } while ((next_char = getc(bcsar_file)) != EOF);
    } while (1);
  }
  return 0;
}
