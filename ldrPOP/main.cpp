#include "loader.h"
#include "tagtype.h"

FILE *getIcon( const char *fileName, uint32_t hash ){

    FILE *f = FS.fopen( fileName, "r" );
    
    if( !f )
	return nullptr;

    struct {
	uint32_t id, len;
    } tag;

    while( FS.fread( &tag, sizeof(tag), 1, f ) ){
	
	if( tag.id == TAG_IMG_36X36_4BPP ){
	    return f;
	}else if( tag.id == TAG_CODE ){
	    break;
	}else{
	    FS.fseek( f, tag.len, SEEK_CUR );
	}
	
    }

    FS.fclose(f);
    
    return nullptr;
    
}


