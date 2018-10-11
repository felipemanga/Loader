#include <stdio.h>
#include <functional>
#include "kernel.h"
#include "tagtype.h"
#include "api.h"
#include "mode2_impl.h"
#include "directprint.h"
#include "drawCharDirect.h"
#include "miniprint_impl.h"
#include "fontTIC80.h"
#include "loader.h"
#include "strutil.h"

using namespace NAVIGATOR;

Kernel *kapi;

int32_t x, y, tx, ty;
uint8_t hlcolor;
char path[256];
const uint8_t maxitem = 8;
const char *errmsg;

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

struct Item {
    uint8_t icon[ 24*12 ];
    char name[ 25 ];
    char ext[4];
    uint32_t nameHash, off;
    bool hasName, isDir, isIconLoaded, isIconValid;

    void load( uint32_t off ){
	char buf[256];
	if( off != this->off ){
	    hasName = false;
	    isDir = false;
	    isIconLoaded = false;
	    isIconValid = true;
	    this->off = off;
	    name[0] = 0;
	}

	if( !isIconValid  )
	    return;

	if( !hasName || !isIconLoaded ){
	    hasName = true;

	    FILE *fp = FS.fopen(".loader/index", "rb");

	    if( fp ){

		FS.fseek( fp, off * 512, SEEK_SET );
		char *namep;

		buf[0] = 0;

		FS.fread( buf, 1, 256, fp );
		FS.fclose( fp );

		getExtension( ext, buf+1 );

		namep = buf+1;
		while( *namep++ );
		while( namep != buf+1 && *namep != '/' ) namep--;
		if( *namep=='/' ) namep++;
		uint32_t i;
		for( i=0; i<24 && *namep && *namep != '.'; ++i )
		    name[i] = *namep++;
		name[i] = 0;

		nameHash = hash(name);
	    
		isDir = buf[0];
	    }
	    
	}

	if( !isIconValid || isIconLoaded )
	    return;

	FILE *ifp;
	if( isDir ){
	    
	    concat(buf+1, "/icon.16c");
	    ifp = FS.fopen(buf+1, "r");
	    
	}else{
	    
	    Loader *loader;
	    char plugin[ 40 ];
	    FS.sprintf( plugin, ".loader/loaders/%s.pop", ext );
	    loader  = (Loader *) kapi->createProcess( plugin ).api;
	    
	    if( loader )
		ifp = loader->getIcon( buf+1, nameHash );
	    else
		return;
	    
	}

	if( ifp ){
	    FS.fread( icon, 1, sizeof(icon), ifp );
	    FS.fclose( ifp );
	    isIconLoaded = true;
	}else{
	    isIconValid = false;
	}
	
    }
    
} items[maxitem];

int32_t fileCount, fileIndex;
int32_t selection;

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

bool forceDraw;

bool draw( Kernel *kapi ){
    bool ret = stopped();

    if( ret && !forceDraw )
	return false;
    
    forceDraw = false;

    fillRect( 0, 0, width, height, api.clearColor );

    if( x < tx ) x++;
    else if( x > tx ) x--;
    if( y < ty ) y++;
    else if( y > ty ) y--;
    
    fillRect(
	x,  y,
	26, 26,
	hlcolor++
	);
    
    for( int i=0, c=fileIndex; i<maxitem; ++i, c=next(c) ){
	Item &item = items[i];
	int col = i&3, row = i>>2;	
	
	item.load( c );

	if( item.isIconLoaded ){
	    
	    drawBitmap(
		col*26+api.itemOffsetX,
		row*26+api.itemOffsetY,
		24, 24, item.icon
		);
	    
	}else{
	    
	    fillRect(
		col*26+api.itemOffsetX,
		row*26+api.itemOffsetY,
		24, 24,
		0
		);

	}
	
    }

    DIRECT::setPixel(220-1, 176-1, 0);
    lcdRefresh();

    if( !ret ){

	if( stopped() || forceDraw ){

	    Item &item = items[selection];

	    cursor_x = 110;
	    cursor_y = 2;

	    char *c = item.name;
	    while( *c++ )
		cursor_x -= font[0]>>1;
	
	    if( item.isDir ){
		print("[");
		print(item.name);
		print("]");
	    }else{
		print(item.name);
	    }
	
	}


    }

    return ret;
}

bool shiftRight;
bool startSelection( Kernel *kapi ){
    kapi->createProcess( path );
    return true;
}

void empty( Kernel *kapi ){
    
    if( isPressedB() ){
	while( isPressedB() );
	
	kapi->createProcess(".loader/desktop.pop");
    }
    
}

void update( Kernel *kapi ){
    
    if( stopped() ){
	
	if( isPressedB() ){
	    
	    while( isPressedB() );

	    if( !popPath() ){
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
	
	tx = (selection&0x3) * 26 + api.itemOffsetX - 1;
	ty = (selection>>2) * 26 + api.itemOffsetY - 1;
	
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
	char *buf = (char *) screenbuffer;
	uint32_t bufPos = 0, pathLen = strlen(path)+2, fileLen;

	FS.fclose(fp);

	while( dirent *e = FS.readdir( d ) ){

	    if( e->d_name[0] == '.' )
		continue;

	    fileLen = pathLen + strlen(e->d_name);

	    if( bufPos + fileLen >= sizeof(screenbuffer) ){
		flushDirIndex( buf, bufPos );
		bufPos = 0;
	    }
	    
	    {

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
		
	}

	flushDirIndex( buf, bufPos );
	
    }

    FS.fclose(fp);
    FS.closedir(d);

    if( fileCount == 0 ){
	api.run = empty;
	fillRect(0,0,width,height,api.clearColor);
	DIRECT::setPixel(220-1, 176-1, 0);
	lcdRefresh();
	cursor_x = 20;
	cursor_y = 40;
	print("Empty Folder");
    }else{
	api.run = update;
	selection = 0;
	fileIndex = 0;
	x = (selection&0x3) * 26 + api.itemOffsetX - 1;
	y = (selection>>2) * 26 + api.itemOffsetY - 1;
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
    
	    7, 4,
    
	    5, 4, 2, 36, // itemStrideX, itemOffsetX, itemStrideY, itemOffsetY

	    -5, -5, // moveX, moveY
    
	};

    }
