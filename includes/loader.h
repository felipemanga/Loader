#pragma once

#include "kernel.h"
#include "fsapi.h"

struct Loader : public Process {
    bool (*getIcon)( const char *fileName, uint32_t hash, uint8_t *buffer );
    void (*activate)( Kernel *kapi );
};

bool getIcon( const char *fileName, uint32_t hash, uint8_t *buf );
void activate( Kernel *kapi );
