#include "loader.h"
#include "tagtype.h"

FILE *getIcon( const char *fileName, uint32_t hash ){    
    FS.sprintf( buf, "loader/cache/%x.16c", hash );

    FILE *f = FS.fopen( buf, "r" );
    if( f )
	return f;
    
    f = FS.fopen( buf, "w+" );
    if( !f )
	return nullptr;

    char buf[24*12], *pos;
    pos = buf;
    for( int i=0; i<24*12; ++i ){
	hash ^= hash<<17;
	hash ^= hash>>13;
	hash ^= hash<<5;
	*pos++ = (char) hash;
    }

    FS.fwrite( buf, 1, sizeof(buf), f );

    FS.fseek( f, 0, SEEK_SET );
    
    return f;
    
}


