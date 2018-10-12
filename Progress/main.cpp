#include "mode2_impl.h"
#include "kernel.h"
#include "api.h"
#include "fsapi.h"
#include "drawCharSetPixel.h"
#include "miniprint_impl.h"
#include "fontTIC80.h"
#include "../ldrBIN/drawicon.h"
#include "../ldrBIN/grammar.h"
#include "../ldrBIN/ships.h"

char ship[24*12];

Progress *progress;

void update( Kernel *kernel );

void init( Kernel *kernel ){
    progress = (Progress *) kernel->ipcbuffer;
    font = fontTIC806x6;

    initIcon( ship );
    initGrammar( progress->hash, 12, 5 );

    u32 y = 0;
    for( u32 i=0; i<bufheight; i++, y += 12 ){
	for( u32 x=0; x<6; x++ ){
	    ship[y + 11 - x] = (ship[y + x]>>4) | (ship[y + x]<<4);
	}
    }

    drawcolor = (ship[0]&0xF) ^ 0xF;
    
    PROGRESS::api.run = update;
    update( kernel );    
}

uint32_t prevY = 500;

void update( Kernel *kernel ){

    if( ((Progress *)progress)->total == 0 )
	return;

    uint32_t y = UDIV( 112 * progress->copied, progress->total ) - 24;
    if( y == prevY )
	return;
    prevY = y;

    uint8_t bg = ship[0]&0xF;
    uint8_t bg2 = (bg+1) & 0xF;
    
    fillRect( 0, 0, 42, 88, bg2 );
    fillRect( 42, 0, 24, 88, bg );
    fillRect( 66, 0, 44, 88, bg2 );
    drawBitmap( 42, y, 24, 24, (unsigned char *) ship );
    
    cursor_x = 5;
    cursor_y = 80;

    print( UDIV(100 * progress->copied, progress->total) );
    print( "%" );

    lcdRefresh();
    
}

namespace PROGRESS{
    API api = {
	init, 0, 0
    };
}
