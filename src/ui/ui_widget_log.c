#include "ui_widget_log.h"

void ui_widget_log_init(struct ui_widget_log_data* data, struct nk_context* ctx, const char* title)
{
    data->ctx = ctx;
    data->title = title;
    vector_init(&data->lines, sizeof(char*));
}

void ui_widget_log_destroy(struct ui_widget_log_data* data)
{
    vector_destroy(&data->lines);
}

void ui_widget_log_render(struct ui_widget_log_data* data)
{
    struct nk_context* ctx = data->ctx;
    struct nk_panel layout;

    if (nk_begin(ctx, &layout, data->title, nk_rect(500, 200, 250, 250),
                NK_WINDOW_TITLE))
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
