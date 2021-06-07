#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include "../view.h"
#include "../image.h"

static xcb_connection_t *connection;
static xcb_window_t window;

static xcb_gcontext_t gc_pix;
static xcb_image_t *textures[IMAGE_COUNT];

static v_update_callback onUpdate;
static v_mouse_callback onMousePress;
static v_mouse_callback onMouseRelease;

static void unpackImage(struct PPMImage *atlas, struct VTextureRect texcrd);

void vInitView(struct PPMImage *atlas, struct VTextureRect texcrds[], int cell_width, int cell_height)
{
    int screen_num;
    connection = xcb_connect(NULL, &screen_num);

    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(connection));
    for (int i = 0; i < screen_num; ++i)
        xcb_screen_next(&iter);

    xcb_screen_t *screen = iter.data;
    window = xcb_generate_id(connection);

    int background_color = 0x707070;
    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t values[2];
    values[0] = background_color;
    values[1] = XCB_EVENT_MASK_EXPOSURE |
                XCB_EVENT_MASK_BUTTON_PRESS |
                XCB_EVENT_MASK_BUTTON_RELEASE;

    xcb_create_window(connection,
                      screen->root_depth,
                      window,
                      screen->root,
                      20, 200,
                      cell_width * VCELL_SIZE, cell_height * VCELL_SIZE,
                      0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      mask, values);

    xcb_map_window(connection, window);
    xcb_flush(connection);

    gc_pix = xcb_generate_id(connection);
    xcb_create_gc(connection, gc_pix, window, 0, NULL);

    for (int i = 0; i < IMAGE_COUNT; ++i)
        unpackImage(atlas, texcrds[i]);

    free(atlas->buffer);
}

void vRefresh()
{
    xcb_flush(connection);
}

void vUpdate()
{
    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(connection)))
    {
        switch (event->response_type & ~0x80)
        {
            case XCB_EXPOSE:
            {
                if (onUpdate != NULL)
                    onUpdate();

                xcb_flush(connection);
            }
            break;
            case XCB_BUTTON_PRESS:
            {
                xcb_button_press_event_t *bpe = (xcb_button_press_event_t *)event;
                if (onMousePress != NULL)
                    onMousePress(bpe->detail, bpe->event_x, bpe->event_y);

                xcb_flush(connection);
            }
            break;
            case XCB_BUTTON_RELEASE:
            {
                xcb_button_release_event_t *bre = (xcb_button_release_event_t *)event;
                if (onMouseRelease != NULL)
                    onMouseRelease(bre->detail, bre->event_x, bre->event_y);

                xcb_flush(connection);
            }
            break;
        }

        free(event);
    }
}

void vDispose()
{
    xcb_disconnect(connection);
}

void vDrawHiddenCell(int x, int y)
{
    xcb_image_put(connection, window, gc_pix, textures[IMAGE_HIDDEN], x, y, 0);
}

void vDrawPressedCell(int x, int y)
{
    xcb_image_put(connection, window, gc_pix, textures[IMAGE_CLICKED], x, y, 0);
}

void vDrawHint(int x, int y, int hint)
{
    xcb_image_put(connection, window, gc_pix, textures[IMAGE_EMPTY + hint], x, y, 0);
}

void vDrawBomb(int x, int y)
{
    xcb_image_put(connection, window, gc_pix, textures[IMAGE_BOMB], x, y, 0);
}

void vDrawCoss(int x, int y)
{
    xcb_image_put(connection, window, gc_pix, textures[IMAGE_CROSS], x, y, 0);
}

void vDrawFlag(int x, int y)
{
    xcb_image_put(connection, window, gc_pix, textures[IMAGE_FLAG], x, y, 0);
}

void vDrawQuestion(int x, int y)
{
    xcb_image_put(connection, window, gc_pix, textures[IMAGE_QUESTION], x, y, 0);
}

void vSetOnUpdateCallback(v_update_callback callback)
{
    onUpdate = callback;
}

void vSetMousePressCallback(v_mouse_callback callback)
{
    onMousePress = callback;
}

void vSetMouseReleaseCallback(v_mouse_callback callback)
{
    onMouseRelease = callback;
}

void vWinMessage()
{
    printf("You Win!\n");
}

void vLoseMessage()
{
    printf("Game Over!\n");
}

static void unpackImage(struct PPMImage *atlas, struct VTextureRect texcrd)
{
    xcb_image_t *ximg = xcb_image_create_native(connection, texcrd.width, texcrd.height, XCB_IMAGE_FORMAT_XY_PIXMAP, 24, NULL, texcrd.width * texcrd.height * 3, NULL);

    for (int y = 0; y < texcrd.height; ++y)
    {
        for (int x = 0; x < texcrd.width; ++x)
        {
            int index = (y + texcrd.y) * atlas->height * 3 + (x + texcrd.x) * 3;

            uint8_t r = atlas->buffer[index++];
            uint8_t g = atlas->buffer[index++];
            uint8_t b = atlas->buffer[index];
            xcb_image_put_pixel(ximg, x, y, b + (g << 8) + (r << 16));
        }
    }

    textures[texcrd.id] = ximg;
}