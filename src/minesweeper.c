#include <time.h>
#include "minesweeper.h"

#define RESERVE_CODE 0x80

#define _xyInside(x, y) ((x) >= 0 && (x) < mn_field_width && (y) >= 0 && (y) < mn_field_height)
#define _isBomb(x, y) (mn_field[x][y] == BOMB_CODE)

static uint8_t mn_field[FIELD_MAX_WIDTH][FIELD_MAX_HEIGHT];
static uint8_t *mn_picker[FIELD_MAX_WIDTH * FIELD_MAX_HEIGHT];

static int mn_field_width, mn_field_height;
static int mn_bomb_count;

static void mnPlaceBombs();

int mnInitField(int x, int y, int width, int height, int bombs)
{
    time_t t;
    srand((unsigned)time(&t));

    if (width > FIELD_MAX_WIDTH || height > FIELD_MAX_HEIGHT || bombs >= FIELD_MAX_WIDTH * FIELD_MAX_HEIGHT)
        return 0;

    mn_field_width = width;
    mn_field_height = height;
    mn_bomb_count = bombs;

    for (int i = 0; i < width; ++i)
        for (int j = 0; j < height; ++j)
            mn_field[i][j] = 0;

    // Reserve empty space
    mn_field[x][y] = RESERVE_CODE;

    mnPlaceBombs();
    mn_field[x][y] &= ~RESERVE_CODE;

    return 0;
}

int mnGetCell(int x, int y, uint8_t *cell)
{
    if (cell == NULL || !_xyInside(x, y))
        return -1;

    *cell = mn_field[x][y];
    return 0;
}

static void mnShiftItems(int index, int size)
{
    for (int i = index + 1; i < size; ++i)
        mn_picker[i - 1] = mn_picker[i];
}

static void mnSetHints(int x, int y)
{
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            int _x = x + i;
            int _y = y + j;

            if (_xyInside(_x, _y) && !_isBomb(_x, _y))
                ++mn_field[_x][_y];
        }
    }
}

static void mnPlaceBombs()
{
    int size = mn_field_width * mn_field_height;
    int bombs = mn_bomb_count;

    int i = 0;
    for (int x = 0; x < mn_field_width; ++x)
    {
        for (int y = 0; y < mn_field_height; ++y)
        {
            if (mn_field[x][y] == RESERVE_CODE)
            {
                --size;
                continue;
            }

            mn_picker[i++] = &mn_field[x][y];
        }
    }

    // Place bombs
    while (bombs > 0 && size > 0)
    {
        int index = rand() % size;
        *mn_picker[index] = BOMB_CODE;

        mnShiftItems(index, size);

        --size;
        --bombs;
    }

    // Place hints
    for (int x = 0; x < mn_field_width; ++x)
    {
        for (int y = 0; y < mn_field_height; ++y)
        {
            if (mn_field[x][y] == BOMB_CODE)
                mnSetHints(x, y);
        }
    }
}
