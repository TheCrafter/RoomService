#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

#include <log.h>
#include <vector.h>

#include "window.h"
#include "ui.h"
#include "string_utils.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

struct layout_data
{
    const char* button_label;
    struct vector output_vec;
    float* bg;
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

    struct nk_color background = nk_rgb_fv(data->bg);

    struct nk_panel output_layout;
    if (nk_begin(ctx, &output_layout, "Output", nk_rect(500, 200, 250, 250),
                NK_WINDOW_TITLE|NK_WINDOW_SCALABLE))
    {
        nk_layout_row_dynamic(ctx, 10, 1);
        for (size_t i = 0; i < data->output_vec.size; ++i)
        {
            struct cstring* str = vector_at(&data->output_vec, i);
            nk_label(ctx, str->data, NK_TEXT_LEFT); 
        }
    }
    nk_end(ctx);

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
        {
            struct cstring* cstr = (struct cstring*)malloc(sizeof(struct cstring));
            cstring_init(cstr, "Button pressed");
            vector_append(&data->output_vec, cstr);
        }

        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

        /* Background control widget */
        {
            struct nk_panel combo;
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
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
        }
    }
    nk_end(ctx);

    /* Save background color */
    nk_color_fv(data->bg, background);
}

void render(struct window* window, struct nk_context* ctx, struct layout_data* data)
{
    int width, height;
    window_get_size(window, &width, &height);

    /* Draw background */
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(data->bg[0], data->bg[1], data->bg[2], data->bg[3]);
    glViewport(0, 0, width, height);

    /* Render UI */
    ui_render(ctx, layout_render, data);
}

int main()
{
#ifdef USE_LEAK_DETECTOR
    /* Init leak detector */
    ld_init();
#endif

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

    /* Initial background */
    float bg[] = {0.28f, 0.48f, 0.62f, 1.0f};

    /* Create data for ui layout */
    struct layout_data data;
    data.button_label = "Button";
    data.bg = bg;
    vector_init(&data.output_vec, sizeof(struct cstr*));

    /* Main Loop */
    while (!window_should_close(window))
    {
        /* Input */
        window_update(window);

        /* Render */
        render(window, ctx, &data);

        /* Swap buffers */
        window_swap_buffers(window);
    }

    /* Shutdown */
    for (size_t i = 0; i < data.output_vec.size; ++i)
        cstring_destroy(vector_at(&data.output_vec, i));
    vector_destroy(&data.output_vec);

    ui_destroy();
    glfwTerminate();

#ifdef USE_LEAK_DETECTOR
    ld_print_leaks();
    ld_shutdown();
#endif
    return 0;
}
