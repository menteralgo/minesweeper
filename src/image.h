#pragma once

#include <stdint.h>

#define PPM_HEADER_P6 6

struct PPMImage
{
    int header;
    int width;
    int height;
    int maxval;
    int size;
    uint8_t *buffer;
};

int mnReadImageP6(char *path, struct PPMImage *image);