#include "ui.h"
#include <stdlib.h>
#include <stdio.h>

#define NK_IMPL_IMPLEMENTATION
#include "../nuklear_impl.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

void ui_init(struct ui_context* ctx, struct window* wnd)
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
    ui_widget_log_init(&ctx->log_widget, "Output");

    /* Init background color */
    float bg[] = {1.0f, 1.0f, 1.0f, 1.0f};
    ctx->bg = bg;
}

void ui_destroy(struct ui_context* ctx)
{
    /* Shutdown nuklear */
    nk_impl_shutdown();

    /* Destroy widgets */
    ui_widget_log_destroy(&ctx->log_widget);
}

static void ui_extra_layout_render(struct ui_context* ui_ctx)
{
    struct nk_context* ctx = ui_ctx->nk_ctx;
    struct nk_color background = nk_rgb_fv(ui_ctx->bg);
    struct nk_panel layout;

    if (nk_begin(ctx, &layout, "Demo", nk_rect(50, 50, 230, 250),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;
        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "Button"))
        {
            const char* msg = "Button pressed";
            vector_append(&ui_ctx->log_widget.lines, &msg);
        }

        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

        /* Background control widget */
        {
            struct nk_panel combo;
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, &combo, background, nk_vec2(nk_widget_width(ctx),400)))
            {
                nk_layout_row_dynamic(ctx, 120, 1);
                background = nk_color_picker(ctx, background, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                background.r = (nk_byte)nk_propertyi(ctx, "#R:", 0, background.r, 255, 1,1);
                background.g = (nk_byte)nk_propertyi(ctx, "#G:", 0, background.g, 255, 1,1);
                background.b = (nk_byte)nk_propertyi(ctx, "#B:", 0, background.b, 255, 1,1);
                background.a = (nk_byte)nk_propertyi(ctx, "#A:", 0, background.a, 255, 1,1);
                nk_combo_end(ctx);
            }
        }
    }
    nk_end(ctx);

    /* Save background color */
    nk_color_fv(ui_ctx->bg, background);
}

void ui_render(struct ui_context* ctx)
{
    /* Start new nuklear frame */
    nk_impl_new_frame();

    /* Render widgets */
    ui_widget_log_render(&ctx->log_widget, ctx->nk_ctx);

    /* Render extra layout (non-widgets) */
    ui_extra_layout_render(ctx);

    /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
     * with blending, scissor, face culling, depth test and viewport and
     * defaults everything back into a default state.
     * Make sure to either a.) save and restore or b.) reset your own state after
     * rendering the UI.
     */
    nk_impl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}
