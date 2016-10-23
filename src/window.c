#include "window.h"
#include <stdlib.h>

/* -------------------------------------------------- */
/* Helper Functions declarations                      */
/* -------------------------------------------------- */
void configure_glfw();
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/* Implement window data struct */
struct window
{
    struct GLFWwindow* window_handle;        
    window_error_cb error_cb;
    window_key_cb key_cb;
};

struct window* window_create(int width, int height, const char* title)
{
    /* Allocate space for window struct */
    struct window* window = malloc(sizeof(struct window));

    /* Initialize GLFW */
    if (!glfwInit())
    {
        free(window);
        return NULL;
    }

    /* Configure GLFW */
    configure_glfw();

    /* Create context */
    window->window_handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        free(window);
        return NULL;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window->window_handle);

    /* Set window user pointer */
    glfwSetWindowUserPointer(window->window_handle, window);

    /* Set callbacks */
    glfwSetKeyCallback(window->window_handle, key_callback);

    /* Load OpenGL extensions */
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    /* Return window data */
    return window;
}

void window_destroy(struct window* window)
{
    /* Destroy window */
    glfwDestroyWindow(window->window_handle);

    /* Deinitialize GLFW */
    glfwTerminate();

    /* Deallocate space used for window struct */
    free(window);
}

void window_update(struct window* window)
{
    (void)window;

    /* Poll events */
    glfwPollEvents();
}

void window_swap_buffers(struct window* window)
{
    glfwSwapBuffers(window->window_handle);
}

void window_close(struct window* window)
{
    glfwSetWindowShouldClose(window->window_handle, GLFW_TRUE);
}

bool window_should_close(struct window* window)
{
    return glfwWindowShouldClose(window->window_handle);
}

void window_get_size(struct window* window, int* width, int* height)
{
    glfwGetWindowSize(window->window_handle, width, height);
}

struct GLFWwindow* window_get_glfw_handle(struct window* window)
{
    return window->window_handle;
}

void window_set_error_callback(struct window* window, window_error_cb error_cb)
{
    glfwSetErrorCallback(error_cb);
    window->error_cb = error_cb;
}

void window_set_key_callback(struct window* window, window_key_cb key_cb)
{
    window->key_cb = key_cb;    
}

/* -------------------------------------------------- */
/* Helper function implementation                     */
/* -------------------------------------------------- */
void configure_glfw()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
}

void key_callback(GLFWwindow* window_handle, int key, int scancode, int action, int mods)
{
    struct window* window = glfwGetWindowUserPointer(window_handle);

    int act = -1;
    switch (action)
    {
        case GLFW_PRESS:
            act = KEY_ACTION_PRESS;
            break;
        case GLFW_RELEASE:
            act = KEY_ACTION_RELEASE;
            break;
        case GLFW_REPEAT:
            act = KEY_ACTION_REPEAT;
            break;
    }

    if (window->key_cb)
        window->key_cb(window, key, scancode, act, mods);
}
