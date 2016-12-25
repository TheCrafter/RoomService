#include "ui.h"
#include <stdlib.h>
#include <stdio.h>

#define NK_IMPL_IMPLEMENTATION
#include "../nuklear_impl.h"
#include "ui_widget_log.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

void ui_init(struct ui_context* ctx, struct window* wnd, float width, float height)
{
    /* Init nuklear */
    {
        struct nk_context* nk_ctx = nk_impl_init(wnd, NK_IMPL_INSTALL_CALLBACKS);

        /* Init nuklear */
        nk_ctx = nk_impl_init(wnd, NK_IMPL_INSTALL_CALLBACKS);

        /* Load Fonts: if none of these are loaded a default font will be used  */
        {
            struct nk_font_atlas *atlas;
            nk_impl_font_stash_begin(&atlas);

            struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "res/Font/Roboto-Regular.ttf", 16, 0);
            nk_impl_font_stash_end();

            /* Load Cursor: if you uncomment cursor loading please hide the cursor */
            /*nk_style_load_all_cursors(ctx, atlas->cursors);*/

            nk_style_set_font(nk_ctx, &roboto->handle);
        }

        /* style.c */
        /*set_style(nk_ctx, THEME_WHITE);*/
        /*set_style(nk_ctx, THEME_RED);*/
        /*set_style(nk_ctx, THEME_BLUE);*/
        /*set_style(nk_ctx, THEME_DARK);*/

        /* Add nuklear context to ui_context */
        ctx->nk_ctx = nk_ctx;
    }

    /* Init widgets */
    ui_widget_log_init(&ctx->log_view, "Output", 0, 0, 250, height);
    ui_widget_log_init(&ctx->info_view, "Info", width - 250, 0, 250, height);

    /* Init background color */
    float bg[] = {1.0f, 1.0f, 1.0f, 1.0f};
    ctx->bg = bg;
}

void ui_destroy(struct ui_context* ctx)
{
    /* Shutdown nuklear */
    nk_impl_shutdown();

    /* Destroy widgets */
    ui_widget_log_destroy(&ctx->log_view);
    ui_widget_log_destroy(&ctx->info_view);
}

static void ui_view_render(struct ui_context* ctx, struct ui_view* view)
{
    view->render_cb(ctx->nk_ctx, view);
}

void ui_render(struct ui_context* ctx)
{
    /* Start new nuklear frame */
    nk_impl_new_frame();

    /* Render widgets */
    ui_view_render(ctx, &ctx->log_view);
    ui_view_render(ctx, &ctx->info_view);

    /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
     * with blending, scissor, face culling, depth test and viewport and
     * defaults everything back into a default state.
     * Make sure to either a.) save and restore or b.) reset your own state after
     * rendering the UI.
     */
    nk_impl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}
