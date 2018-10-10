#pragma once

#include "kernel.h"
#include "fsapi.h"

struct Loader : public Process {
    FILE *(*getIcon)( const char *fileName, uint32_t hash );
};

