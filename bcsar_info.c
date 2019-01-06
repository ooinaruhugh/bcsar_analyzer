#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "bcsar.h"

STRG_header *read_strg(BCSAR_header *header, FILE *bcsar)
{
  STRG_header *strg = calloc(1, header->strg_length);
  fseek(bcsar, header->strg_loc, SEEK_SET);
  fread(strg, header->strg_length, 1, bcsar);
  return strg;
}

void *read_partition(FILE *file, uint64_t offset, uint64_t size)
{
  void *partition = calloc(1, size);
  fseek(file, offset, SEEK_SET);
  fread(partition, size, 1, file);
  return partition;
}

void print_strg(STRG_header *strg)
{
}

void print_string_table(STRG_header *strg, char *file, char delimiter)
{
  FILE *output;
  char *outfile;
  if (file)
  {
    outfile = calloc(strlen(file) + 10, 1);
    sprintf(outfile, "%s.list.csv", file);

    output = fopen(outfile, "a");

    fprintf(output, "sep=,\nIndex,Name\n");
  }
  else
  {
    output = stdout;
    fprintf(output, "Filename string table\n");
  }

  for (int i = 0; i < strg->filename_count; i++)
  {
    char *filename = (uintptr_t)strg + STRG_HEADER_END + strg->offset_table[i].data_offset;
    fprintf(output, "%8i%c%s\n", i, delimiter, filename);
    fflush(output);
  }

  if (file)
  {
    fclose(output);
    free(outfile);
  }
}

void print_lookup_table(STRG_Lookup_table *strg_lookup, char *file, char delimiter)
{
  char *lookup;
  FILE *output;
  if (file)
  {
    lookup = calloc(strlen(file) + 12, 1);
    sprintf(lookup, "%s.lookup.csv", file);

    output = fopen(lookup, "a");

    fprintf(output, "sep=,\nhas_data,bittest_condition,fail_condition_leaf,success_condition_leaf,index,resource_id,resource_type\n");
  }
  else
  {
    output = stdout;
    fprintf(output, "String lookup table\n");
  }

  for (int i = 0; i < strg_lookup->count; i++)
  {
    Lookup_entry *entry = strg_lookup->entries + i;
    fprintf(output, "%8i%c%08x%c%08x%c%08x%c%08x%c%08x%c%08x%c%08x\n", i, delimiter, entry->has_data, delimiter, entry->bit_test, delimiter, entry->fail_leaf_index, delimiter, entry->success_leaf_index, delimiter, entry->lookup_index, delimiter, entry->res_id.id, delimiter, entry->res_type);
    fflush(output);
  }

  if (file)
  {
    fclose(output);
    free(lookup);
  }
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

    STRG_Lookup_table *lookup_table = (uintptr_t)strg + strg->lookup_table + 0x8;
    printf("Index of the root entry: %" PRIu32 "\n", lookup_table->root_index);
    printf("Entry count: %" PRIu32 "\n", lookup_table->count);

    puts("\n== INFO partition info ==");
    Info_table *info = read_partition(bcsar_file, header->info_loc, header->info_length);
    printf("Length of partition: %x\n", info->length);

    // Prints file name table

    // print_string_table(strg, argv[1], ','); // To CSV
    // print_string_table(strg, NULL, ' '); // To stdout

    // Prints the raw string lookup table
    
    // print_lookup_table(lookup_table, argv[1], ','); // To CSV
    // print_lookup_table(lookup_table, NULL, ' '); // To stdout

    free(magic_buffer);
    free(strg);
    free(header);
    fclose(bcsar_file);
  }
  return 0;
}
