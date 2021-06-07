#pragma once

#include <stdlib.h>
#include <stdint.h>

#define FIELD_MAX_WIDTH 32
#define FIELD_MAX_HEIGHT 32

#define BOMB_CODE 0xFF

int mnInitField(int x, int y, int width, int height, int bombs);

int mnGetCell(int x, int y, uint8_t *cell);


int mainfunc();

void mnNewGame(int config);


typedef void (*mn_bombed_callback)();
typedef void (*mn_winner_callback)();

void mnInitSurface(int width, int height, int bombs, mn_bombed_callback bombed, mn_winner_callback winner);

void mnDrawSurface();

void mnSurfaceStop();