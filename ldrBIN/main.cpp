#include "loader.h"
#include "tagtype.h"
#include "grammar.h"
#include "drawicon.h"

using u32 = std::uint32_t;

u32 scale,
    hullColorA, hullColorB,
    detailColorA, detailColorB;

RULE(guns){
    []( u32 x, u32 y ){},
	
    []( u32 x, u32 y ){
	u32 h = UDIV(rnd( scale*3, scale*5 ), depth+9) + 1;
	u32 w = UDIV( 10, h ) | 1;
	
	rectangle( x - (w>>1), y, w, h, detailColorA );
    },
	
    []( u32 x, u32 y ){
	u32 h = UDIV(rnd( scale*2, scale*4 ), depth+9) + 1;
	u32 w = UDIV( 6, h ) | 1;
	rectangle( x - (w), y, w>>1, h, detailColorA );
	rectangle( x + (w), y, w>>1, h, detailColorA );
    },
	
    []( u32 x, u32 y ){
	u32 h = UDIV(rnd( scale*2, scale*4 ), depth+9) + 1;
	u32 w = UDIV( 6, h ) | 1;
	rectangle( x - (w>>1), y, w, h, detailColorA );
	rectangle( x - (w>>1) - 1, y+h, w+2, h+2, detailColorB );
    }
};

RULE(hull){
    []( u32 x, u32 y ){
	u32 h = UDIV(rnd( scale*6, scale*10 ), depth+9) + 1;
	u32 w = UDIV( 40, h ) + 2;
	rectangle( x - (w>>1), y, w, h, hullColorA );
	apply(guns, x, y-h );
    }
};

RULE(bridge){
    []( u32 x, u32 y ){
	u32 w = UDIV(rnd( scale*2, scale*5 ), depth+9) + 2;
	u32 h = UDIV( 40, w ) + 2;
	x -= w;
	rectangle( x, y, w, h, hullColorB );
	apply( hull, x, y );
    }
};

RULE(body){
    []( u32 x, u32 y ){
	apply(hull, x, y );
    },
    []( u32 x, u32 y ){
	apply( bridge, x, y );
    },
    []( u32 x, u32 y ){
	apply( hull, x, y );
	apply( bridge, x, y );
    },
    []( u32 x, u32 y ){
	apply( bridge, x, y );
	apply( hull, x, y );
    }
};

Rule root = []( u32 x, u32 y ){
    scale = height - y;
    hullColorA = (rnd()&0x7) + 4;
    hullColorB = (hullColorA+1) & 0xF;
    detailColorA = (hullColorA+8) & 0xF;
    detailColorB = (hullColorA+9) & 0xF;

    u32 bg = (hullColorA+4) & 0xF;
    bg |= ((hullColorA+5) & 0xF) << 4;

    for( uint32_t i=0; i<(width*height>>1); ++i )
	buffer[i] = bg;
    
    apply( body, x, y );
};

FILE *getIcon( const char *fileName, u32 hash ){    
    char buf[24*12];
    FS.sprintf( buf, ".loader/cache/%x.16c", hash );

    FILE *f = FS.fopen( buf, "r" );
    if( f )
	return f;
    
    f = FS.fopen( buf, "w+" );
    if( !f )
	return nullptr;

    initIcon( buf );
    
    initGrammar( hash, 12, rnd(1, 5) );

    u32 y = 0;
    for( u32 i=0; i<height; i++, y += 12 ){
	for( u32 x=0; x<6; x++ ){
	    buf[y + 11 - x] = (buf[y + x]>>4) | (buf[y + x]<<4);
	}
    }

    FS.fwrite( buf, 1, sizeof(buf), f );

    FS.fseek( f, 0, SEEK_SET );
    
    return f;
    
}


