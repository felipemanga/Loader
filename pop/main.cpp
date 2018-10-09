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
	}

	if( !hasName || !isIconLoaded ){
	    hasName = true;

	    FILE *fp = FS.fopen("loader/index", "rb");

	    if( fp ){
		DBG( 1111 );

		FS.fseek( fp, off * 256, SEEK_SET );
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
		for( i=0; i<24 && *namep; ++i )
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
	    FS.sprintf( plugin, "loader/loaders/%s.pop", ext );
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

    FS.seekdir(d, items[selection].off );
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

bool draw( Kernel *kapi ){
    bool ret = stopped(), redraw = false;

    if( ret )
	return false;

    fillRect(0,0,width,height,api.clearColor);
    DIRECT::setPixel(220-1, 176-1, 0);
    lcdRefresh();

    cursor_x = 0;
    cursor_y = 0;

    for( int i=0, c=fileIndex; i<maxitem; ++i, c=next(c) ){
	Item &item = items[i];
	int col = i&3, row = i>>2;	
	
	item.load( c );

	if( i == selection ){
	    fillRect(
		col*26+api.itemOffsetX-1,
		row*26+api.itemOffsetY-1,
		26, 26,
		hlcolor++
		);
	}

	if( item.isIconLoaded ){
	    
	    drawBitmap(
		col*26+api.itemOffsetX,
		row*26+api.itemOffsetY,
		24, 24, item.icon
		);
	    
	}else{
	    
	    if( item.isIconValid )
		return false;

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

	if( x < tx ) x++;
	else if( x > tx ) x--;
	if( x == tx ) x = tx = 0;
	if( y < ty ) y++;
	else if( y > ty ) y--;
	if( y == ty ) y = ty = 0;

	if( stopped() ){

	    if( redraw )
		x = -1;
	    
	    Item &item = items[selection];

	    cursor_x = 0;
	    cursor_y = 0;
	
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
	
	kapi->createProcess("loader/desktop.pop");
    }
    
}

void update( Kernel *kapi ){
    int32_t fileId, itemId;
    
    if( stopped() ){
	
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
	
	if( isPressedRight() || isPressedDown() ){
	    selection++;

	    if( selection >= (maxitem>>1) ){
		
		selection = 0;
		fileIndex += maxitem;
		while( fileIndex >= fileCount )
		    fileIndex -= fileCount;
		
	    }else{
		
		tx = api.moveX;
		ty = api.moveY;
		
	    }
	    
	}else if( isPressedLeft() || isPressedUp() ){
	    
	    selection--;
	    if( selection < 0 ){
		
		selection = maxitem-1;
		
	    }else{
		
		x = api.moveX;
		y = api.moveY;
		
	    }

	}
	
    }

    draw( kapi );
    
}

    void initdir(){
	
	DIR *d = FS.opendir( path );
	FILE *fp = FS.fopen( "loader/index", "w" );

	fileCount = 0;

	if( fp && d ){

	    FS.fclose(fp);

	    while( dirent *e = FS.readdir( d ) ){

		if( e->d_name[0] == '.' )
		    continue;

		{
		    char *buf = (char *) screenbuffer;
		    buf[0] = 1;

		    if( e->d_type & DT_ARC ){
			char ext[4];
			getExtension( ext, e->d_name );

			FS.sprintf( buf, "loader/loaders/%s.pop", ext );
			FILE *f = FS.fopen( buf, "r" );
			if( f ) FS.fclose(f);
			else continue;
			buf[0] = 0;

		    }else if( !(e->d_type & DT_DIR) ){
			continue;
		    }

		    fileCount++;
		    FS.sprintf( buf+1, "%s/%s", path, e->d_name );
		    FILE *fp = FS.fopen( "loader/index", "a" );
		    if( fp ) FS.fwrite( buf, 1, 256, fp );
		    FS.fclose(fp);
		}
		
	    }	
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
    
	    5, 0, 2, 32, // itemStrideX, itemOffsetX, itemStrideY, itemOffsetY

	    -5, -5, // moveX, moveY
    
	};

    }
