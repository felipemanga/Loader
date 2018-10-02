#include <stdio.h>
#include "../kernel/kapi.h"
#include "../kernel/tagtype.h"
#include "api.h"
#include "lcd.hpp"
#include "miniprint.h"
#include "font8x8.h"

const char *path = "loader/desktop";
const uint8_t bgcolor = 0;
uint8_t hlcolor;
const uint8_t maxitem = 4;
const char *errmsg;

void error( KAPI *kapi ){
    cursor_x = 0;
    cursor_y = 0;
    print(errmsg);
    lcdRefresh();    
}

void showError( const char *m ){
    errmsg = m;
    api.run = error;
}

int32_t x, y, tx, ty;

struct Item {
    uint8_t icon[ 36*18 ];
    char name[ 36 ];
    bool loaded;

    bool load( DIR *d, long off ){
	FS.seekdir( d, off );
	return load( FS.readdir( d ), false );
    }

    bool load( dirent *e, bool quick ){
	if( !e || !(e->d_type & DT_ARC) ){
	    showError("Error: 1+2=5");
	    return false;
	}

	char buf[256], *bufp = buf, *namep = name;
	const char *dir = path;

	while( *dir ) *bufp++ = *dir++;
	*bufp++ = '/';
	
	bool isPop = false;
	for( uint32_t i=0; e->d_name[i]; ++i ){
	    *bufp++ = *namep++ = e->d_name[i];
	    if( e->d_name[i] == '.' &&
		(e->d_name[i+1]&0xDF) == 'P' &&
		(e->d_name[i+2]&0xDF) == 'O' &&
		(e->d_name[i+3]&0xDF) == 'P' ){
		isPop = true;
		*(namep-1) = 0;
	    }
	}

	if( !isPop ){
	    showError("!pop");
	    return false;
	}
	
	*bufp++ = *namep++ = 0;

	if( quick )
	    return true;

	FILE *f = FS.fopen( buf, "rb" );
	if( !f ){
	    showError("could not open file");
	    return false;
	}

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
	return true;
    }
    
} items[maxitem];
uint32_t start;

int32_t fileCount;

int32_t selection, prevselection;
int32_t hlx;

int prev( int i ){
    i--;
    if( i<0 ) i = sizeof(items)/sizeof(items[0])-1;
    return i;
}

int next( int i ){
    i++;
    if( i>sizeof(items)/sizeof(items[0])-1 ) i=0; // no modulo!
    return i;
}

bool loadFiles( DIR *d ){

    Item &scratch = items[0];
    for( fileCount=0; dirent *e = FS.readdir( d ); ){
	if( scratch.load( e, true ) )
	    fileCount++;
    }

    return fileCount > 0;

}

void draw(){
    
    fillRect( 0,  0, width, height, bgcolor );
    
    if( x == tx ){
	fillRect( 35, 25, 38, 38, hlcolor++ );
	
	cursor_x = 2*40+20-45;
	Item &sel = items[next(start)];
	for( int i=0; sel.name[i]; ++i )
	    cursor_x -= 2;
	cursor_y = 18;
	print( sel.name );
    }


    for( int i=0, c=start; i<maxitem; ++i, c=next(c) ){
	drawBitmap( (i+1)*40+2-45+x, 26, 36, 36, items[c].icon );
    }

    if( x < tx ) x++;
    else if( x > tx ) x--;
    if( x == tx ) x = tx = 0;

    lcdRefresh();    
}

bool shiftRight;
char buf[256];
bool startSelection( KAPI *kapi ){
    
    char *bufp = buf;
    const char *dir = path;
    while( *dir ) *bufp++ = *dir++;
    *bufp++ = '/';
	    
    DIR *d = FS.openddir( path );
    if( !d ) return false;

    FS.seekdir(d, selection);
    dirent *e = FS.readdir(d);

    for( uint32_t i=0; e && e->d_name[i]; ++i )
	*bufp++ = e->d_name[i];
    *bufp++ = 0;
    
    FS.closedir(d);

    showError("exited");
    
    if( e )
	kapi->createProcess( buf );

    return true;
}

void showModes( KAPI *kapi ){
    if( tx == x ){
	int32_t fileId, itemId;
	bool moved = true;
	
	if( isPressedA() && items[selection].loaded ){
	    startSelection( kapi );
	    return;
	}
	
	if( shiftRight ){
	    
	    selection++;
	    if( selection == fileCount ) selection = 0;
	    fileId = selection+1;
	    if( fileId == fileCount ) fileId = 0;
	    fileId++;
	    if( fileId == fileCount ) fileId = 0;
	    itemId = start;
	    start = next(start);
	    shiftRight = false;
	    moved = true;

	    if( isPressedRight() ){
	    
		tx = -40;
		shiftRight = true;
	    
	    }	    

	}else if( isPressedRight() ){
	    
	    tx = -40;
	    shiftRight = true;
	    moved = false;
	    
	}else if( isPressedLeft() ){
	    
	    selection--;
	    if( selection < 0 ) selection = fileCount-1;
	    fileId = selection-1;
	    if( fileId < 0 ) fileId = fileCount-1;
	    start = prev(start);
	    itemId = start;
	    x = -40;
	    moved = true;

	}else moved = false;

	if( moved ){
	    DIR *d = FS.openddir( path );

	    if( !items[itemId].load( d, fileId ) ){
		items[itemId].loaded = false;
		uint8_t *p = items[itemId].icon;
		for( int i=36*18; i; --i )
		    *p++ = 0;
	    }else
		items[itemId].loaded = true;

	    FS.closedir(d);
	}
	
    }
    
    draw();
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

void init( KAPI *kapi ){
    
    font = font8x8;
    cursor_x = 0;
    cursor_y = 0;
    drawcolor = 7;
    prevselection = -100;

    DIR *d = FS.openddir( path );

    if( !d ){
	showError("Desktop folder missing");
	return;
    }

    if( !loadFiles(d) ){
	FS.closedir( d );
	showError("Nothing in Desktop");
	return;
    }

    int cfile = fileCount - 1;
    for( int i=0; i<maxitem; ++i ){
	if( !items[i].load( d, cfile++ ) ){
	    FS.closedir( d );
	    return;
	}else items[i].loaded = true;

	if( cfile >= fileCount )
	    cfile = 0;
    }
    start = 0;
    FS.closedir( d );

    api.run = showModes;
    showModes( kapi );	
    
}

API api = {
    init
};

