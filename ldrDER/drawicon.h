
char *buffer;
uint32_t bufwidth=24, bufheight=24;

void initIcon( char *buf ){
    buffer = buf;
}

void plot( uint32_t x, uint32_t y, uint32_t color ){
    if( x>=bufwidth || y>=bufheight ) return;

    uint32_t i = y*(bufwidth>>1) + (x>>1);
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
	    plot( x+i, y, color );
	}
	y++;
    }
}

void vlineb( uint32_t x, uint32_t y, uint32_t h, uint32_t color ){
    if( x >= 24 || y >= 24 ) return;
    if( h + y >= 24 ) h = 24 - y;

    while( h-- ){
	plot( x, y, color );
	y++;
    }
}

void hlineb( uint32_t x, uint32_t y, uint32_t w, uint32_t color ){
    if( x >= 24 || y >= 24 ) return;
    if( w + x >= 24 ) w = 24 - x;

    while( w-- ){
	plot( x, y, color );
	x++;
    }
}
