#include "ui_widget_log.h"
#include <stdlib.h>
#include <nuklear_config.h>
#include <nuklear.h>
#include <vector.h>

struct ui_widget_log
{
    const char* title;
    struct vector lines;
};

static void ui_widget_log_render(struct nk_context* ctx, struct ui_view* view, void* view_data)
{
    struct ui_widget_log* data = (struct ui_widget_log*)view_data;
    struct nk_panel layout;

    if (nk_begin(ctx, &layout, data->title, nk_rect(view->x, view->y, view->width, view->height),
                NK_WINDOW_TITLE|NK_WINDOW_MOVABLE))
    {
        nk_layout_row_dynamic(ctx, 10, 1);
        for (size_t i = 0; i < data->lines.size; ++i)
        {
            char* str = *(char**)vector_at(&data->lines, i);
            nk_label(ctx, str, NK_TEXT_LEFT); 
        }
    }
    nk_end(ctx);
}

void ui_widget_log_init(struct ui_view* view, const char* title, float x, float y, float w, float h)
{
    struct ui_widget_log* data = malloc(sizeof(struct ui_widget_log));
    data->title = title;
    vector_init(&data->lines, sizeof(char*));

    view->x = x;
    view->y = y;
    view->width = w;
    view->height = h;
    view->render_cb = &ui_widget_log_render;
    view->data = data;
}

void ui_widget_log_destroy(struct ui_view* view)
{
    struct ui_widget_log* data = view->data;
    vector_destroy(&data->lines);
    free(data);
}

void ui_widget_log_append(struct ui_view* view, const char* msg)
{
    struct ui_widget_log* data = view->data;
    vector_append(&data->lines, &msg);
}
