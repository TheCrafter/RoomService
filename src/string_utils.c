#include "string_utils.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Assert messages */
static const char* not_initialized_msg = "You should use cstring_new to initialize the string first";

/* Struct implementation */
struct cstring
{
    char* str;
};

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
struct cstring* cstring_new(const char* str)
{
    struct cstring* cstr = (struct cstring*)malloc(sizeof(struct cstring));
    cstr->str = create_string(str);
    return cstr;
}

void cstring_destroy(struct cstring* str)
{
    assert(str->str != NULL && not_initialized_msg);
    free(str->str);
    free(str);
}

void cstring_append(struct cstring* dest, const char* src)
{
    assert(dest->str != NULL && not_initialized_msg);

    size_t str_len = strlen(dest->str);
    size_t src_len = strlen(src);

    /* Append */
    dest->str = realloc(dest->str, str_len + src_len + 1);
    strncat(dest->str, src, src_len);
}

char* cstring_get(struct cstring* str)
{
    assert(str->str != NULL && not_initialized_msg);
    return str->str;
}
