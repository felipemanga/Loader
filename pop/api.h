#pragma once

namespace DESKTOP {

struct API : public PAPI {
    void (*clear)();
    void (*fillRect)( int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color );
    void (*vline)( uint32_t x, uint32_t y, uint32_t h, uint32_t color );
    void (*hline)( uint32_t x, uint32_t y, uint32_t w, uint32_t color );
    void (*setPixel)( uint32_t x, uint32_t y, uint32_t color );
    void (*drawBitmap)(int x, int y, int w, int h, const unsigned char *bitmap);
    void (*setDrawColor)( uint32_t c );
    void (*setCursor)(int, int);
    void (*printf)( const char *, ... );

    void (*lcdRefresh)();

    uint8_t *screenbuffer;
    uint16_t *palette;

    uint8_t width, height;

    volatile uint8_t clearX, clearY, clearWidth, clearHeight, clearColor;
    volatile uint8_t lblX, lblY, lblCenter, lblColor;
    volatile int32_t itemStrideX, itemOffsetX,  itemStrideY, itemOffsetY;
    volatile int8_t moveX, moveY;
};

extern API api;

}
