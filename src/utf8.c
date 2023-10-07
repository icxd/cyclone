#include <assert.h>
#include <stdlib.h>

#include "utf8.h"

UTF8String utf8_new(const char *str) {
    UTF8String s;
    s.len = 0;
    while (str[s.len] != '\0') {
        s.len++;
    }
    s.data = malloc(s.len * sizeof(rune));
    for (usize i = 0; i < s.len; i++) {
        s.data[i] = (rune)str[i];
    }
    return s;
}

void utf8_free(UTF8String *str) {
    free(str->data);
    str->data = NULL;
    str->len = 0;
}

const char *utf8_to_cstr(UTF8String s) {
    char *cstr = malloc(s.len + 1);
    for (usize i = 0; i < s.len; i++) {
        rune c = s.data[i];
        assert(c <= 127 && "UTF-8 string contains non-ASCII characters");
        cstr[i] = (char)c;
    }
    cstr[s.len] = '\0';
    return cstr;
}
