#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

struct image_data {
  int32_t width, height;
  size_t size;
  uint8_t data[];
};

bool write_image(const char *filename, char **error);
struct image_data *read_image(const char *filename, char **error);
