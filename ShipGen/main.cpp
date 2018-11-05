#include "mode2_impl.h"
#include "kernel.h"
#include "api.h"
#include "fsapi.h"
#include "drawCharSetPixel.h"
#include "miniprint_impl.h"
#include "fontTIC80.h"
#include "backlight.h"
#include "../ldrBIN/drawicon.h"
#include "../ldrBIN/grammar.h"
#include "../ldrBIN/ships.h"

u32 prevState;
char buf[24*12];

void redraw(){
    
    prevState = rndState;

    initIcon( buf );
    
    initGrammar( rndState, 12, 5 );

    u32 y = 0;
    for( u32 i=0; i<bufheight; i++, y += 12 ){
	for( u32 x=0; x<6; x++ ){
	    buf[y + 11 - x] = (buf[y + x]>>4) | (buf[y + x]<<4);
	}
    }
    
}


void update( Kernel *kernel ){
    bool init = rndState == 0x12345678;

    font = fontTIC806x6;

    if( isPressedC() ){
	fadeBacklight(false);
	while( isPressedC() );	
	kernel->createProcess(".loader/desktop.pop");
	return;
    }

    if( isPressedB() || init ){
	while( isPressedA() ); // yes, A.
	redraw();
	fillRect( 0, 0, 110, 88, buf[0]&0xF );
	drawBitmap( 55 - 12, 44 - 12, 24, 24, (unsigned char *) buf );
	drawcolor = (buf[0]&0xF) ^ 0xF;
    }

    cursor_x = 0;
    cursor_y = 60;
    print(" A] Save\n B] Next\n C] Exit");

    if( isPressedA() ){
	char fileName[20];
	FS.sprintf( fileName, "%x.icn", prevState );
	
	FILE *f = FS.fopen( fileName, "wb" );

	cursor_x = 10;
	cursor_y = 10;
	
	if(f){
	    FS.fwrite( buf, 1, sizeof(buf), f );
	    FS.fclose(f);
	}

	cursor_x = 20;
	cursor_y = 5;
	print(fileName);
	lcdRefresh();
	while( isPressedA() );
	
    }

    lcdRefresh();
    if( init )
	fadeBacklight(true);
}

namespace SHIPGEN{
    API api = {
	update, 0, 0
    };
}
