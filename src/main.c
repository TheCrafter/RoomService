#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

#include <nuklear_config.h>
#include <nuklear.h>
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear_glfw_gl3.h"
#include "window.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

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

void render(struct window* window, struct nk_color* background)
{
    float bg[4];
    int width, height;

    nk_color_fv(bg, *background);
    window_get_size(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(bg[0], bg[1], bg[2], bg[3]);
    /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
     * with blending, scissor, face culling, depth test and viewport and
     * defaults everything back into a default state.
     * Make sure to either a.) save and restore or b.) reset your own state after
     * rendering the UI.
     */
    nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

int main()
{
    // Create window
    struct window* window = window_create(WINDOW_WIDTH, WINDOW_HEIGHT, "RoomService");
    if (window == NULL)
        return -1;

    /* Get GLFW handle */
    GLFWwindow* glfw_handle = window_get_glfw_handle(window);

    /* Set callbacks */
    window_set_error_callback(window, error_callback);
    window_set_key_callback(window, key_callback);

    /* Platform */
    int width = 0, height = 0;
    struct nk_context *ctx;

    /* GLFW */
    window_get_size(window, &width, &height);

    /* OpenGL */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    /* Init nuklear with GLFW */
    ctx = nk_glfw3_init(glfw_handle, NK_GLFW3_INSTALL_CALLBACKS);

    /* Load Fonts: if none of these are loaded a default font will be used  */
    {
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&atlas);

        struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "res/Font/Roboto-Regular.ttf", 16, 0);
        nk_glfw3_font_stash_end();

        /* Load Cursor: if you uncomment cursor loading please hide the cursor */
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/

        nk_style_set_font(ctx, &roboto->handle);
    }

    /* style.c */
    /*set_style(ctx, THEME_WHITE);*/
    /*set_style(ctx, THEME_RED);*/
    /*set_style(ctx, THEME_BLUE);*/
    /*set_style(ctx, THEME_DARK);*/

    struct nk_color background = nk_rgb(28,48,62);
    while (!window_should_close(window))
    {
        /* Input */
        window_update(window);
        nk_glfw3_new_frame();

        /* GUI */
        {
            struct nk_panel layout;
            if (nk_begin(ctx, &layout, "Demo", nk_rect(50, 50, 230, 250),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
            {
                enum {EASY, HARD};
                static int op = EASY;
                static int property = 20;
                nk_layout_row_static(ctx, 30, 80, 1);
                if (nk_button_label(ctx, "button"))
                    fprintf(stdout, "button pressed\n");

                nk_layout_row_dynamic(ctx, 30, 2);
                if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
                if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

                {struct nk_panel combo;
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, "background:", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_combo_begin_color(ctx, &combo, background, nk_vec2(nk_widget_width(ctx),400))) {
                    nk_layout_row_dynamic(ctx, 120, 1);
                    background = nk_color_picker(ctx, background, NK_RGBA);
                    nk_layout_row_dynamic(ctx, 25, 1);
                    background.r = (nk_byte)nk_propertyi(ctx, "#R:", 0, background.r, 255, 1,1);
                    background.g = (nk_byte)nk_propertyi(ctx, "#G:", 0, background.g, 255, 1,1);
                    background.b = (nk_byte)nk_propertyi(ctx, "#B:", 0, background.b, 255, 1,1);
                    background.a = (nk_byte)nk_propertyi(ctx, "#A:", 0, background.a, 255, 1,1);
                    nk_combo_end(ctx);
                }}
            }
            nk_end(ctx);
        }

        /* Render */
        render(window, &background);

        /* Swap buffers */
        window_swap_buffers(window);
    }
    nk_glfw3_shutdown();
    glfwTerminate();

    return 0;
}
