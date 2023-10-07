#ifndef _CYCLONE_FILE_H
#define _CYCLONE_FILE_H

#include "common.h"
#include "vector.h"

typedef struct {
    string path;
    vec(char) contents;
    u32 id;
} File;

File *file_new(string path);
void file_free(File *file);

#endif // _CYCLONE_FILE_H