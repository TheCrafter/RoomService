#include "ui.h"
#include <stdlib.h>
#include <stdio.h>

#define NK_IMPL_IMPLEMENTATION
#include "nuklear_impl.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

struct nk_context* ui_init(struct window* wnd)
{
    /* Init nuklear */
    struct nk_context* ctx = nk_impl_init(wnd, NK_IMPL_INSTALL_CALLBACKS);

    /* Init nuklear */
    ctx = nk_impl_init(wnd, NK_IMPL_INSTALL_CALLBACKS);

    /* Load Fonts: if none of these are loaded a default font will be used  */
    {
        struct nk_font_atlas *atlas;
        nk_impl_font_stash_begin(&atlas);

        struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "res/Font/Roboto-Regular.ttf", 16, 0);
        nk_impl_font_stash_end();

        /* Load Cursor: if you uncomment cursor loading please hide the cursor */
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/

        nk_style_set_font(ctx, &roboto->handle);
    }

    /* style.c */
    /*set_style(ctx, THEME_WHITE);*/
    /*set_style(ctx, THEME_RED);*/
    /*set_style(ctx, THEME_BLUE);*/
    /*set_style(ctx, THEME_DARK);*/

    return ctx;
}

void ui_destroy()
{
    nk_impl_shutdown();
}

void ui_render(struct nk_context* ctx, ui_layout_cb layout_cb, void* layout_data)
{
    nk_impl_new_frame();

    /* Declare nuklear layout via callback */
    layout_cb(ctx, layout_data);

    /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
     * with blending, scissor, face culling, depth test and viewport and
     * defaults everything back into a default state.
     * Make sure to either a.) save and restore or b.) reset your own state after
     * rendering the UI.
     */
    nk_impl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}
