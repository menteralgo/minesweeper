#pragma once

#include "image.h"

#define VCELL_SIZE 32
#define IMAGE_COUNT 15

#define IMAGE_HIDDEN 0
#define IMAGE_CLICKED 1
#define IMAGE_FLAG 2
#define IMAGE_QUESTION 3
#define IMAGE_BOMB 4
#define IMAGE_CROSS 5
#define IMAGE_EMPTY 6
#define IMAGE_1 7
#define IMAGE_2 8
#define IMAGE_3 9
#define IMAGE_4 10
#define IMAGE_5 11
#define IMAGE_6 12
#define IMAGE_7 13
#define IMAGE_8 14


typedef void (*v_update_callback)();
typedef void (*v_mouse_callback)(int button, int x, int y);

struct VTextureRect 
{
    int id;
    int x;
    int y;
    int width;
    int height;
};


void vInitView(struct PPMImage *image, struct VTextureRect texcrds[], int cell_width, int cell_height);

void vUpdate();

void vRefresh();

void vDispose();


void vSetOnUpdateCallback(v_update_callback callback);

void vSetMousePressCallback(v_mouse_callback callback);

void vSetMouseReleaseCallback(v_mouse_callback callback);


void vDrawHiddenCell(int x, int y);

void vDrawPressedCell(int x, int y);

void vDrawHint(int x, int y, int hint);

void vDrawBomb(int x, int y);

void vDrawCoss(int x, int y);

void vDrawFlag(int x, int y);

void vDrawQuestion(int x, int y);

void vWinMessage();

void vLoseMessage();