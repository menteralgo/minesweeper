#include "minesweeper.h"
#include "view.h"

#define CELL_HINT_MASK 0xF

#define CELL_TYPE_HINT 0
#define CELL_TYPE_HIDDEN 1
#define CELL_TYPE_FLAG 2
#define CELL_TYPE_QUESTION 3
#define CELL_TYPE_BOMB 4
#define CELL_TYPE_CROSS 5

#define _encodeType(value) ((value) << 4)
#define _decodeType(value) ((value) >> 4)

static uint8_t mn_surface[FIELD_MAX_WIDTH][FIELD_MAX_HEIGHT];
static int mn_surface_width, mn_surface_height;
static int mn_bombs_count;

static char mn_is_first_click;

static mn_bombed_callback mnOnBombed;
static mn_winner_callback mnOnWinner;

static void mnDrawCell(int sx, int sy);
static void mnOnBomb();
static void mnShowBombs();

static void mnOnMousePress(int button, int x, int y);
static void mnOnMouseRelease(int button, int x, int y);

void mnInitSurface(int width, int height, int bombs, mn_bombed_callback bombed, mn_winner_callback winner)
{
    mn_surface_width = width;
    mn_surface_height = height;
    mn_bombs_count = bombs;

    mnOnBombed = bombed;
    mnOnWinner = winner;

    mn_is_first_click = 1;

    vSetMousePressCallback(mnOnMousePress);
    vSetMouseReleaseCallback(mnOnMouseRelease);

    for (int x = 0; x < mn_surface_width; ++x)
    {
        for (int y = 0; y < mn_surface_height; ++y)
        {
            mn_surface[x][y] = _encodeType(CELL_TYPE_HIDDEN);
        }
    }
}

void mnDrawSurface()
{
    for (int x = 0; x < mn_surface_width; ++x)
    {
        for (int y = 0; y < mn_surface_height; ++y)
        {
            mnDrawCell(x, y);
        }
    }
}

void mnSurfaceStop()
{
    vSetMousePressCallback(NULL);
    vSetMouseReleaseCallback(NULL);
}

static void mnDrawCell(int sx, int sy)
{
    uint8_t cell = mn_surface[sx][sy];
    int x = sx * VCELL_SIZE;
    int y = sy * VCELL_SIZE;

    switch (_decodeType(cell))
    {
    case CELL_TYPE_HIDDEN:
        vDrawHiddenCell(x, y);
        break;

    case CELL_TYPE_HINT:
        vDrawHint(x, y, cell & CELL_HINT_MASK);
        break;

    case CELL_TYPE_BOMB:
        vDrawBomb(x, y);
        break;

    case CELL_TYPE_FLAG:
        vDrawFlag(x, y);
        break;

    case CELL_TYPE_QUESTION:
        vDrawQuestion(x, y);
        break;

    case CELL_TYPE_CROSS:
        vDrawCoss(x, y);
        break;
    }
}

static void mnOpenHiddenRecursive(int sx, int sy)
{
    uint8_t cell;
    if (mnGetCell(sx, sy, &cell) < 0 || _decodeType(mn_surface[sx][sy]) != CELL_TYPE_HIDDEN)
        return;

    if (cell == 0)
    {
        mn_surface[sx][sy] = _encodeType(CELL_TYPE_HINT);

        mnOpenHiddenRecursive(sx + 1, sy);
        mnOpenHiddenRecursive(sx - 1, sy);
        mnOpenHiddenRecursive(sx, sy + 1);
        mnOpenHiddenRecursive(sx, sy - 1);
    }
    else if (cell == BOMB_CODE)
    {
        mnOnBomb();
    }
    else
    {
        mn_surface[sx][sy] = _encodeType(CELL_TYPE_HINT) + cell;
    }
}

static int mnIsWinner()
{
    int cells_left = 0;
    for (int x = 0; x < FIELD_MAX_WIDTH; ++x)
    {
        for (int y = 0; y < FIELD_MAX_HEIGHT; ++y)
        {
            uint8_t cell = mn_surface[x][y];
            if (_decodeType(cell) != CELL_TYPE_HINT)
                ++cells_left;
        }
    }

    return cells_left == mn_bombs_count;
}

static void mnOpenCell(int sx, int sy)
{
    uint8_t cell = mn_surface[sx][sy];

    switch (_decodeType(cell))
    {
    case CELL_TYPE_HIDDEN:
        mnOpenHiddenRecursive(sx, sy);
        break;
    case CELL_TYPE_BOMB:
        break;
    }
}

static void mnOnMousePress(int button, int x, int y)
{
    int sx = x / VCELL_SIZE;
    int sy = y / VCELL_SIZE;

    x = sx * VCELL_SIZE;
    y = sy * VCELL_SIZE;

    if (button == 1 && _decodeType(mn_surface[sx][sy]) == CELL_TYPE_HIDDEN)
        vDrawPressedCell(x, y);
}

static void mnOnMouseRelease(int button, int x, int y)
{
    int sx = x / VCELL_SIZE;
    int sy = y / VCELL_SIZE;

    if (mn_is_first_click)
    {
        mnInitField(sx, sy, mn_surface_width, mn_surface_height, mn_bombs_count);
        mn_is_first_click = 0;
    }

    if (button == 1)
    {
        mnOpenCell(sx, sy);
        if (mnIsWinner() && mnOnWinner != NULL)
            mnOnWinner();
    }
    else if (button == 3)
    {
        uint8_t cell_type = _decodeType(mn_surface[sx][sy]);
        switch (cell_type)
        {
        case CELL_TYPE_HIDDEN:
            mn_surface[sx][sy] = _encodeType(CELL_TYPE_FLAG);
            break;

        case CELL_TYPE_FLAG:
            mn_surface[sx][sy] = _encodeType(CELL_TYPE_QUESTION);
            break;

        case CELL_TYPE_QUESTION:
            mn_surface[sx][sy] = _encodeType(CELL_TYPE_HIDDEN);
            break;
        }
    }

    mnDrawSurface();
}

static void mnOnBomb()
{
    mnShowBombs();    

    if (mnOnBombed != NULL)
        mnOnBombed();
}

static void mnShowBombs()
{
    for (int x = 0; x < mn_surface_width; ++x)
    {
        for (int y = 0; y < mn_surface_height; ++y)
        {
            uint8_t cell;
            mnGetCell(x, y, &cell);
            int ct = _decodeType(mn_surface[x][y]);

            if (cell == BOMB_CODE)
            {
                if (ct != CELL_TYPE_FLAG)
                    mn_surface[x][y] = _encodeType(CELL_TYPE_BOMB);
            }
            else if (ct == CELL_TYPE_FLAG)
            {
                mn_surface[x][y] = _encodeType(CELL_TYPE_CROSS);
            }
        }
    }
}