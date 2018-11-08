#pragma once

#include "kernel.h"
#include "fsapi.h"

struct Loader : public Process {
    bool (*getIcon)( const char *fileName, uint8_t *buffer, uint32_t size );
    bool (*drawScreenshot)(
	const char *fileName,
	uint32_t offset,
	uint32_t lineCount
	);
    bool (*drawIconDirect)(
	const char *fileName,
	uint32_t size,
	int32_t x, int32_t y
	);
    void (*activate)( Kernel *kapi );
};

WEAK bool getIcon( const char *fileName, uint8_t *buf, uint32_t size );

WEAK bool drawScreenshot(
    const char *fileName,
    uint32_t offset,
    uint32_t lineCount
    );

WEAK bool drawIconDirect(
    const char *fileName,
    uint32_t size,
    int32_t x, int32_t y
    );

void activate( Kernel *kapi );
