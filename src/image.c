#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "image.h"

static void getvalue(FILE *file, char *buff);
static void skipline(FILE *file);

int mnReadImageP6(char *path, struct PPMImage *image)
{
    if (image == NULL)
    {
        fprintf(stderr, "PPMImage is null\n");
        return -1;
    }

    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        char *error = strerror(errno);
        fprintf(stderr, "File '%s' open error %d: %s\n", path, errno, error);

        return -1;
    }

    int c;
    char str[10];

    // Get header
    getvalue(file, str);
    if (strcmp("P6", str) == 0)
    {
        image->header = PPM_HEADER_P6;
    }
    else 
    {
        fprintf(stderr, "Unknown ppm format: '%s'\n", str);
        return -1;
    }

    // Skip comment line
    while(isspace(fgetc(file)))
        ;
    while ((c = fgetc(file)) != '\n' && c != '\r')
        ;

    // Get width
    getvalue(file, str);
    image->width = atoi(str);

    // Get height
    getvalue(file, str);
    image->height = atoi(str);

    // Get maxval
    getvalue(file, str);
    image->maxval = atoi(str);
    if (image->maxval != 255)
    {
        fprintf(stderr, "Unknown format. Max value = %d\n", image->maxval);
        return -1;
    }

    image->size = image->width * image->height * 3;

    while (isspace(c = fgetc(file)))
        ;
    ungetc(c, file);

    // Read buffer
    uint8_t *buffer = malloc(image->size);
    
    int i = 0;
    while ((c = fgetc(file)) != EOF)
        buffer[i++] = c;
    
    image->buffer = buffer;

    return 0;
}

static void getvalue(FILE *file, char *buff)
{
    int c;
    int i = 0;

    while (isspace((c = fgetc(file))))
        ;

    do
        buff[i++] = c;
    while(!isspace(c = fgetc(file)));

    buff[i] = '\0';
}