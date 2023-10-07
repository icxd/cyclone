#include <stdio.h>
#include <stdlib.h>

#include "file.h"

File *file_new(string path) {
    File *file = malloc(sizeof(*file));
    file->path = path;

    vec_init(&file->contents);

    FILE *fp = fopen(path.data, "r");
    if (!fp) {
        fprintf(stderr, "error: could not open file '" SV_FMT "'\n",
                SV_ARG(path));
        exit(1);
    }

    char c;
    while ((c = fgetc(fp)) != EOF) {
        vec_push(&file->contents, c);
    }

    fclose(fp);

    file->id = -1;
    return file;
}

void file_free(File *file) {
    vec_free(&file->contents);
    free(file);
}