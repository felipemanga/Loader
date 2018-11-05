#include "loader.h"
#include "grammar.h"
#include "drawicon.h"
#include "ships.h"
#include "../kernel/iap.h"

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

void copyCode( uint32_t addr, FILE *f ){
    uint32_t readCount;
    __attribute__ ((aligned)) char b[0x400];

    while( (readCount = FS.fread( b, 1, sizeof(b), f )) ){
	
	CopyPageToFlash( addr, (uint8_t*) b, sizeof(b) );
	addr += readCount;
	
    }
    
}

void fadeOut(){
}

void activate( Kernel *kapi ){
    uint32_t dest = 0x39000;
    uint32_t end = 0x40000;

    const char *fileName = (char *) kapi->ipcbuffer;
    FILE *f = FS.fopen("/.loader/firstBoot.bin", "r");

    if( !f )
	return;
    copyCode( 0, f );
    FS.fclose(f);

    f = FS.fopen( fileName, "r" );
    if( !f )
	return;
    uint32_t *src = (uint32_t *) 0x10000;
    copyCode( 0x10000, f );
    FS.fclose(f);

    for( ; dest<end; dest+=0x400 ){
	__attribute__ ((aligned)) uint32_t b[0x100];
	for( uint32_t i=0; i<0x100; ++i ){
	    b[i] = *src++;
	}
	CopyPageToFlash( dest, (uint8_t *) b, 0x400 );
    }

    *((uint32_t*)0xE000ED0C) = 0x05FA0004; //issue system reset
    
}

