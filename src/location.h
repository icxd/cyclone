#ifndef _CYCLONE_LOCATION_H
#define _CYCLONE_LOCATION_H

#include "common.h"
#include "vector.h"

typedef struct {
    u32 file_id;
    u32 line;
    u32 column;
} Location;

#define LOC(file_id, line, column) ((Location){file_id, line, column})
#define LOC_FMT SV_FMT ":%u:%u"
#define LOC_ARG(c, loc)                                                        \
    SV_ARG(*vec_get(&c->file_names, (loc).file_id)), (loc).line, (loc).column

#endif // _CYCLONE_LOCATION_H