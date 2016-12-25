#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

#include "window.h"
#include "ui/ui.h"

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

void render(struct window* window, struct ui_context* ctx)
{
    int width, height;
    window_get_size(window, &width, &height);

    /* Draw background */
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(ctx->bg[0], ctx->bg[1], ctx->bg[2], ctx->bg[3]);
    glViewport(0, 0, width, height);

    /* Render UI */
    ui_render(ctx);
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

    /* UI */
    struct ui_context ctx;
    ui_init(&ctx, window, width, height);

    /* Initial background */
    float bg[] = {0.28f, 0.48f, 0.62f, 1.0f};
    ctx.bg = bg;

    /* Main Loop */
    while (!window_should_close(window))
    {
        /* Input */
        window_update(window);

        /* Render */
        render(window, &ctx);

        /* Swap buffers */
        window_swap_buffers(window);
    }

    /* Shutdown */
    ui_destroy(&ctx);
    window_destroy(window);

#ifdef USE_LEAK_DETECTOR
    ld_print_leaks();
    ld_shutdown();
#endif
    return 0;
}
