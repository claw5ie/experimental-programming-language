#include <iostream>
#include <cassert>
#include <sys/stat.h>

#include "src/Parser.hpp"

char *read_file(const char *filepath)
{
  size_t const file_size =
    [filepath]() -> size_t
    {
      struct stat stats;

      if (stat(filepath, &stats) == -1)
      {
        std::cerr << "ERROR: failed to stat the file `" << filepath << "`.\n";
        exit(EXIT_FAILURE);
      }

      return stats.st_size;
    }();

  FILE *const file = std::fopen(filepath, "r");

  if (!file)
  {
    std::cerr << "ERROR: failed to open the file `" << filepath << "`.\n";
    exit(EXIT_FAILURE);
  }

  char *const file_data = new char[file_size + 1];
  fread(file_data, 1, file_size, file);
  file_data[file_size] = '\0';

  return file_data;
}

int main(int argc, char **argv)
{
  assert(argc == 2);

  const char *const file_data = read_file(argv[1]);

  Tokenizer tokenizer = { file_data, { }, 0, 0 };
  parse(tokenizer);

  delete[] file_data;

  return EXIT_SUCCESS;
}
