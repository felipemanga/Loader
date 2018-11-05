using u32 = std::uint32_t;

u32 scale,
    hullColorA, hullColorB,
    detailColorA, detailColorB;

u32 minX, maxX, minY, maxY;

RULE(greebles){

    []( u32 x, u32 y ){
	u32 w = rnd(3, 5)|1;
	for( u32 i=0; i<w; i++ ){
	    hlineb( x - i, minY-i, i<<1, detailColorA );	    
	}
    },

    []( u32 x, u32 y ){
	u32 w = maxX - minX;
	for( u32 i=0; i<=w+1; i+=2 ){
	    hlineb( x - (i>>1), maxY+((w-i)>>1), i, hullColorA );
	}
    },
	
    []( u32 x, u32 y ){
	
	for( u32 i=minY+1; i<maxY-1; i += 2 ){
	    plot( minX+1, i, detailColorB );
	    plot( maxX-1, i, detailColorB );
	    plot( minX+1, i+1, detailColorA );
	    plot( maxX-1, i+1, detailColorA );
	    plot( minX+2, i, hullColorA );
	    plot( maxX-2, i, hullColorA );
	}

    },
	
    []( u32 x, u32 y ){
	u32 h = UDIV(rnd( scale, scale<<1 ), depth+2) + 1;
	u32 w = UDIV( 10, h );
	if( !w ) w = 1;
	rectangle( x - (w>>1), y, w, h, detailColorA );
    },
	
    []( u32 x, u32 y ){
	u32 h = UDIV(rnd( scale, scale<<1 ), depth+2) + 1;
	u32 w = UDIV( 6, h );
	if( !w ) w = 1;

	rectangle( x - w, y, w, h, detailColorA );
	rectangle( x + w, y, w, h, detailColorA );
    },
	
    []( u32 x, u32 y ){
	u32 h = UDIV(rnd( scale<<1, scale<<2 ), depth+2) + 1;
	u32 w = UDIV( 20, h );
	if( !w ) w = 1;

	rectangle( x - (w>>1), y, w, h, detailColorA );
	rectangle( x - (w>>1) - 1, y, w+1, h>>1, detailColorA );
    }
};

RULE(hull){
    []( u32 x, u32 y ){
	u32 h = UDIV(rnd( scale, scale+(scale>>1) ), depth ) + 1;
	u32 w = UDIV( 100, h * depth );
	if( !w ) w = 1;
	rectangle( x - (w>>1), y, w, h, hullColorB );
	minX = x - (w>>1);
	maxX = minX + w;
	minY = y;
	maxY = y + h;

	for( u32 i=rnd(1, 3); i; --i )
	    apply(greebles, x, y+h-1 );
    },
	
    []( u32 x, u32 y ){
	u32 h = UDIV(rnd( scale>>1, scale ), depth ) + 1;
	u32 w = UDIV( 30, h * depth );
	if( !w ) w = 1;
	u32 x2 = UDIV(w*2, rnd(1, 4));
	u32 x3 = UDIV(w*2, rnd(1, 4));
	
	rectangle( x-(w>>1), y, w, h, hullColorA );
	rectangle( x-(x2>>1), y+h, x2, x2, hullColorA );
	rectangle( x-(x3>>1), y+h+x2, x3, x3, hullColorA );
	
	minX = x - (x3>>1);
	maxX = minX + x3;
	minY = y;
	maxY = y + h + x2 + x3;
	
	for( u32 i=rnd(1, 4); i; --i )
	    apply(greebles, x, y+h-1 );
    }

};

RULE(bridge){
    []( u32 x, u32 y ){
	u32 w = UDIV(rnd( scale>>2, scale ), depth) + 2;
	u32 h = UDIV( 16, w ) + 1;
	int32_t slope = rnd(0, 10), dy = rnd(0, 2) - 1;
	x -= w;
	u32 j = slope;
	for( u32 i=0; i<w; ++i ){
	    vlineb( x+i, y, h, hullColorA );
	    j--;
	    if( !j ){
		j = slope;
		y += dy;
	    }
	}
	
	apply( hull, x + rnd(0, w>>1), y );
	minX = x;
	maxX = x + w;
	minY = y;
	maxY = y + h;
    },
	
    []( u32 x, u32 y ){
	u32 w = UDIV(rnd( scale>>2, scale ), depth) + 2;
	u32 h = UDIV( 20, w ) + 2;
	int32_t slope = rnd(0, 10), dy = rnd(0, 2) - 1;
	x -= w;
	u32 j = slope;
	for( u32 i=0; i<w; ++i ){
	    vlineb( x+i, y, h, hullColorA );
	    j--;
	    if( !j ){
		j = slope;
		y += dy;
	    }
	}
	
	minX = x;
	maxX = x + w;
	minY = y;
	maxY = y + h;
	apply( greebles, x + rnd(0, w>>1), y );
    }

};

RULE(body){
    []( u32 x, u32 y ){
	apply( bridge, x, y );
    },
    []( u32 x, u32 y ){
	apply( hull, x, y );
	apply( bridge, x, rnd(minY+1, maxY-1) );
    },
    []( u32 x, u32 y ){
	apply( bridge, x, y );
	apply( hull, x, rnd(minY-1, maxY+1) );
    }
};

Rule root = []( u32 x, u32 y ){
    scale = 12;
    hullColorA = rnd(6, 14);
    hullColorB = (hullColorA) & 0xF;
    detailColorA = (hullColorA+1) & 0xF;
    detailColorB = (hullColorA-1) & 0xF;

    u32 bg = (hullColorA+7) & 0xF;
    bg |= bg << 4;

    for( uint32_t i=0; i<(bufwidth*bufheight>>1); ++i )
	buffer[i] = bg;
    
    apply( body, x, y );
};
