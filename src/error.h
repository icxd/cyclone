#ifndef _CYCLONE_ERROR_H
#define _CYCLONE_ERROR_H

#include "common.h"
#include "location.h"

typedef struct {
    string message;
    Location location;
} Error;

#define ERROR(message, location) ((Error){message, location})

#endif // _CYCLONE_ERROR_H