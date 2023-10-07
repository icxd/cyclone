#ifndef _CYCLONE_COMMON_H
#define _CYCLONE_COMMON_H

#define VM_DEBUG

#define ASSERT(cond, msg, ...)                                                 \
    do {                                                                       \
        if (!(cond)) {                                                         \
            fprintf(stderr, "%s:%u: ERROR: " msg "\n", __FILE__, __LINE__,     \
                    ##__VA_ARGS__);                                            \
            exit(1);                                                           \
        }                                                                      \
    } while (0)
#define UNIMPLEMENTED() ASSERT(false, "Unimplemented")
#define UNIMPLEMENTED_MSG(msg, ...)                                            \
    ASSERT(false, "Unimplemented: " msg, ##__VA_ARGS__)

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void *rawptr;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef ptrdiff_t isize;

typedef struct string string;
struct string {
    char *data;
    usize size;
};
#define SV(s) ((string){(s), strlen(s)})
#define SV_LEN(s, len) ((string){(s), (len)})
#define SV_NULL ((string){NULL, 0})
#define SV_FMT "%.*s"
#define SV_ARG(s) (int)(s).size, (s).data

static inline string sv_from_fmt(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    va_list args2;
    va_copy(args2, args);
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char *data = malloc(size + 1);
    vsnprintf(data, size + 1, fmt, args2);
    va_end(args2);
    return (string){data, size};
}

static inline bool sv_eq(string a, string b) {
    return a.size == b.size && memcmp(a.data, b.data, a.size) == 0;
}

static inline bool sv_eq_cstr(string a, const char *b) {
    usize b_size = strlen(b);
    return a.size == b_size && memcmp(a.data, b, b_size) == 0;
}

static inline string sv_substr(string s, usize start, usize end) {
    return (string){s.data + start, end - start};
}

static inline string sv_concat(string a, string b) {
    string result;
    result.size = a.size + b.size;
    result.data = malloc(result.size + 1);
    memcpy(result.data, a.data, a.size);
    memcpy(result.data + a.size, b.data, b.size);
    result.data[result.size] = '\0';
    return result;
}

static inline char *u32_to_hex(u32 value) {
    char *result = malloc(9);
    sprintf(result, "0x%08x", value);
    return result;
}

typedef u8 Color;
enum {
    COLOR_BLACK = 30,
    COLOR_RED = 31,
    COLOR_GREEN = 32,
    COLOR_YELLOW = 33,
    COLOR_BLUE = 34,
    COLOR_MAGENTA = 35,
    COLOR_CYAN = 36,
    COLOR_WHITE = 37,
};

typedef u8 Attribute;
enum {
    ATTR_NONE = 0,
    ATTR_BOLD = 1,
    ATTR_DIM = 2,
    ATTR_UNDERLINED = 4,
    ATTR_BLINK = 5,
    ATTR_REVERSE = 7,
    ATTR_HIDDEN = 8,
};

static inline char *color(Color color, Attribute attr) {
    char *result = malloc(10);
    sprintf(result, "\033[%u;%um", attr, color);
    return result;
}

static inline char *color_reset(void) {
    char *result = malloc(10);
    sprintf(result, "\033[0m");
    return result;
}

#endif // _CYCLONE_COMMON_H