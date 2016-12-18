#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

#include "window.h"
#include "ui.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

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

void render(struct window* window, struct nk_context* ctx)
{
    int width, height;
    window_get_size(window, &width, &height);

    glViewport(0, 0, width, height);
    ui_render(ctx);
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
