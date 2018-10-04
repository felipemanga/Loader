#include <stdio.h>
#include "../kernel/kapi.h"
#include "../kernel/tagtype.h"
#include "api.h"
#include "lcd.hpp"
#include "miniprint.h"
#include "font8x8.h"

using namespace DESKTOP;

const char *widgetspath = "loader/desktopwidgets";
const char *path = "loader/desktop";
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
uint8_t hlcolor;

char *concat( char *str, const char *src ){
    while(*str) str++;
    while(*src) *str++ = *src++;
    *str = 0;
    return str;
}

bool compare( const char *a, const char *b ){
    while( *a && *b ){
	char la = *a++, lb = *b++;
	if( la >= 'a' && la <= 'z' ) la &= 0xDF;
	if( lb >= 'a' && lb <= 'z' ) lb &= 0xDF;
	if( la != lb )
	    return false;
    }
    return *a == *b;
}

struct Item {
    uint8_t icon[ 36*18 ];
    char name[ 36 ];
    bool loaded, isPop;

    bool load( DIR *d, int32_t off ){
	FS.seekdir( d, off );
	
	dirent *e = FS.readdir( d );
	if( !e || !(e->d_type & DT_ARC) )
	    return false;

	char buf[256], *bufp = buf, *namep = name;
	const char *dir = path;

	while( *dir ) *bufp++ = *dir++;
	*bufp++ = '/';
	
	isPop = false;
	for( uint32_t i=0; e->d_name[i]; ++i ){
	    *bufp++ = *namep++ = e->d_name[i];
	    if( i>=35 ) namep--;
	    if( e->d_name[i] == '.' &&
		(e->d_name[i+1]&0xDF) == 'P' &&
		(e->d_name[i+2]&0xDF) == 'O' &&
		(e->d_name[i+3]&0xDF) == 'P' ){
		isPop = true;
		*(namep-1) = 0;
	    }
	}

	*bufp++ = *namep++ = 0;

	if( !isPop )
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
int32_t selection;

int prev( int i ){
    i--;
    if( i<0 ) i = sizeof(items)/sizeof(items[0])-1;
    return i;
}

int next( int i ){
    i++;
    if( uint32_t(i)>sizeof(items)/sizeof(items[0])-1 ) i=0; // no modulo!
    return i;
}

bool countFiles( DIR *d ){

    for( fileCount=0; FS.readdir( d ); fileCount++ ){;}
    return fileCount > 0;

}

bool stopped(){
    return x == tx && y == ty;
}

bool addSelectionToPath( char *buf ){
    DIR *d = FS.opendir( path );
    if( !d ) return false;

    FS.seekdir(d, selection);
    dirent *e = FS.readdir(d);

    if( e )
	concat( buf, e->d_name );
    
    FS.closedir(d);
    
    return e != nullptr;
}

bool draw(){
    bool ret = stopped();

    lcdRefresh();
    api.clear();
    
    if( ret ){
	fillRect(
	    api.itemStrideX+api.itemOffsetX-1,
	    api.itemStrideY+api.itemOffsetY-1,
	    38, 38, hlcolor++
	    );
    }


    for( int i=0, c=start; i<maxitem; ++i, c=next(c) ){
	api.drawBitmap(
	    i*api.itemStrideX+api.itemOffsetX+x,
	    i*api.itemStrideY+api.itemOffsetY+y,
	    36, 36, items[c].icon
	    );
    }

    if( ret ){
	
	cursor_x = api.lblX;
	Item &sel = items[next(start)];
	
	if( api.lblCenter ){
	    for( int i=0; sel.name[i]; ++i )
		cursor_x -= 2;
	}
	
	cursor_y = api.lblY;
	drawcolor = api.lblColor;
	print( sel.name );
	
    }else{

	if( x < tx ) x++;
	else if( x > tx ) x--;
	if( x == tx ) x = tx = 0;
	if( y < ty ) y++;
	else if( y > ty ) y--;
	if( y == ty ) y = ty = 0;

    }

    return ret;
}

bool shiftRight;
bool startSelection( KAPI *kapi ){
    char buf[256];
    buf[0] = 0;
    concat( buf, path );
    concat( buf, "/" );
    if( !addSelectionToPath( buf ) )
	return false;

    kapi->createProcess( buf );
    return true;
}

void shift( int32_t fileId, int32_t itemId ){

    DIR *d = FS.opendir( path );

    if( !items[itemId].load( d, fileId ) ){
	items[itemId].loaded = false;
	uint8_t *p = items[itemId].icon;
	for( int i=36*18; i; --i )
	    *p++ = 0;
    }else
	items[itemId].loaded = true;

    FS.closedir(d);

}

void showModes( KAPI *kapi ){
    int32_t fileId, itemId;
    
    if( stopped() ){
	
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
	    shift( fileId, itemId );
	}
	
	if( isPressedA() && items[selection].loaded ){
	    while( isPressedA() );
	    startSelection( kapi );
	    return;
	}
	
	if( isPressedRight() || isPressedDown() ){
	    
	    tx = api.moveX;
	    ty = api.moveY;
	    shiftRight = true;
	    
	}else if( isPressedLeft() || isPressedUp() ){
	    
	    selection--;
	    if( selection < 0 ) selection = fileCount-1;
	    fileId = selection-1;
	    if( fileId < 0 ) fileId = fileCount-1;
	    start = prev(start);
	    itemId = start;
	    x = api.moveX;
	    y = api.moveY;

	    shift( fileId, itemId );

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

int initPluginCount = 0;
void initPlugin( KAPI *kapi ){

    FS.init("");
    DIR *d = FS.opendir( widgetspath );
    if( d ){
	FS.seekdir( d, initPluginCount++ );
	
	while( dirent *e = FS.readdir(d) ){
	    if( e->d_name[0] == '.' ) continue;
	    
	    char fullName[256], *fnp = fullName;
	    const char *src = widgetspath;
	    while( (*fnp++ = *src++) );
	    fnp--;
	    *fnp++ = '/';
	    src = e->d_name;
	    while( (*fnp++ = *src++) );
	    
	    kapi->createProcess( fullName );
	    FS.closedir(d);
	    
	    return;
	}

	FS.closedir(d);
    }
    api.run = showModes;
	
}

void init( KAPI *kapi ){
    
    font = font8x8;
    cursor_x = 0;
    cursor_y = 0;
    drawcolor = 7;

    DIR *d = FS.opendir( path );

    if( !d ){
	showError("Desktop folder missing");
	return;
    }

    if( !countFiles(d) ){
	FS.closedir( d );
	showError("Empty Desktop");
	return;
    }

    int cfile = fileCount - 1;
    for( int i=0; i<maxitem; ++i ){
	items[i].loaded = items[i].load( d, cfile++ );
	if( cfile >= fileCount )
	    cfile = 0;
    }
    start = 0;
    FS.closedir( d );

    initPluginCount = 0;
    api.run = initPlugin;
    initPlugin( kapi );
}

void clear(){    
    fillRect(
	api.clearX,
	api.clearY,
	api.clearWidth,
	api.clearHeight,
	api.clearColor
	);
}

void setCursor( int x, int y ){
    cursor_x = x;
    cursor_y = y;
}

void setDrawColor( uint32_t c ){
    drawcolor = c&0xF;
}

namespace DESKTOP {
    
API api = {
    init, 0, 0,

    clear,    
    fillRect,
    vline,
    hline,
    setPixel,
    drawBitmap,
    setDrawColor,
    setCursor,
    cprintf,
    lcdRefresh,
    
    screenbuffer,
    palette,
    width, height,
    
    0, 0, width, height, 0, 
    55, 18, true, 7, // lblX, lblY, lblCenter, lblColor

    40, -2, 0, 26, // itemStrideX, itemOffsetX, itemStrideY, itemOffsetY

    -40, 0, // moveX, moveY
};

}
