#ifndef _CYCLONE_VECTOR_H
#define _CYCLONE_VECTOR_H

#include "common.h"

#define vec(T)                                                                 \
    struct {                                                                   \
        T *data;                                                               \
        usize size;                                                            \
        usize capacity;                                                        \
    }
#define vec_init(v)                                                            \
    do {                                                                       \
        (v)->data = NULL;                                                      \
        (v)->size = 0;                                                         \
        (v)->capacity = 0;                                                     \
    } while (0)
#define vec_free(v)                                                            \
    do {                                                                       \
        free((v)->data);                                                       \
        vec_init(v);                                                           \
    } while (0)
#define vec_push(v, e)                                                         \
    do {                                                                       \
        if ((v)->size == (v)->capacity) {                                      \
            (v)->capacity = (v)->capacity ? (v)->capacity * 2 : 1;             \
            (v)->data =                                                        \
                realloc((v)->data, (v)->capacity * sizeof(*(v)->data));        \
        }                                                                      \
        (v)->data[(v)->size++] = (e);                                          \
    } while (0)
#define vec_pop(v) ((v)->data[--(v)->size])
#define vec_last(v) ((v)->data[(v)->size - 1])
#define vec_get(v, i) (&((v)->data[(i)]))
#define vec_set(v, i, e)                                                       \
    do {                                                                       \
        (v)->data[(i)] = (e);                                                  \
    } while (0)
#define vec_len(v) ((v)->size)

#define vec_foreach(v, e)                                                      \
    for (usize _i = 0; _i < (v)->size && ((e) = (v)->data[_i], 1); _i++)

#define vec_range(v, start, end) for (usize _i = (start); _i < (end); _i++)

#define vec_to_string(v)                                                       \
    ((string){                                                                 \
        .data = (char *)(v)->data,                                             \
        .size = (v)->size * sizeof(*(v)->data),                                \
    })

#endif // _CYCLONE_VECTOR_H