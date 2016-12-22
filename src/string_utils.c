#include "string_utils.h"
#include <string.h>
#include <assert.h>

/* Assert messages */
#define NOT_INITIALIZED_MSG "You should use cstring_new to initialize the string first"
#define APPENDING_NULL_MSG "You are trying to append a null pointer."

/* -------------------------------------------------- */
/* Utilities                                          */
/* -------------------------------------------------- */
static char* create_string(const char* src)
{
    char* str;
    if (src != NULL)
    {
        size_t src_size = strlen(src);
        str = (char*)malloc(src_size + 1);
        strncpy(str, src, src_size);
    }
    else
    {
        str = (char*)malloc(1);
        memset(str, 0, 1);
    }

    return str;
}

/* -------------------------------------------------- */
/* API                                                */
/* -------------------------------------------------- */
void cstring_init(struct cstring* cstr, const char* str)
{
    memset(cstr, 0, sizeof(struct cstring));
    cstr->data = create_string(str);
}

void cstring_destroy(struct cstring* str)
{
    assert(str->data != NULL && NOT_INITIALIZED_MSG);
    free(str->data);
}

void cstring_append(struct cstring* dest, const char* src)
{
    assert(dest->data != NULL && NOT_INITIALIZED_MSG);
    assert(src != NULL && APPENDING_NULL_MSG);

    size_t str_len = strlen(dest->data);
    size_t src_len = strlen(src);

    /* Append */
    dest->data = realloc(dest->data, str_len + src_len + 1);
    strncat(dest->data, src, src_len);
}
