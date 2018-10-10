#pragma once

#include <cstdint>

#define POK_LCD_W 220
#define POK_LCD_H 176
#define POK_COLORDEPTH 4
const uint32_t width = 110;
const uint32_t height = 88;

void fillRect( int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color );
void vline( uint32_t x, uint32_t y, uint32_t h, uint32_t color );
void hline( uint32_t x, uint32_t y, uint32_t w, uint32_t color );
void setPixel( uint32_t x, uint32_t y, uint32_t color );
void drawBitmap(int x, int y, int w, int h, const unsigned char *bitmap);
int drawChar(int x, int y, const unsigned char *bitmap, unsigned int index);

void lcdRefresh();
void cprintf( const char *str, ... );

extern uint8_t drawcolor;
extern uint16_t palette[16];
extern uint8_t screenbuffer[(((POK_LCD_H/2)+1)*POK_LCD_W/2)*POK_COLORDEPTH/8];
