#include <stdio.h>
#include "kernel.h"
#include "tagtype.h"
#include "api.h"
#include "mode2_impl.h"
#include "drawCharSetPixel.h"
#include "miniprint_impl.h"
#include "mini4x6_impl.h"
#include "backlight.h"
#include "strutil.h"

using namespace DESKTOP;

const char *widgetspath = "/.loader/desktopwidgets";
const char *path = "/.loader/desktop";
const uint8_t maxitem = 4;
const char *errmsg;

uint8_t logPos=0;
void log( const char *str, ... ){
    /* * /
    cursor_x = 0;
    cursor_y = logPos*8;
    int *b = (int *) &str;
    ecprintf( b );
    lcdRefresh();
    logPos++;

    for( volatile int i=0; i<2000; ++i ){
	for( volatile int j=0; j<1000; ++j );
    }

    if( logPos >= 10 ){
	logPos = 0;
	api.clear();
    }
    /* */
}

void error( Kernel *kapi ){
    cursor_x = 0;
    cursor_y = 0;
    print(errmsg);
    lcdRefresh();    
}

void showError( const char *m ){
    /* */
    log("%s", m);
    /*/
    cursor_x = 0;
    cursor_y = 0;
    print(m);
    lcdRefresh();
    /* */
    while(true);
}

int32_t x, y, tx, ty;
uint8_t hlcolor;


struct Item {
    uint8_t icon[ 36*18 ];
    char name[ 36 ];
    bool loaded, isPop;

    bool load( DIR *d, int32_t off ){
	log("load %d", off);
	FS.seekdir( d, off );
	
	dirent *e = FS.readdir( d );
	log("readdir %p", e);

	if( !e ){
	    showError("read dir fail");
	    return false;
	}

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

	if( !isPop ){
	    showError(e->d_name);
	    return true;
	}

	log("fopen %s", buf);

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

uint32_t fadeState;

bool draw(){
    bool ret = stopped();

    lcdRefresh();
    api.clear();

    if( fadeState == 1 ){
	fadeState++;
	fadeBacklight(true);
    }
    
    if( ret ){
	fillRect(
	    api.itemStrideX+api.itemOffsetX-1,
	    api.itemStrideY+api.itemOffsetY-1,
	    38, 38, hlcolor++
	    );
    }


    for( int i=0, c=start; i<maxitem; ++i, c=next(c) ){
	if( items[c].loaded )
	    api.drawBitmap(
		i*api.itemStrideX+api.itemOffsetX+x,
		i*api.itemStrideY+api.itemOffsetY+y,
		36, 36, items[c].icon
		);
    }

    if( ret ){

	if( fadeState == 0 )
	    fadeState++;
	
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
bool startSelection( Kernel *kapi ){
    char buf[256];
    FS.sprintf(buf, "%s/", path);
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

void showModes( Kernel *kapi ){
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
	    fadeState = 0;
	    fadeBacklight(false);
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
void initPlugin( Kernel *kapi ){
    DIR *d = FS.opendir( widgetspath );
    log("opendir %s %p", widgetspath, d);
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
    log("showModes");
}

void init( Kernel *kapi ){
    setBacklight(false);
    font = fontMini;
    cursor_x = 0;
    cursor_y = 0;
    drawcolor = 7;

    api.clear();

    log("init");

    DIR *d = FS.opendir( path );

    log("open %s %p", path, d); 

    if( !d ){
	showError("Desktop folder missing");
	return;
    }

    if( !countFiles(d) ){
	FS.closedir( d );
	showError("Empty Desktop");
	return;
    }

    log("file count %d", fileCount);

    int cfile = fileCount - 1;
    int loadCount = 0;
    for( int i=0; i<maxitem; ++i ){
	items[i].loaded = items[i].load( d, cfile++ );
	loadCount += items[i].loaded;
	if( cfile >= fileCount )
	    cfile = 0;
    }

    log("load count: %d", loadCount);

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
