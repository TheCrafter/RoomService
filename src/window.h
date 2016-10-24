/*********************************************************************************************************************/
/*                                                  /===-_---~~~~~~~~~------____                                     */
/*                                                 |===-~___                _,-'                                     */
/*                  -==\\                         `//~\\   ~~~~`---.___.-~~                                          */
/*              ______-==|                         | |  \\           _-~`                                            */
/*        __--~~~  ,-/-==\\                        | |   `\        ,'                                                */
/*     _-~       /'    |  \\                      / /      \      /                                                  */
/*   .'        /       |   \\                   /' /        \   /'                                                   */
/*  /  ____  /         |    \`\.__/-~~ ~ \ _ _/'  /          \/'                                                     */
/* /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                                                      */
/*                   \_|      /        _)   ;  ),   __--~~                                                           */
/*                     '~~--_/      _-~/-  / \   '-~ \                                                               */
/*                    {\__--_/}    / \\_>- )<__\      \                                                              */
/*                    /'   (_/  _-~  | |__>--<__|      |                                                             */
/*                   |0  0 _/) )-~     | |__>--<__|     |                                                            */
/*                   / /~ ,_/       / /__>---<__/      |                                                             */
/*                  o o _//        /-~_>---<__-~      /                                                              */
/*                  (^(~          /~_>---<__-      _-~                                                               */
/*                 ,/|           /__>--<__/     _-~                                                                  */
/*              ,//('(          |__>--<__|     /                  .----_                                             */
/*             ( ( '))          |__>--<__|    |                 /' _---_~\                                           */
/*          `-)) )) (           |__>--<__|    |               /'  /     ~\`\                                         */
/*         ,/,'//( (             \__>--<__\    \            /'  //        ||                                         */
/*       ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'                                          */
/*     `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/                                                     */
/*   ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~                                                       */
/*    ;'( ')/ ,)(                              ~~~~~~~~~~                                                            */
/*   ' ') '( (/                                                                                                      */
/*     '   '  `                                                                                                      */
/*********************************************************************************************************************/
#ifndef _WINDOW_H
#define _WINDOW_H

#include <stdbool.h>
#include "input.h"
/* TODO: Move glfw includes back to .c when window is integrated with nuklear */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/* Window data */
struct window;

/* Callbacks */
typedef void(*window_error_cb)(int error, const char* description);
typedef void(*window_key_cb)(struct window*, int, int, int, int);
typedef void(*window_scroll_cb)(struct window*, double, double);
typedef void(*window_char_cb)(struct window*, unsigned int);

/* Initialize/Deinitialize the window */
struct window* window_create(int width, int height, const char* title);
void window_destroy(struct window* window);

/* -------------------------------------------------- */
/*                    Operations                      */
/* -------------------------------------------------- */
/* Updates the window */
void window_update(struct window* window);

/* Swaps buffers */
void window_swap_buffers(struct window* window);

/* Sets system clipboard to the specified string */
void window_set_clipboard_string(struct window* window, const char* string);

/* Cursor */
void window_hide_cursor(struct window* window);
void window_show_cursor(struct window* window);
void window_set_cursor_pos(struct window* window, double x, double y);

/* Sends close signal to window */
void window_close(struct window* window);

/* -------------------------------------------------- */
/*                     Getters                        */
/* -------------------------------------------------- */
/* Checks if the window should close or not */
bool window_should_close(struct window* window);

/* Get sizes */
void window_get_size(struct window* window, int* width, int* height);
void window_get_framebuffer_size(struct window* window, int* width, int* height);

/* Get string from system clipboard */
const char* window_get_clipboard_string(struct window* window);

/* Cursor position */
void window_get_cursor_pos(struct window* window, double* x, double* y);

/* Input */
enum key_action window_get_key(struct window* window, enum key k);
enum key_action window_get_mouse_button(struct window* window, enum mouse_button m);

/* -------------------------------------------------- */
/*                    Callbacks                       */
/* -------------------------------------------------- */
void window_set_error_callback(struct window* window, window_error_cb cb);
void window_set_key_callback(struct window* window, window_key_cb cb);
void window_set_scroll_callback(struct window* window, window_scroll_cb cb);
void window_set_char_callback(struct window* window, window_char_cb cb);

#endif /* ! _WINDOW_H */
