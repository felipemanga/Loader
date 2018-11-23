#include "loader.h"
#include "tagtype.h"
#include "directprint.h"


struct {
    uint32_t id, len;
} tag;

FILE *f;

bool findTag( const char *fileName, uint32_t targetTag ){
    f = FS.fopen( fileName, "r" );
    if( !f )
	return false;

    while( FS.fread( &tag, sizeof(tag), 1, f ) ){

	if( tag.id == targetTag ){
	    return true;
	}else if( tag.id == TAG_CODE ){
	    break;
	}else{
	    FS.fseek( f, tag.len, SEEK_CUR );
	}
	
    }

    FS.fclose(f);
    return false;
}

bool getIcon( const char *fileName, uint8_t *buf, uint32_t size ){
    uint32_t blen = size * (size>>1), targetTag;
    if( size == 24 ){
        targetTag = TAG_IMG_24X24_4BPP;
    }else if( size == 36 ){
        targetTag = TAG_IMG_36X36_4BPP;
    }else
	return false;
    
    if( !findTag( fileName, targetTag ) )
	return false;
    
    FS.fread( buf, 1, blen, f );
    FS.fclose( f );
    return true;
    
}


bool drawScreenshot(
    const char *fileName,
    uint32_t offset,
    uint32_t lineCount
    ){
    
    if( !findTag( fileName, TAG_IMG_220X176_565 ) )
	return false;

    uint16_t line[220];

    if( offset )
	FS.fseek( f, sizeof(line)*offset, SEEK_CUR );

    bool prevstate = true;

    while( lineCount-- ){
	FS.fread( line, 2, 220, f );
	for( uint32_t x=0; x<220; ++x ){
	    DIRECT::write_data(line[x]);
	}
	
	bool state = isPressedAny();
	if( state && !prevstate )
	    break;
	prevstate = state;
    }

    FS.fclose(f);

    return true;
}

bool drawIconDirect(
    const char *fileName,
    uint32_t size,
    int32_t x, int32_t y
    ){

    uint32_t targetTag;
    
    if( size == 24 ){
        targetTag = TAG_IMG_24X24_565;
    }else if( size == 36 ){
        targetTag = TAG_IMG_36X36_565;
    }else
	return false;

    if( !findTag( fileName, targetTag ) )
	return false;
    
    uint16_t buf[ 36 ];
    char h = size;
    while( h-- ){
	DIRECT::goToXY(x, y++);
	FS.fread( buf, 2, size, f );
	for( uint32_t x=0; x<size; ++x ){
	    DIRECT::write_data(buf[x]);
	}
    }

    FS.fclose( f );

    return true;
}


void activate( Kernel *kapi ){

    kapi->createProcess( (char *) kapi->ipcbuffer );

}
