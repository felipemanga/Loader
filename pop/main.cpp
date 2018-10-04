#include <stdio.h>
#include "../kernel/kapi.h"
#include "../kernel/tagtype.h"
#include "api.h"
#include "miniprint.h"
#include "font8x8.h"
#include "./lcd.hpp"

using namespace DESKTOP;

const char *widgetspath = "loader/popwidgets";
char path[256];
const uint8_t maxitem = 16;
const char *errmsg;

void initdir();

void error( KAPI *kapi ){
    cursor_x = 0;
    cursor_y = 0;
    drawcolor = 5;
    print(errmsg);
    lcdRefresh();    
}

void showError( const char *m ){
    errmsg = m;
    error(nullptr);
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
    // uint8_t icon[ 36*18 ];
    char name[ 36 ];
    bool loaded, isPop, isDir;

    bool load( DIR *d, int32_t off ){
	FS.seekdir( d, off );
	
	dirent *e = FS.readdir( d );

	isDir = e->d_type & DT_DIR;
	isPop = false;

	if( !e || (e->d_type & (DT_DIR | DT_ARC)) == 0 )
	    return false;

	char buf[256], *bufp = buf, *namep = name;
	const char *dir = path;
	buf[0] = 0;
	bufp = concat( buf, dir );
	*bufp++ = '/';

	isPop = false;
	for( uint32_t i=0; e->d_name[i]; ++i ){
	    *bufp++ = *namep++ = e->d_name[i];
	    if( i>=35 ) namep--;
	    if( e->d_name[i] == '.' ){

		isPop = compare( e->d_name+i+1, "POP" );
		
		if( !isPop )
		    isPop = compare( e->d_name+i+1, "BIN" );
		
		if( isPop )
		    *(namep-1) = 0;
		
	    }
	}

	*bufp++ = *namep++ = 0;

	return isPop || isDir;
/*
	if( !isPop || (e->d_type & DT_DIR) )
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
*/
	
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

bool popPath(){
    char *c = path;
    if( !*c ) return false;

    while( *c ) c++;
    while( c != path && *c != '/' ) c--;
    *c = 0;

    return true;
}

bool addSelectionToPath(){
    DIR *d = FS.opendir( path );
    if( !d ) return false;

    FS.seekdir(d, selection);
    dirent *e = FS.readdir(d);

    if( e ){
	if( path[0] )
	    concat( path, "/" );
	concat( path, e->d_name );
    }
    
    FS.closedir(d);
    
    return e != nullptr;
}

bool stopped(){
    return x == tx && y == ty;
}

bool draw(){
    bool ret = stopped();

    lcdRefresh();
    api.clear();

    cursor_x = 0;
    cursor_y = 0;
    print( path );
    
    for( int i=0, c=start; i<maxitem; ++i, c=next(c) ){
	Item &item = items[c];
	cursor_x = i*5;
	cursor_y = i*5 + 10;
	
	if( item.isDir ){
	    drawcolor = i == 1 ? hlcolor++&0xF : 8;
	    print("[");
	    print(item.name);
	    print("]");
	}else if( item.isPop ){
	    drawcolor = i == 1 ? hlcolor++&0xF : 9;
	    print(item.name);
	}else{
	    drawcolor = i == 1 ? 14 : 13;
	    print(item.name);
	}
	
	/*
	api.drawBitmap(
	    i*api.itemStrideX+api.itemOffsetX+x,
	    i*api.itemStrideY+api.itemOffsetY+y,
	    36, 36, items[c].icon
	    );
	*/
    }

    if( !ret ){

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
    kapi->createProcess( path );
    return true;
}

void shift( int32_t fileId, int32_t itemId ){

    DIR *d = FS.opendir( path );

    if( !items[itemId].load( d, fileId ) ){
	items[itemId].loaded = false;
	/*
	uint8_t *p = items[itemId].icon;
	for( int i=36*18; i; --i )
	    *p++ = 0;
	*/
    }else
	items[itemId].loaded = true;

    FS.closedir(d);

}

void showFiles( KAPI *kapi ){
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

	if( isPressedB() ){
	    
	    while( isPressedB() );

	    if( !popPath() ){
		kapi->createProcess("loader/desktop.pop");
		return;
	    }else{
		initdir();
		return;
	    }
	    
	}
	
	if( isPressedA() && items[selection].loaded ){
	    while(isPressedA());

	    if( !addSelectionToPath() )
		return;

	    if( items[next(start)].isDir ){
		initdir();
		return;		
	    }else{
		startSelection( kapi );
		popPath();
		return;
	    }
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
	    fullName[0] = 0;
	    const char *src = widgetspath;
	    fnp = concat( fullName, src );
	    fnp = concat( fnp, "/" );
	    fnp = concat( fnp, e->d_name );
	    
	    kapi->createProcess( fullName );
	    FS.closedir(d);

	    return;
	}

	FS.closedir(d);
    }
    
    api.run = showFiles;
    initdir();
}

void initdir(){
    DIR *d = FS.opendir( path );

    if( !d ){
	fileCount = 0;
	return;
    }

    if( !countFiles(d) ){
	FS.closedir( d );
	showError("Empty");
	return;
    }

    int cfile = fileCount - 1;
    for( int i=0; i<maxitem; ++i ){
	items[i].loaded = items[i].load( d, cfile++ );
	if( cfile >= fileCount )
	    cfile = 0;
    }
    start = 0;
    selection = 0;
    FS.closedir( d );
}

void init( KAPI *kapi ){
    path[0] = 0;
    font = font8x8;
    cursor_x = 0;
    cursor_y = 0;
    drawcolor = 7;

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
    
    0, 0, width, height, 2, 
    55, 18, true, 7, // lblX, lblY, lblCenter, lblColor

    40, -2, 0, 26, // itemStrideX, itemOffsetX, itemStrideY, itemOffsetY

    -10, 0, // moveX, moveY
};

}
