#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include "window.h"

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

    /* Main loop */
    while (!window_should_close(window))
    {
        /* Update */
        window_update(window);

        /* Render */
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap buffers */
        window_swap_buffers(window);
    }

    /* Shutdown window */
    window_destroy(window);
    return 0;
}
