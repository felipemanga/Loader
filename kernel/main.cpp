#include <stdio.h>
#include "tagtype.h"
#include "iap.h"
#include "kapi.h"

#define BAD_PTR ((void *) ~0)
#define MAX_PID 8
#define BAD_PID 0xFF

struct Process {
    PAPI *api;
    uint32_t hash;
    uint32_t pid;
} processes[MAX_PID];
uint32_t nextPID ;
uint8_t procmap[16];
uint8_t ipcbuffer[256];
char b[256];
uint32_t killedpid;
uint32_t activepid;

uint32_t getActivePID(){
    return activepid;
}

uint32_t hash( const char *str ){
    uint32_t h = 5381;
    while( *str )
	h = ((h << 5) + h) + *str++;
    return h;
}

void *loadPOP( const char *fileName, bool exec, void *arg );

ProcessHandle createProcess( const char *path );
void killProcess( uint32_t pid );

KAPI _kapi = {
    ipcbuffer,
    createProcess,
    killProcess,
    getActivePID
};

KAPI *kapi = &_kapi;

void killProcess( uint32_t pid ){

    if( pid == 0 ) return;

    pid &= (MAX_PID<<1) - 1;
    pid--;

    if( pid >= MAX_PID ) return;

    processes[ procmap[pid] ].api = nullptr;
    
    for( uint32_t j=0; j<16; ++j ){
	if( procmap[j] == pid ){
	    procmap[j] = BAD_PID;
	    killedpid = pid;
	}
    }

}

uint32_t procToLoadHash;
ProcessState procToLoadState;
char procToLoad[256];

ProcessHandle createProcess( const char *path ){
    uint32_t h = hash(path);

    if( h == procToLoadHash && procToLoadState != ProcessState::ready )
	return ProcessHandle{ procToLoadState };

    uint32_t i;
    for( i=0; i<MAX_PID; ++i )
	if( h == processes[i].hash )
	    return ProcessHandle{ ProcessState::ready, processes[i].api };

    for( i=0; i<256 && (procToLoad[i] = path[i]); ++i ){;}

    if( i == 256 )
	return ProcessHandle{ ProcessState::error };

    procToLoadHash = h;
    procToLoadState = ProcessState::pending;
    
    return ProcessHandle{ procToLoadState };
}

void loop(){
    
    while( true ){

	uint32_t live = 0;
	for( uint32_t i=0; i<MAX_PID; ++i ){
	    if( processes[i].api ){
		activepid = processes[i].pid;
		live = 1;
		processes[i].api->run( kapi );

		if( procToLoadState == ProcessState::pending ){

		    if( loadPOP( procToLoad, true, kapi ) ){
			procToLoadState = ProcessState::ready;
		    }else{
			procToLoadState = ProcessState::error;
		    }
	    
		}
		
	    }
	}
    
	if( !live && !loadPOP( "loader/desktop.pop", true, kapi ) ){
	    break;
	}

    }

    *((uint32_t*)0xE000ED0C) = 0x05FA0004; //issue system reset

}

int main () {
    for( uint32_t i=0; i<16; ++i )
	procmap[i] = BAD_PID;
    // ((uint32_t *)0xA0002000)[1] &= ~(1<<9);
    loop();
}

void copyCode( uint32_t addr, uint32_t count, FILE *f, uint8_t pid ){
    uint32_t readCount, start, end;

    if( addr >= 0x10000000 ){
	start = addr - 0x10000000;
	end = start + count;
	if( end & 0x3FF )
	    end += 0x400;
	start >>= 10;
	end >>= 10;
    }else{
	start = 0;
	end = 0;
    }

    while( count ){
	
	if( addr < 0x10000000 ){
	    readCount = FS.fread( b, 1, 256, f );
	    if( !readCount ) break;
	    CopyPageToFlash( addr, (uint8_t*) b );
	}else{
	    readCount = FS.fread( (char *) addr, 1, count, f );
	}
	
	if( !readCount ) break;
	count -= readCount;
	addr += readCount;
	
    }

    for( uint32_t i=start; i<end; ++i ){
	killProcess( procmap[i] );
	procmap[i] = pid;
    }
    
}

void *loadPOP( const char *fileName, bool exec, void *arg ){
    FS.init("sd");
    FILE *f = FS.fopen( fileName, "rb" );
    if( !f ) return NULL;

    uint32_t pid = 0;
    while( processes[pid].api && pid < MAX_PID )
	pid++;

    uint32_t h = hash( fileName );

    uint32_t targetAddr = 0;
    while( !FS.feof(f) ){
	
	struct{
	    uint32_t id, size;
	} tag;
	
	if( !FS.fread( &tag, 4, 2, f ) )
	    break;

	switch( tag.id ){
	case TAG_OFFSETADDRESS:
	    FS.fread( &targetAddr, 4, 1, f );
	    break;
	    
	case TAG_CODE:
	    FS.fseek( f, -16, SEEK_CUR );
	    copyCode( targetAddr, -1, f, pid );
	    break;
	    
	case TAG_CODECHUNK:
	{
	    FS.fread( &targetAddr, 4, 1, f );
	    copyCode( targetAddr, tag.size-4, f, pid );
	    break;
	}
	
	default:
	    FS.fseek( f, tag.size, SEEK_CUR );
	    break;
	}
    }

    FS.fclose(f);

    if( exec && targetAddr == 0 )
	*((uint32_t*)0xE000ED0C) = 0x05FA0004; //issue system reset
    
    if( exec ){
	targetAddr |= 1;
	using callType = uint32_t (*)( void * );
	callType call = (callType) targetAddr;
	targetAddr = call(arg);
	processes[pid].api = (PAPI *) targetAddr;
	processes[pid].hash = h;
	processes[pid].pid = pid + 1 + (nextPID++ << 4);
    }

    return (void *) targetAddr;
}

