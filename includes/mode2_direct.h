#include <stdio.h>
#include "miniprint.h"
#include "directprint.h"

#define POK_LCD_W 220
#define POK_LCD_H 176
#define POK_COLORDEPTH 4
#define width 220
#define height 176

uint8_t drawcolor;

uint16_t palette[16] = {
0x18e4,
0x4a8a,
0xa513,
0xffff,
0x2173,
0x52bf,
0x0d11,
0x3d9b,
0x8915,
0xf494,
0xa102,
0xfc23,
0xfee7,
0x7225,
0x53e2,
0x8663
};

void lcdRefresh() {
}

void setPixel( uint32_t x, uint32_t y, uint32_t color ){
    DIRECT::setPixel(x, y, color);
}

void vline( uint32_t x, uint32_t y, uint32_t h, uint32_t color ){
    if( x >= width || y >= height || !h ) return;
    if( y+h >= height )
	h = height - y;

    color &= 0xF;
    
    while( h-- ){
	DIRECT::setPixel( x, y++, color );
    }
}

void hline( uint32_t x, uint32_t y, uint32_t w, uint32_t color ){
    if( x >= width || y >= height || !w ) return;
    if( x+w >= width )
	w = width - x;

    while( w-- ){
	DIRECT::setPixel( x++, y, color );
    }
}

void drawRect( int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color ){
    if( x >= width || y >= height ) return;
    vline( x, y, h, color );
    vline( x+w, y, h, color );
    hline( x, y, w, color );
    hline( x, y+h, w, color );
}


void fillRect( int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color ){
    if( x >= width || y >= height ) return;
    if( y < 0 ){
	h += y;
	y = 0;
    }
    if( x < 0 ){
	w += x;
	x = 0;
    }
    if( y+h >= height )
	h = height - y;
    if( x+w >= width )
	w = width - x;
    
    color &= 0xF;
    while( h-- ){
	DIRECT::setPixel(x, y++, color);
	for( uint32_t i=0; i<w; ++i ){
	    DIRECT::toggle_data();
	}
    }
}

void drawBitmap(int x, int y, int w, int h, const unsigned char *bitmap){
    uint32_t skip = 0;
    if( x <= -w || x >= width || y <= -h || y >= height )
	return;

    if( y < 0 ){
	bitmap -= y*(w>>1);
	h += y;
	y = 0;
    }

    if( y+h >= height ){
	h = height - y;
    }

    if( x < 0 ){
	skip = -x/2;
	bitmap += skip;
	w += x;
	x = 0;
    }

    if( x+w >= width ){
	skip += (x+w - width) >> 1;
	w = width - x;
    }

    w >>= 1;
    while( h-- ){
	DIRECT::goToXY( x, y++ );
	for( int i=w; i; --i ){
	    DIRECT::write_data( palette[(*bitmap>>4)&0xF] );
	    DIRECT::write_data( palette[*bitmap&0xF] );
	    bitmap++;
	}
	bitmap += skip;
    }
}

void ecprintf( int *b ){
    char *str = (char *) *b++;

    while( char c = *str++ ){
	if( c == '%' ){
	    c = *str++;
	    if( !c ) return;
	    if( c == 'd' ){
		print( *b++ );
		continue;
	    }else if( c == 'c' ){
		print( (char) *b++ );
		continue;
	    }else if( c == 's' ){
		print( (char *) *b++ );
		continue;
	    }else if( c == 'p' ){
		uint32_t x = *b++;
		for( int shift=28; shift>=0; shift-=4 ){
		    uint32_t y=(x>>shift)&0xF;
		    if( y > 9 ) write('A'+(y-0xA));
		    else write( '0' + y );
		}
		continue;
	    }
	}
	write( c );
    }

}

void cprintf( const char *str, ... ){
    int *b = (int *) &str;
    ecprintf( b );
}
