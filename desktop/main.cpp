#include <stdio.h>
#include "../kernel/kapi.h"
#include "../kernel/tagtype.h"
#include "api.h"
#include "lcd.hpp"
#include "miniprint.h"
#include "font8x8.h"

const char *path = "loader/desktop";
const uint8_t bgcolor = 1;
const uint8_t barcolor = 2;
const uint8_t hlcolor = 8;

struct Item {
    uint8_t icon[ 36*18 ];
    long tell;
    int32_t x, y, tx, ty;

    void setPosition( int8_t x, int8_t y ){
	this->tx = this->x = uint32_t(x) << 16;
	this->ty = this->y = uint32_t(y) << 16;
    }

    void moveTo( int8_t x, int8_t y ){
	this->tx = uint32_t(x)<<16;
	this->ty = uint32_t(y)<<16;
    }

    void update(){
	x -= (tx - x*3)>>2;
	y -= (ty - y*3)>>2;
    }

    bool moving(){
	return (tx>>16) != (x>>16) || (ty>>16) != (y>>16);
    }

    bool load( dirent *e ){
	tell = -1;

	if( !(e->d_type & DT_ARC) )
	    return false;

	char buf[256], *bufp = buf;
	const char *dir = path;

	while( *dir ) *bufp++ = *dir++;
	*bufp++ = '/';
	
	bool isPop = false;
	for( uint32_t i=0; e->d_name[i]; ++i ){
	    *bufp++ = e->d_name[i];
	    if( e->d_name[i] == '.' &&
		(e->d_name[i+1]&0xDF) == 'P' &&
		(e->d_name[i+2]&0xDF) == 'O' &&
		(e->d_name[i+3]&0xDF) == 'P' )
		isPop = true;
	}

	if( !isPop )
	    return false;
	
	*bufp++ = 0;

	FILE *f = FS.fopen( buf, "rb" );
	if( !f ) return false;

	struct {
	    uint32_t id, len;
	} tag;

	while( FS.fread( &tag, sizeof(tag), 1, f ) ){
	    if( tag.id == TAG_IMG_36X36_4BPP ){
		FS.fread( icon, 1, sizeof(Item::icon), f );
		break;
	    }else if( tag.id == TAG_CODE ){
		break;
	    }else{
		FS.fseek( f, tag.len, SEEK_CUR );
	    }
	}
    
	FS.fclose(f);
	
	tell = e->d_off;
	return true;
    }
    
} items[5];
int itemStart, itemEnd, itemCount;
int selection;

void showModes( KAPI *kapi ){
    fillRect( 0,  0, width, height, bgcolor );
    fillRect( 0, 27, width, 32, barcolor );
    fillRect( selection*32, 27, 32, 32, hlcolor );
    hline( 0, 26, width, 3 );
    hline( 0, 59, width, 3 );

    cursor_x = 0;
    cursor_y = 0;
    print("hello world");
    
    if( isPressedRight() ){
	selection++;
	if( selection == 4 ) selection = 4;
    }else if( isPressedLeft() ){
	selection--;
	if( selection < 0 ) selection = 0;
    }
    
    for( int i=0; i<4; ++i ){
	drawBitmap( i*32+4, 31, 36, 36, items[i].icon );
    }

    lcdRefresh();    
}

/* * /
extern "C" {

    void *__wrap__sbrk( int incr )
    {
	extern char   _pvHeapStart;
	static char * heap_end;
	char *        prev_heap_end;

	if (heap_end == 0)
	    heap_end = & _pvHeapStart;

	prev_heap_end = heap_end;
	heap_end += incr;

	return (void *) prev_heap_end;
    }
}
/* */

void error( KAPI *kapi ){
}

long diroff = 0;
void init( KAPI *kapi ){
    
    font = font8x8;
    cursor_x = 0;
    cursor_y = 0;
    drawcolor = 7;

    DIR *d = FS.openddir( path );

    if( !d ){
	api.run = error;
	return;
    }
    
    while( dirent *e = FS.readdir( d ) )
    {
	if( items[diroff].load( e ) ){
	    diroff++;
	    if( diroff == 5 )
		break;
	}
    }
    
    FS.closedir( d );

    api.run = showModes;
    showModes( kapi );
}

API api = {
    init
};

