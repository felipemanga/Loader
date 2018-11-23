#include <stdio.h>
#include <functional>
#include "kernel.h"
#include "tagtype.h"
#include "api.h"
#include "mode2_direct.h"
#include "drawCharSetPixel.h"
#include "miniprint_impl.h"
#include "backlight.h"
#include "fontTIC80.h"
#include "loader.h"
#include "strutil.h"

using namespace NAVIGATOR;

Kernel *kapi;

int32_t x, y, tx, ty;
int32_t selection;
uint8_t hlcolor;
char largeBuf[ 1024 ];
char path[256];
const uint8_t maxitem = 10;
const char *errmsg;
bool forceDraw;

void getXY( int32_t id, int32_t &x, int32_t &y ){
    x = id;
    if( id < (maxitem>>1) ){
	y = 0;
    }else{
	y = api.itemStrideY;
	x -= maxitem>>1; 
    }
    y += api.itemOffsetY;
    x *= api.itemStrideX;
    x += api.itemOffsetX;
}

void initdir();

void error( Kernel *kapi ){
    cursor_x = 10;
    cursor_y = 10;
    drawcolor = 4;
    print( errmsg );
    while(true);
}

void showError( const char *m ){
    errmsg = m;
    error(nullptr);
}

bool loadFileEntry( uint32_t off ){
    FILE *fp = FS.fopen(".loader/index", "rb");

    if( !fp )
	return false;

    FS.fseek( fp, off * 512, SEEK_SET );
    largeBuf[0] = 0;
    FS.fread( largeBuf, 1, 256, fp );
    FS.fclose( fp );

    return true;
}


struct Item {
    int32_t x, y;
    char ext[4];
    uint32_t off;
    bool isDir, isIconLoaded, isIconValid;

    void setOffset( uint32_t off ){
	if( off == this->off )
	    return;
	
	isDir = false;
	isIconLoaded = false;
	isIconValid = true;
	this->off = off;
    }
    
} items[maxitem];

void renderItem( int32_t itemId ){
    Item &item = items[itemId];
    
    if( !item.isIconValid || item.isIconLoaded || !loadFileEntry(item.off) )
	return;

    int32_t x, y;
    getXY( itemId, x, y );
    
    getExtension( item.ext, largeBuf+1 );

    char *namep = largeBuf+1;
    while( *namep++ );
    while( namep != largeBuf+1 && *namep != '/' ) namep--;
    if( *namep=='/' ) namep++;

    item.isDir = largeBuf[0];

    ProcessHandle ph;

    {
	char plugin[ 40 ];
	FS.sprintf( plugin, ".loader/loaders/%s.pop", item.isDir ? "BIN" : item.ext );

	ph = kapi->createProcess( plugin );
    }
    
    Loader *loader  = (Loader *) ph.api;

    if( loader ){
	
	if( itemId == selection ){
	    DIRECT::setPixel(220-1, 176-1, 0);
	    if( !loader->drawScreenshot || !loader->drawScreenshot( largeBuf+1, 0, 90 ) ){
		fillRect(0,0, width,90, hlcolor);

		cursor_x = 110;
		cursor_y = 50;
		drawcolor = api.lblColor;

		char *ch = namep;
		while( *ch++ )
		    cursor_x -= font[0]>>1;
	
		if( item.isDir ){
		    print("[");
		    print(namep);
		    print("]");
		}else{
		    print(namep);
		}
	    }
	}
	
	if( loader->drawIconDirect ){
	    item.isIconLoaded = loader->drawIconDirect(
		largeBuf+1,
		36,
		x, y
		);
	}
	
	if( loader->getIcon && !item.isIconLoaded ){
	    uint8_t buf[36*(36>>1)];
	    item.isIconLoaded = loader->getIcon(largeBuf+1, buf, 36);
	    if( item.isIconLoaded ){
		drawBitmap( x, y, 36, 36, buf );
	    }
	}

	if( !item.isIconLoaded ){
	    fillRect( x, y, 36, 36, 0 );
	    item.isIconValid = false;
	}
	
    }else if( ph.state == ProcessState::error ){
	item.isIconValid = false;
    }else{
	forceDraw = true;
    }
}

int32_t fileCount, fileIndex;

int prev( int i ){
    i--;
    if( i<0 ) i = fileCount-1;
    return i;
}

int next( int i ){
    i++;
    if( i>=fileCount ) i=0; // no modulo!
    return i;
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
    FILE *fp = FS.fopen(".loader/index", "rb");
    if( !fp ) return false;

    FS.fseek( fp, items[selection].off * 512 + 1, SEEK_SET );
    if( path[0] )
	concat( path, "/" );

    char *b = path;
    while(*b) b++;
    FS.fread( path, 1, 256 - (b - path), fp );
    
    FS.fclose(fp);

    return true;
}

bool stopped(){
    return x == tx && y == ty;
}

bool draw( Kernel *kapi ){
    bool ret = stopped();
    int c;

    if( ret && !forceDraw )
	return false;

    drawRect( x-1, y-1, 37, 37, api.clearColor );
    drawRect( x-2, y-2, 39, 39, api.clearColor );
    x = tx;
    y = ty;

    forceDraw = false;

    c = fileIndex;
    for( int i=0; i<selection; ++i, c=next(c) );
    
    Item &sel = items[selection];
    if( !ret )
	sel.off = ~0;
    sel.setOffset(  c);
    renderItem( selection );

    if( forceDraw )
	return true;
    
    c=fileIndex;
    for( int i=0; i<maxitem; ++i, c=next(c) ){
	if( i == selection )
	    continue;

	items[i].setOffset( c );
	renderItem(i);	
    }
	    
    if( !forceDraw ){
	fadeBacklight(true);
	for( int i=0; i<100; ++i ){
	    drawRect( x-1, y-1, 37, 37, hlcolor++ );
	    drawRect( x-2, y-2, 39, 39, hlcolor++ );
	    for( volatile int j=0; j<5000; ++j );
	}
    }

    return ret;
}

bool shiftRight;
bool startSelection( Kernel *kapi ){
    DIRECT::setPixel( 220-1, 176-1, 0 );
    
    char plugin[ 40 ];
    FS.sprintf( (char *) kapi->ipcbuffer, "%s", path );

    FS.sprintf( plugin, ".loader/loaders/%s.pop", items[selection].ext );

    Loader * loader = (Loader *) kapi->createProcess( plugin ).api;
    if( loader )
	loader->activate( kapi );

    return true;
}

void empty( Kernel *kapi ){
    
    if( isPressedB() ){
	while( isPressedB() );
	
	kapi->createProcess(".loader/desktop.pop");
    }
    
}

void update( Kernel *kapi ){
    
    if( stopped() && !forceDraw ){
	
	if( isPressedB() ){
	    
	    while( isPressedB() );

	    if( !popPath() ){
		DIRECT::setPixel(220-1, 176-1, 0);
		kapi->createProcess(".loader/desktop.pop");
		return;
	    }else{
		initdir();
		return;
	    }
	    
	}
	
	if( isPressedA() ){
	    while(isPressedA());

	    if( !addSelectionToPath() )
		return;

	    if( items[selection].isDir ){
		initdir();
		return;		
	    }else{
		startSelection( kapi );
		popPath();
		return;
	    }
	}

	if( isPressedDown() && selection < (maxitem>>1) ){
	    selection += maxitem>>1;
	}else if( isPressedUp() && selection >= (maxitem>>1) ){
	    selection -= maxitem>>1;
	}
	
	
	if( isPressedRight() ){
	    selection++;

	    if( selection == (maxitem>>1) || selection == maxitem ){
		
		selection = 0;
		fileIndex += maxitem;
		
		while( fileIndex >= fileCount )
		    fileIndex -= fileCount;
		
	    }
	    
	}else if( isPressedLeft() ){
	    
	    selection--;
	    if( selection < 0 || selection == (maxitem>>1)-1 ){
		
		selection = (maxitem>>1)-1;
		fileIndex -= maxitem;
		
		while( fileIndex < 0 )
		    fileIndex += fileCount;
				
	    }

	}
	
	getXY( selection, tx, ty );
	
    }

    draw( kapi );
    
}

void flushDirIndex( char *buf, int32_t bufPos ){

    if( bufPos <= 0 )
	return;
    
    FILE *fp = FS.fopen( ".loader/index", "a" );
    
    if( !fp )
	return;
	
    while( bufPos > 0 ){
				
	uint32_t len = strlen(buf+1) + 2;
	
	FS.fwrite( buf, 1, 512, fp );
	buf += len;
	bufPos -= len;
				
    }
    
    FS.fclose(fp);			

}

void initdir(){
	
    DIR *d = FS.opendir( path );
    FILE *fp = FS.fopen( ".loader/index", "w" );

    fileCount = 0;

    if( fp && d ){
	char *buf = largeBuf;
	uint32_t bufPos = 0, pathLen = strlen(path)+2, fileLen;

	FS.fclose(fp);

	while( dirent *e = FS.readdir( d ) ){

	    if( e->d_name[0] == '.' )
		continue;

	    fileLen = pathLen + strlen(e->d_name);

	    if( bufPos + fileLen >= sizeof(largeBuf) ){
		flushDirIndex( buf, bufPos );
		bufPos = 0;
	    }

	    if( e->d_type & DT_ARC ){
			
		char ext[4], tmp[30];
		getExtension( ext, e->d_name );

		FS.sprintf( tmp, ".loader/loaders/%s.pop", ext );

		FILE *f = FS.fopen( tmp, "r" );
		if( f ) FS.fclose(f);
		else continue;
			
		buf[bufPos++] = 0;
			
	    }else if( !(e->d_type & DT_DIR) ){
		continue;
	    }else{
		buf[bufPos++] = 1;
	    }

	    fileCount++;
		    
	    FS.sprintf( buf+bufPos, "%s/%s", path, e->d_name );
	    bufPos += fileLen;
	}

	flushDirIndex( buf, bufPos );
	
    }

    FS.fclose(fp);
    FS.closedir(d);

    fillRect(0,0,width,height,api.clearColor);
    
    if( fileCount == 0 ){
	api.run = empty;
	lcdRefresh();
	cursor_x = 20;
	cursor_y = 40;
	print("Empty Folder");
	DIRECT::setPixel(220-1, 176-1, 0);
    }else{
	api.run = update;
	selection = 0;
	fileIndex = 0;
	for( uint32_t i=0; i<maxitem; ++i )
	    items[i].off = ~0;
	forceDraw = true;
    }

}

void init( Kernel *kapi ){
    ::kapi = kapi;
    path[0] = 0;
    font = fontTIC806x6;
    cursor_x = 0;
    cursor_y = 10;
    drawcolor = 7;
    x = -1;
    initdir();
}

namespace NAVIGATOR {
    
    API api = {
	init, 0, 0,
    
	0, // clearColor
	7,
    
	42, 6, 42, 95, // itemStrideX, itemOffsetX, itemStrideY, itemOffsetY

	-5, -5, // moveX, moveY
    
    };

}
