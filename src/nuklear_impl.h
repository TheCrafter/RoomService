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
#ifndef _NUKLEAR_IMPL_H_
#define _NUKLEAR_IMPL_H_

#include <assert.h>
#include <string.h>
#include <nuklear_config.h>
#include <nuklear.h>
#include "window.h"

/*
 * ==============================================================
 *
 *                              API
 *
 * ===============================================================
 */
enum nk_impl_init_state {
    NK_IMPL_DEFAULT = 0,
    NK_IMPL_INSTALL_CALLBACKS
};

static struct nk_context*   nk_impl_init(struct window* wnd, enum nk_impl_init_state);
static void                 nk_impl_shutdown(void);
static void                 nk_impl_font_stash_begin(struct nk_font_atlas **atlas);
static void                 nk_impl_font_stash_end(void);
static void                 nk_impl_new_frame(void);
static void                 nk_impl_render(enum nk_anti_aliasing, int max_vertex_buffer, int max_element_buffer);

static void                 nk_impl_device_destroy(void);
static void                 nk_impl_device_create(void);

static void                 nk_impl_char_callback(struct window* wnd, unsigned int codepoint);
static void                 nk_impl_scroll_callback(struct window* wnd, double xoff, double yoff);

#endif // ! _NUKLEAR_IMPL_H_

/*
 * ==============================================================
 *
 *                          IMPLEMENTATION
 *
 * ===============================================================
 */
#ifdef NK_IMPL_IMPLEMENTATION

#ifndef NK_IMPL_TEX_MAX
#define NK_IMPL_TEX_MAX 256
#endif

struct nk_impl_device {
    struct nk_buffer cmds;
    struct nk_draw_null_texture null;
    GLuint vbo, vao, ebo;
    GLuint prog;
    GLuint vert_shdr;
    GLuint frag_shdr;
    GLint attrib_pos;
    GLint attrib_uv;
    GLint attrib_col;
    GLint uniform_tex;
    GLint uniform_proj;
    GLuint font_tex;
};

struct nk_impl_vertex {
    float position[2];
    float uv[2];
    nk_byte col[4];
};

static struct nk_impl {
    struct window* wnd;
    int width, height;
    int display_width, display_height;
    struct nk_impl_device ogl;
    struct nk_context ctx;
    struct nk_font_atlas atlas;
    struct nk_vec2 fb_scale;
    unsigned int text[NK_IMPL_TEX_MAX];
    int text_len;
    float scroll;
} impl;

#ifdef __APPLE__
  #define NK_SHADER_VERSION "#version 150\n"
#else
  #define NK_SHADER_VERSION "#version 300 es\n"
#endif

static void nk_impl_device_create(void)
{
    GLint status;
    static const GLchar *vertex_shader =
        NK_SHADER_VERSION
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 TexCoord;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";
    static const GLchar *fragment_shader =
        NK_SHADER_VERSION
        "precision mediump float;\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main(){\n"
        "   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

    struct nk_impl_device *dev = &impl.ogl;
    nk_buffer_init_default(&dev->cmds);
    dev->prog = glCreateProgram();
    dev->vert_shdr = glCreateShader(GL_VERTEX_SHADER);
    dev->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(dev->vert_shdr, 1, &vertex_shader, 0);
    glShaderSource(dev->frag_shdr, 1, &fragment_shader, 0);
    glCompileShader(dev->vert_shdr);
    glCompileShader(dev->frag_shdr);
    glGetShaderiv(dev->vert_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glGetShaderiv(dev->frag_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glAttachShader(dev->prog, dev->vert_shdr);
    glAttachShader(dev->prog, dev->frag_shdr);
    glLinkProgram(dev->prog);
    glGetProgramiv(dev->prog, GL_LINK_STATUS, &status);
    assert(status == GL_TRUE);

    dev->uniform_tex = glGetUniformLocation(dev->prog, "Texture");
    dev->uniform_proj = glGetUniformLocation(dev->prog, "ProjMtx");
    dev->attrib_pos = glGetAttribLocation(dev->prog, "Position");
    dev->attrib_uv = glGetAttribLocation(dev->prog, "TexCoord");
    dev->attrib_col = glGetAttribLocation(dev->prog, "Color");

    {
        /* buffer setup */
        GLsizei vs = sizeof(struct nk_impl_vertex);
        size_t vp = offsetof(struct nk_impl_vertex, position);
        size_t vt = offsetof(struct nk_impl_vertex, uv);
        size_t vc = offsetof(struct nk_impl_vertex, col);

        glGenBuffers(1, &dev->vbo);
        glGenBuffers(1, &dev->ebo);
        glGenVertexArrays(1, &dev->vao);

        glBindVertexArray(dev->vao);
        glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

        glEnableVertexAttribArray((GLuint)dev->attrib_pos);
        glEnableVertexAttribArray((GLuint)dev->attrib_uv);
        glEnableVertexAttribArray((GLuint)dev->attrib_col);

        glVertexAttribPointer((GLuint)dev->attrib_pos, 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
        glVertexAttribPointer((GLuint)dev->attrib_uv, 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
        glVertexAttribPointer((GLuint)dev->attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void nk_impl_device_upload_atlas(const void *image, int width, int height)
{
    struct nk_impl_device *dev = &impl.ogl;
    glGenTextures(1, &dev->font_tex);
    glBindTexture(GL_TEXTURE_2D, dev->font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, image);
}

static void nk_impl_device_destroy(void)
{
    struct nk_impl_device *dev = &impl.ogl;
    glDetachShader(dev->prog, dev->vert_shdr);
    glDetachShader(dev->prog, dev->frag_shdr);
    glDeleteShader(dev->vert_shdr);
    glDeleteShader(dev->frag_shdr);
    glDeleteProgram(dev->prog);
    glDeleteTextures(1, &dev->font_tex);
    glDeleteBuffers(1, &dev->vbo);
    glDeleteBuffers(1, &dev->ebo);
    nk_buffer_free(&dev->cmds);
}

static void nk_impl_render(enum nk_anti_aliasing AA, int max_vertex_buffer, int max_element_buffer)
{
    struct nk_impl_device *dev = &impl.ogl;
    GLfloat ortho[4][4] = {
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f,-2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f,1.0f, 0.0f, 1.0f},
    };
    ortho[0][0] /= (GLfloat)impl.width;
    ortho[1][1] /= (GLfloat)impl.height;

    /* setup global state */
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    /* setup program */
    glUseProgram(dev->prog);
    glUniform1i(dev->uniform_tex, 0);
    glUniformMatrix4fv(dev->uniform_proj, 1, GL_FALSE, &ortho[0][0]);
    glViewport(0,0,(GLsizei)impl.display_width,(GLsizei)impl.display_height);
    {
        /* convert from command queue into draw list and draw to screen */
        const struct nk_draw_command *cmd;
        void *vertices, *elements;
        const nk_draw_index *offset = NULL;

        /* allocate vertex and element buffer */
        glBindVertexArray(dev->vao);
        glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

        glBufferData(GL_ARRAY_BUFFER, max_vertex_buffer, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_element_buffer, NULL, GL_STREAM_DRAW);

        /* load draw vertices & elements directly into vertex + element buffer */
        vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        {
            /* fill convert configuration */
            struct nk_convert_config config;
            static const struct nk_draw_vertex_layout_element vertex_layout[] = {
                {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_impl_vertex, position)},
                {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_impl_vertex, uv)},
                {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_impl_vertex, col)},
                {NK_VERTEX_LAYOUT_END}
            };
            memset(&config, 0, sizeof(config));
            config.vertex_layout = vertex_layout;
            config.vertex_size = sizeof(struct nk_impl_vertex);
            config.vertex_alignment = NK_ALIGNOF(struct nk_impl_vertex);
            config.null = dev->null;
            config.circle_segment_count = 22;
            config.curve_segment_count = 22;
            config.arc_segment_count = 22;
            config.global_alpha = 1.0f;
            config.shape_AA = AA;
            config.line_AA = AA;

            /* setup buffers to load vertices and elements */
            {struct nk_buffer vbuf, ebuf;
            nk_buffer_init_fixed(&vbuf, vertices, (size_t)max_vertex_buffer);
            nk_buffer_init_fixed(&ebuf, elements, (size_t)max_element_buffer);
            nk_convert(&impl.ctx, &dev->cmds, &vbuf, &ebuf, &config);}
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

        /* iterate over and execute each draw command */
        nk_draw_foreach(cmd, &impl.ctx, &dev->cmds)
        {
            if (!cmd->elem_count) continue;
            glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
            glScissor(
                (GLint)(cmd->clip_rect.x * impl.fb_scale.x),
                (GLint)((impl.height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) * impl.fb_scale.y),
                (GLint)(cmd->clip_rect.w * impl.fb_scale.x),
                (GLint)(cmd->clip_rect.h * impl.fb_scale.y));
            glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);
            offset += cmd->elem_count;
        }
        nk_clear(&impl.ctx);
    }

    /* default OpenGL state */
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

static void nk_impl_char_callback(struct window* wnd, unsigned int codepoint)
{
    (void)wnd;
    if (impl.text_len < NK_IMPL_TEX_MAX)
        impl.text[impl.text_len++] = codepoint;
}

static void nk_impl_scroll_callback(struct window* wnd, double xoff, double yoff)
{
    (void)wnd; (void)xoff;
    impl.scroll += (float)yoff;
}

static void nk_impl_clipbard_paste(nk_handle usr, struct nk_text_edit *edit)
{
    const char* text = window_get_clipboard_string(impl.wnd);
    if (text) nk_textedit_paste(edit, text, nk_strlen(text));
    (void)usr;
}

static void nk_impl_clipbard_copy(nk_handle usr, const char *text, int len)
{
    char *str = 0;
    (void)usr;
    if (!len) return;
    str = (char*)malloc((size_t)len+1);
    if (!str) return;
    memcpy(str, text, (size_t)len);
    str[len] = '\0';
    window_set_clipboard_string(impl.wnd, str);
    free(str);
}

static struct nk_context* nk_impl_init(struct window* wnd, enum nk_impl_init_state init_state)
{
    impl.wnd = wnd;
    if (init_state == NK_IMPL_INSTALL_CALLBACKS) {
        window_set_scroll_callback(wnd, nk_impl_scroll_callback);
        window_set_char_callback(wnd, nk_impl_char_callback);
    }

    nk_init_default(&impl.ctx, 0);
    impl.ctx.clip.copy = nk_impl_clipbard_copy;
    impl.ctx.clip.paste = nk_impl_clipbard_paste;
    impl.ctx.clip.userdata = nk_handle_ptr(0);
    nk_impl_device_create();
    return &impl.ctx;
}

static void nk_impl_font_stash_begin(struct nk_font_atlas **atlas)
{
    nk_font_atlas_init_default(&impl.atlas);
    nk_font_atlas_begin(&impl.atlas);
    *atlas = &impl.atlas;
}

static void nk_impl_font_stash_end(void)
{
    const void *image; int w, h;
    image = nk_font_atlas_bake(&impl.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    nk_impl_device_upload_atlas(image, w, h);
    nk_font_atlas_end(&impl.atlas, nk_handle_id((int)impl.ogl.font_tex), &impl.ogl.null);
    if (impl.atlas.default_font)
        nk_style_set_font(&impl.ctx, &impl.atlas.default_font->handle);
}

static void nk_impl_new_frame(void)
{
    int i;
    double x, y;
    struct nk_context *ctx = &impl.ctx;
    struct window* wnd = impl.wnd;

    window_get_size(wnd, &impl.width, &impl.height);
    window_get_framebuffer_size(wnd, &impl.display_width, &impl.display_height);
    impl.fb_scale.x = (float)impl.display_width/(float)impl.width;
    impl.fb_scale.y = (float)impl.display_height/(float)impl.height;

    nk_input_begin(ctx);
    for (i = 0; i < impl.text_len; ++i)
        nk_input_unicode(ctx, impl.text[i]);

    /* optional grabbing behavior */
    if (ctx->input.mouse.grab)
        window_hide_cursor(wnd);
    else if (ctx->input.mouse.ungrab)
        window_show_cursor(wnd);

    nk_input_key(ctx, NK_KEY_DEL,          window_get_key(wnd, KEY_DELETE)      == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_ENTER,        window_get_key(wnd, KEY_ENTER)       == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_TAB,          window_get_key(wnd, KEY_TAB)         == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_BACKSPACE,    window_get_key(wnd, KEY_BACKSPACE)   == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_UP,           window_get_key(wnd, KEY_UP)          == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_DOWN,         window_get_key(wnd, KEY_DOWN)        == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_TEXT_START,   window_get_key(wnd, KEY_HOME)        == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_TEXT_END,     window_get_key(wnd, KEY_END)         == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_START, window_get_key(wnd, KEY_HOME)        == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_END,   window_get_key(wnd, KEY_END)         == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_DOWN,  window_get_key(wnd, KEY_PAGE_DOWN)   == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_UP,    window_get_key(wnd, KEY_PAGE_UP)     == KEY_ACTION_PRESS);
    nk_input_key(ctx, NK_KEY_SHIFT,        window_get_key(wnd, KEY_LEFT_SHIFT)  == KEY_ACTION_PRESS ||
                                           window_get_key(wnd, KEY_RIGHT_SHIFT) == KEY_ACTION_PRESS);

    if (window_get_key(wnd, KEY_LEFT_CONTROL) == KEY_ACTION_PRESS ||
        window_get_key(wnd, KEY_RIGHT_CONTROL) == KEY_ACTION_PRESS) {
        nk_input_key(ctx, NK_KEY_COPY,            window_get_key(wnd, KEY_C)     == KEY_ACTION_PRESS);
        nk_input_key(ctx, NK_KEY_PASTE,           window_get_key(wnd, KEY_P)     == KEY_ACTION_PRESS);
        nk_input_key(ctx, NK_KEY_CUT,             window_get_key(wnd, KEY_X)     == KEY_ACTION_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_UNDO,       window_get_key(wnd, KEY_Z)     == KEY_ACTION_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_REDO,       window_get_key(wnd, KEY_R)     == KEY_ACTION_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT,  window_get_key(wnd, KEY_LEFT)  == KEY_ACTION_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, window_get_key(wnd, KEY_RIGHT) == KEY_ACTION_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_LINE_START, window_get_key(wnd, KEY_B)     == KEY_ACTION_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_LINE_END,   window_get_key(wnd, KEY_E)     == KEY_ACTION_PRESS);
    } else {
        nk_input_key(ctx, NK_KEY_LEFT, window_get_key(wnd, KEY_LEFT) == KEY_ACTION_PRESS);
        nk_input_key(ctx, NK_KEY_RIGHT, window_get_key(wnd, KEY_RIGHT) == KEY_ACTION_PRESS);
        nk_input_key(ctx, NK_KEY_COPY, 0);
        nk_input_key(ctx, NK_KEY_PASTE, 0);
        nk_input_key(ctx, NK_KEY_CUT, 0);
        nk_input_key(ctx, NK_KEY_SHIFT, 0);
    }

    window_get_cursor_pos(wnd, &x, &y);
    nk_input_motion(ctx, (int)x, (int)y);
    if (ctx->input.mouse.grabbed) {
        window_set_cursor_pos(wnd, ctx->input.mouse.prev.x, ctx->input.mouse.prev.y);
        ctx->input.mouse.pos.x = ctx->input.mouse.prev.x;
        ctx->input.mouse.pos.y = ctx->input.mouse.prev.y;
    }

    nk_input_button(ctx, NK_BUTTON_LEFT,   (int)x, (int)y, window_get_mouse_button(wnd, MOUSE_LEFT)   == KEY_ACTION_PRESS);
    nk_input_button(ctx, NK_BUTTON_MIDDLE, (int)x, (int)y, window_get_mouse_button(wnd, MOUSE_MIDDLE) == KEY_ACTION_PRESS);
    nk_input_button(ctx, NK_BUTTON_RIGHT,  (int)x, (int)y, window_get_mouse_button(wnd, MOUSE_RIGHT)  == KEY_ACTION_PRESS);
    nk_input_scroll(ctx, impl.scroll);
    nk_input_end(&impl.ctx);
    impl.text_len = 0;
    impl.scroll = 0;
}

static void nk_impl_shutdown(void)
{
    nk_font_atlas_clear(&impl.atlas);
    nk_free(&impl.ctx);
    nk_impl_device_destroy();
    memset(&impl, 0, sizeof(impl));
}

#endif
