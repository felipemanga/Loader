#pragma once

#include "kernel.h"
#include "fsapi.h"

struct Loader : public Process {
    bool (*getIcon)( const char *fileName, uint32_t hash, uint8_t *buffer );
};

bool getIcon( const char *fileName, uint32_t hash, uint8_t *buf );
