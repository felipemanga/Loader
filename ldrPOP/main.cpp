#include "loader.h"
#include "tagtype.h"

bool getIcon( const char *fileName, uint32_t hash, uint8_t *buf ){

    FILE *f = FS.fopen( fileName, "r" );
    
    if( !f )
	return false;

    struct {
	uint32_t id, len;
    } tag;

    while( FS.fread( &tag, sizeof(tag), 1, f ) ){
	
	if( tag.id == TAG_IMG_24X24_4BPP ){
	    FS.fread( buf, 1, 24*12, f );
	    FS.fclose( f );
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


