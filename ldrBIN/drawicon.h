
char *buffer;
uint32_t width=24, height=24;

void initIcon( char *buf ){
    buffer = buf;
}

void setPixel( uint32_t x, uint32_t y, uint32_t color ){
    if( x>=width || y>=height ) return;

    uint32_t i = y*(width>>1) + (x>>1);
    uint8_t pixel = buffer[i];
    if (x&1) pixel = (pixel&0xF0)|(color);
    else pixel = (pixel&0x0F) | (color<<4);
    
    buffer[i] = pixel;    
}

void rectangle( uint32_t x, uint32_t y, uint32_t w, int32_t h, uint32_t color ){
    if( x >= 24 || y >= 24 ) return;
    if( h + y >= 24 ) h = 24 - y;
    if( w + x >= 24 ) w = 24 - x;
    
    while( h-- ){
	for( uint32_t i=0; i<w; ++i ){
	    setPixel( x+i, y, color );
	}
	y++;
    }
}

void vline( uint32_t x, uint32_t y, uint32_t h, uint32_t color ){
    if( x >= 24 || y >= 24 ) return;
    if( h + y >= 24 ) h = 24 - y;

    while( h-- ){
	setPixel( x, y, color );
	y++;
    }
}

void hline( uint32_t x, uint32_t y, uint32_t h, uint32_t color ){
    if( x >= 24 || y >= 24 ) return;
    if( h + y >= 24 ) h = 24 - y;

    while( h-- ){
	setPixel( x, y, color );
	y++;
    }
}
