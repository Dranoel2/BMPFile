#pragma once

#include <inttypes.h>

// in the format AARRGGBB
#define TRANS 0x00000000 // transparent
#define BLACK 0xff000000
#define WHITE 0xffffffff
#define RED 0xffff0000
#define ORANGE 0xffffa500
#define YELLOW 0xfffff000
#define GREEN 0xff00ff00
#define BLUE 0xff0000ff
#define PURPLE 0xffa500ff

#define DEFINE_PATTERN const uint32_t data[HEIGHT][WIDTH] =