#include "loader.h"
#include "../ldrDER/grammar.h"
#include "../ldrDER/drawicon.h"
#include "strutil.h"
#include "ships.h"

bool getIcon( const char *fileName, uint8_t *buf, uint32_t size ){
    
    initIcon( (char *) buf, size );    
    initGrammar( hash(fileName), size>>1, 5 );    
    mirror();

    return true;

}

void activate( Kernel *kapi ){

    kapi->createProcess( (char *) kapi->ipcbuffer );

}

