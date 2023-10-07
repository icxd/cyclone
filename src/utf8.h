#ifndef _CYCLONE_UTF8_H_
#define _CYCLONE_UTF8_H_

#include "common.h"

typedef u8 rune;
typedef struct {
    rune *data;
    usize len;
} UTF8String;

UTF8String utf8_new(const char *str);
void utf8_free(UTF8String *str);

const char *utf8_to_cstr(UTF8String s);

#endif // _CYCLONE_UTF8_H_