#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/mman.h>

#include "bmp.h"

// the pattern to be written
// TODO: add more patterns

//#include "patterns/hello.h"
#include "patterns/space_invader.h"

#define BITS_PER_PIXEL 32
#define BYTES_PER_PIXEL (BITS_PER_PIXEL / 8)
#define WIDTH_BYTES (WIDTH * BYTES_PER_PIXEL)

#define HEADER_ADDR 0x00
#define DIM_HEADER_ADDR 0x0E

// size calculations
#define HEADER_SIZE sizeof(struct header)
#define DIM_HEADER_SIZE sizeof(struct bitmap_info_header)
#define TOTAL_HEADER_SIZE (HEADER_SIZE + DIM_HEADER_SIZE)

#define ROW_SIZE WIDTH_BYTES // row size is aligned to 32 bits
#define DATA_SIZE (ROW_SIZE * HEIGHT)

#define TOTAL_SIZE (TOTAL_HEADER_SIZE + DATA_SIZE)

// structs are packed for the correct memory layout
// this means they can be directly memcpy-ed
// this does assume a little-endian cpu (required by the format)
struct __attribute__((__packed__)) header {
    char header[2]; // the letters 'BM'
    uint32_t size; // size of the file
    uint16_t reserved[2]; // does nothing
    uint32_t data; // the starting address of the actual image data
};

struct __attribute__((__packed__)) bitmap_info_header {
    uint32_t size; // will always be 40 (for this header format)
    int32_t width, height; // the width and height of the image
    uint16_t color_planes; // must be 1  ¯\_(ツ)_/¯
    uint16_t bits_per_pixel; // this program supports 32
    uint32_t compression_method; // this program doesn't support compression
    uint32_t image_size; // not needed since no compression is used
    int32_t horizontal_resolution, vertical_resolution; // in pixels per metre
    uint32_t colors_in_palette; // not supported
    uint32_t important_colors; // not supported
};

#define ERROR(...) int error_len = snprintf(NULL, 0, __VA_ARGS__); \
	*error = calloc(1, error_len + 1); \
	snprintf(*error, error_len + 1, __VA_ARGS__); \
	return 0;

bool write_image(const char *filename, char** error) {
    struct header header = {
        .header = { 'B', 'M' },
        .size = TOTAL_SIZE,
        .data = TOTAL_HEADER_SIZE, // data will start immediately after headers
    };

    struct bitmap_info_header bitmap_info_header = {
        .size = 40,
        .width = WIDTH,
        .height = -HEIGHT, // setting height to negative makes rows top-down
        .color_planes = 1,
        .bits_per_pixel = 32,
        .compression_method = 0, // no compression
        .image_size = 0,
        .horizontal_resolution = 2835, // aproximately equal to 72 ppi, doesn't make a difference in testing though
        .vertical_resolution = 2835,
        .colors_in_palette = 0,
        .important_colors = 0,
    };

    uint8_t *buffer = calloc(1, TOTAL_SIZE);
    memcpy(buffer + HEADER_ADDR, &header, sizeof(struct header)); // copy the header
    memcpy(buffer + DIM_HEADER_ADDR, &bitmap_info_header, sizeof(struct bitmap_info_header)); // copy the DIM header

    for (int i = 0; i < HEIGHT; i++) { // copy each row with the correct padding (rows need to be aligned to 32 bits)
        uint8_t *buffer_addr = buffer + TOTAL_HEADER_SIZE + ROW_SIZE * i;
        const uint32_t *row = data[i];

        memcpy(buffer_addr, row, WIDTH_BYTES);
    }

    // write the data
    FILE* file = fopen(filename, "w");
    fwrite(buffer, TOTAL_SIZE, 1, file);
    fclose(file);

    free(buffer);
	return true;
}

struct image_data *read_image(const char *filename, char **error) {
    int status;
    
    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        ERROR("at open: %s", strerror(errno));
    }

    struct stat stat;
    status = fstat(fd, &stat);

    if (status == -1) {
        ERROR("at fstat: %s\n", strerror(errno));
    }

    if (stat.st_size <= TOTAL_HEADER_SIZE) {
        ERROR("file too small");
    }
    
    void *addr = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if (addr == MAP_FAILED) {
        ERROR("at mmap: %s\n", strerror(errno));
    }

    status = close(fd);
    if (status == -1) {
        ERROR("at close: %s\n", strerror(errno));
    }
    
    struct header *header = addr + HEADER_ADDR;

    bool valid = header->header[0] == 'B'
        && header->header[1] == 'M'
        && header->size == stat.st_size;
    
    if (!valid) {
        ERROR("invalid data");
    }

    struct bitmap_info_header *dim_header = addr + DIM_HEADER_ADDR;

    if (dim_header->size != 40) {
        ERROR("unsupported header size: %d", dim_header->size);
    }

    if (dim_header->bits_per_pixel != 32) {
        ERROR("unsupported bits per pixel: %d", dim_header->bits_per_pixel);
    }

    if (dim_header->compression_method != 0) {
        ERROR("unsupported compression method: %d", dim_header->compression_method);
    }

    uint8_t *data_addr = addr + header->data;
    int32_t width = dim_header->width, height = dim_header->height;

    struct image_data *image_data = calloc(sizeof(struct image_data) + width * abs(height), 1);
	image_data->width = width;
	image_data->height = height;

    // reorder data from ARGB to RGBA
    if (height < 0) {
        for (int i = 0; i < height * -1; i++) {
            uint8_t *row_addr = data_addr + width * i * 4;
            for(int j = 0; j < width; j++) {
                size_t index = j * 4;
                uint8_t alpha = row_addr[index];
                uint8_t red = row_addr[index + 1];
                uint8_t green = row_addr[index + 2];
                uint8_t blue = row_addr[index + 3];

                image_data->data[index] = red;
                image_data->data[index + 1] = green;
                image_data->data[index + 2] = blue;
                image_data->data[index + 3] = alpha;
            }
        }
    }

    status = munmap(addr, stat.st_size);
    if (status == -1) {
        ERROR("at munmap: %s\n", strerror(errno));
    }

    return image_data;
}
