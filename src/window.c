#include "window.h"
#include <stdlib.h>

/* -------------------------------------------------- */
/* Helper Functions declarations                      */
/* -------------------------------------------------- */
void configure_glfw();
static enum key_action glfw_action_to_action(int glfw_action);
static void key_callback(struct GLFWwindow* window_handle, int key, int scancode, int action, int mods);
static void scroll_callback(struct GLFWwindow* window_handle, double xoff, double yoff);
static void char_callback(struct GLFWwindow* window_handle, unsigned int codepoint);

/* Implement window data struct */
struct window
{
    struct GLFWwindow* window_handle;        
    window_error_cb error_cb;
    window_key_cb key_cb;
    window_scroll_cb scroll_cb;
    window_char_cb char_cb;
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
    glfwSetScrollCallback(window->window_handle, scroll_callback);
    glfwSetCharCallback(window->window_handle, char_callback);

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

/* -------------------------------------------------- */
/*                    Operations                      */
/* -------------------------------------------------- */
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

void window_set_clipboard_string(struct window* window, const char* string)
{
    glfwSetClipboardString(window->window_handle, string);
}

void window_hide_cursor(struct window* window)
{
    glfwSetInputMode(window->window_handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void window_show_cursor(struct window* window)
{
    glfwSetInputMode(window->window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void window_set_cursor_pos(struct window* window, double x, double y)
{
    glfwSetCursorPos(window->window_handle, x, y);
}

void window_close(struct window* window)
{
    glfwSetWindowShouldClose(window->window_handle, GLFW_TRUE);
}

/* -------------------------------------------------- */
/*                     Getters                        */
/* -------------------------------------------------- */
bool window_should_close(struct window* window)
{
    return glfwWindowShouldClose(window->window_handle);
}

void window_get_size(struct window* window, int* width, int* height)
{
    glfwGetWindowSize(window->window_handle, width, height);
}

void window_get_framebuffer_size(struct window* window, int* width, int* height)
{
    glfwGetFramebufferSize(window->window_handle, width, height);
}

const char* window_get_clipboard_string(struct window* window)
{
    return glfwGetClipboardString(window->window_handle);
}

void window_get_cursor_pos(struct window* window, double* x, double* y)
{
    glfwGetCursorPos(window->window_handle, x, y);
}

enum key_action window_get_key(struct window* window, enum key k)
{
    return glfw_action_to_action(glfwGetKey(window->window_handle, k));
}

enum key_action window_get_mouse_button(struct window* window, enum mouse_button m)
{
    return glfw_action_to_action(glfwGetMouseButton(window->window_handle, m));
}

/* -------------------------------------------------- */
/*                    Callbacks                       */
/* -------------------------------------------------- */
void window_set_error_callback(struct window* window, window_error_cb error_cb)
{
    glfwSetErrorCallback(error_cb);
    window->error_cb = error_cb;
}

void window_set_key_callback(struct window* wnd, window_key_cb cb)       { wnd->key_cb    = cb; }
void window_set_scroll_callback(struct window* wnd, window_scroll_cb cb) { wnd->scroll_cb = cb; }
void window_set_char_callback(struct window* wnd, window_char_cb cb)     { wnd->char_cb   = cb; }

/* -------------------------------------------------- */
/* Helper function implementation                     */
/* -------------------------------------------------- */
void configure_glfw()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
}

static enum key_action glfw_action_to_action(int glfw_action)
{
    switch (glfw_action) {
        case GLFW_PRESS:   return KEY_ACTION_PRESS;
        case GLFW_RELEASE: return KEY_ACTION_RELEASE;
        case GLFW_REPEAT:  return KEY_ACTION_REPEAT;
        default:           return -1;
    }
}

static void key_callback(GLFWwindow* window_handle, int key, int scancode, int action, int mods)
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

static void scroll_callback(struct GLFWwindow* window_handle, double xoff, double yoff)
{
    struct window* window = glfwGetWindowUserPointer(window_handle);
    if (window->scroll_cb)
        window->scroll_cb(window, xoff, yoff);
}

static void char_callback(struct GLFWwindow* window_handle, unsigned int codepoint)
{
    struct window* window = glfwGetWindowUserPointer(window_handle);
    if (window->char_cb)
        window->char_cb(window, codepoint);
}

