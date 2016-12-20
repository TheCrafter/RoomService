#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

#include "window.h"
#include "ui.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

struct layout_data
{
    const char* button_label;
};

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

void layout_render(struct nk_context* ctx, void* layout_data)
{
    struct layout_data* data;
    data = (struct layout_data*)layout_data;

    struct nk_panel layout;
    if (nk_begin(ctx, &layout, "Demo", nk_rect(50, 50, 230, 250),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;
        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, data->button_label))
            fprintf(stdout, "button pressed\n");

        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

        /* Background control widget */
        {
            /* struct nk_panel combo; */
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            /*
            if (nk_combo_begin_color(ctx, &combo, background, nk_vec2(nk_widget_width(ctx),400)))
            {
                nk_layout_row_dynamic(ctx, 120, 1);
                background = nk_color_picker(ctx, background, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                background.r = (nk_byte)nk_propertyi(ctx, "#R:", 0, background.r, 255, 1,1);
                background.g = (nk_byte)nk_propertyi(ctx, "#G:", 0, background.g, 255, 1,1);
                background.b = (nk_byte)nk_propertyi(ctx, "#B:", 0, background.b, 255, 1,1);
                background.a = (nk_byte)nk_propertyi(ctx, "#A:", 0, background.a, 255, 1,1);
                nk_combo_end(ctx);
            }
            */
        }
    }
    nk_end(ctx);

}

void render(struct window* window, struct nk_context* ctx)
{
    int width, height;
    window_get_size(window, &width, &height);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.28f, 0.48f, 0.62f, 1.0f);
    glViewport(0, 0, width, height);

    /* Create data for ui layout */
    struct layout_data data;
    data.button_label = "Button1";

    ui_render(ctx, layout_render, &data);
}

int main()
{
    /* Create window */
    struct window* window = window_create(WINDOW_WIDTH, WINDOW_HEIGHT, "RoomService");
    if (window == NULL)
        return -1;

    /* Set callbacks */
    window_set_error_callback(window, error_callback);
    window_set_key_callback(window, key_callback);

    /* Platform */
    int width, height;
    window_get_size(window, &width, &height);

    /* OpenGL */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    /* Nuklear */
    struct nk_context* ctx;
    ctx = ui_init(window);

    /* Main Loop */
    while (!window_should_close(window))
    {
        /* Input */
        window_update(window);

        /* Render */
        render(window, ctx);

        /* Swap buffers */
        window_swap_buffers(window);
    }

    /* Shutdown */
    ui_destroy();
    glfwTerminate();
    return 0;
}
