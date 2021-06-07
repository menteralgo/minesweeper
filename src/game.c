#include "minesweeper.h"
#include "view.h"
#include "image.h"

#include <stdio.h>

static void mnOnUpdate();
static void mnOnWinner();
static void mnOnBombed();

int mainfunc()
{
    struct PPMImage atlas;
    mnReadImageP6("Atlas.ppm", &atlas);
    mnInitSurface(16, 16, 40, mnOnBombed, mnOnWinner);

    struct VTextureRect texpack[] = {
                                {IMAGE_HIDDEN, 0, 0, 32, 32},
                                {IMAGE_CLICKED, 32, 0, 32, 32},
                                {IMAGE_EMPTY, 64, 0, 32, 32},
                                {IMAGE_FLAG, 96, 0, 32, 32},
                                {IMAGE_QUESTION, 0, 32, 32, 32},
                                {IMAGE_BOMB, 32, 32, 32, 32},
                                {IMAGE_CROSS, 64, 32, 32, 32},
                                {IMAGE_1, 96, 32, 32, 32},
                                {IMAGE_2, 0, 64, 32, 32},
                                {IMAGE_3, 32, 64, 32, 32},
                                {IMAGE_4, 64, 64, 32, 32},
                                {IMAGE_5, 96, 64, 32, 32},
                                {IMAGE_6, 0, 96, 32, 32},
                                {IMAGE_7, 32, 96, 32, 32},
                                {IMAGE_8, 64, 96, 32, 32}
                             };
    vInitView(&atlas, texpack, 16, 16);
    vSetOnUpdateCallback(mnOnUpdate);

    vUpdate();
    vDispose();

    return 0;
}

static void mnOnWinner()
{
    mnSurfaceStop();
    vWinMessage();
}

static void mnOnBombed()
{
    mnSurfaceStop();
    vLoseMessage();
}

static void mnOnUpdate()
{
    mnDrawSurface();
}