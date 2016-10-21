#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include "window.h"

#include "nuklear_config.h"
#include <nuklear.h>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %d %s\n", error, description);
    exit(1);
}

void key_callback(struct window* window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    if (key == KEY_ESCAPE && action == KEY_ACTION_PRESS)
        window_close(window);
}

int main()
{
    struct window* window = window_create(640, 480, "RoomService");

    if (window == NULL)
        return -1;

    window_set_error_callback(window, error_callback);
    window_set_key_callback(window, key_callback);

    /* Init GUI State nk */
    struct nk_context context;

    /* Load Font */
    struct nk_font_atlas atlas;
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
    struct nk_font* roboto = nk_font_atlas_add_from_file(&atlas, "", 14, 0);
    nk_style_set_font(&context, &roboto->handle);
    //context.style.font->width = 0;

    nk_init_default(&context, 0);

    enum {EASY, HARD};
    int op = EASY;
    float value = 0.6f;

    /* Main loop */
    while (!window_should_close(window))
    {
        /* Update */
        window_update(window);

        /* Render */
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        struct nk_panel layout;
        if (nk_begin(&context, &layout, "Show", nk_rect(50, 50, 220, 220),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
            /* fixed widget pixel width */
            nk_layout_row_static(&context, 30, 80, 1);
            if (nk_button_label(&context, "button")) {
                /* event handling */
            }
        
            /* fixed widget window ratio width */
            nk_layout_row_dynamic(&context, 30, 2);
            if (nk_option_label(&context, "easy", op == EASY)) op = EASY;
            if (nk_option_label(&context, "hard", op == HARD)) op = HARD;
        
            /* custom widget pixel width */
            nk_layout_row_begin(&context, NK_STATIC, 30, 2);
            {
                nk_layout_row_push(&context, 50);
                nk_label(&context, "Volume:", NK_TEXT_LEFT);
                nk_layout_row_push(&context, 110);
                nk_slider_float(&context, 0, &value, 1.0f, 0.1f);
            }
            nk_layout_row_end(&context);
        }
        nk_end(&context);

        /* Swap buffers */
        window_swap_buffers(window);
    }

    /* Shutdown window */
    window_destroy(window);
    return 0;
}
