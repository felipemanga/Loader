#include "loader.h"
#include "grammar.h"
#include "drawicon.h"
#include "ships.h"

bool getIcon( const char *fileName, u32 hash, uint8_t *buf ){    

    initIcon( (char *) buf );
    
    initGrammar( hash, 12, 5 );

    u32 y = 0;
    for( u32 i=0; i<bufheight; i++, y += 12 ){
	for( u32 x=0; x<6; x++ ){
	    buf[y + 11 - x] = (buf[y + x]>>4) | (buf[y + x]<<4);
	}
    }

    return true;
    
}


