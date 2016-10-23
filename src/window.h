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
typedef void(*window_error_cb)();
typedef void(*window_key_cb)(struct window*, int, int, int, int);

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

/* Sends close signal to window */
void window_close(struct window* window);

/* -------------------------------------------------- */
/*                     Getters                        */
/* -------------------------------------------------- */
/* Checks if the window should close or not */
bool window_should_close(struct window* window);

void window_get_size(struct window* window, int* width, int* height);

struct GLFWwindow* window_get_glfw_handle(struct window* window);

/* -------------------------------------------------- */
/*                    Callbacks                       */
/* -------------------------------------------------- */
/* Register an error callback for the window */
void window_set_error_callback(struct window* window, window_error_cb error_cb);

/* Register a key callback for the window */
void window_set_key_callback(struct window* window, window_key_cb key_cb);

#endif /* ! _WINDOW_H */
