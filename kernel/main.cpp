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
uint32_t nextPID;
uint8_t procmap[32];
uint8_t ipcbuffer[256];
char b[256];
uint32_t killedpid;

uint32_t hash( const char *str ){
    uint32_t h = 5381;
    while( *str )
	h = ((h << 5) + h) + *str++;
    return h;
}

uint8_t loadPOP( const char *fileName, uint32_t parentPID );

ProcessHandle createProcess( const char *path );
void killProcess( uint32_t pid );
bool isAlive( uint32_t pid );

KAPI _kapi = {
    ipcbuffer,
    createProcess,
    killProcess,
    isAlive
};

KAPI *kapi = &_kapi;
uint32_t callerPID;

bool isAlive( uint32_t pid ){
    return ((pid & 0xFF) < MAX_PID) && processes[ pid&0xFF ].pid == pid;
}

void killProcess( uint32_t lpid ){

    uint8_t pid = lpid & 0xFF;
    if( lpid >= MAX_PID || processes[lpid].pid != lpid )
	return;

    DBG(12);
    DBG(lpid);

    if( callerPID == processes[ pid ].pid )
	callerPID = BAD_PID;

    if( processes[ pid ].api ){
	for( uint32_t j=0; j<MAX_PID; ++j ){
	    if( processes[j].api->parentPid == lpid )
		killProcess(processes[j].pid);
	}
    }
    
    processes[ pid ].api = nullptr;
    processes[ pid ].pid = BAD_PID;
    
    for( uint32_t j=0; j<32; ++j ){
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
	    if( processes[i].api && processes[i].api->run ){
		live = 1;
		void *api = processes[i].api;
		processes[i].api->run( kapi );

		if( procToLoadState == ProcessState::pending ){

		    uint8_t pid = loadPOP( procToLoad, processes[i].pid );

		    if( pid != BAD_PID ){
			procToLoadState = ProcessState::ready;
			DBG(11);
			DBG(pid);
		    }else{
			procToLoadState = ProcessState::error;
			DBG(12);
		    }
	    
		}
		
	    }
	}
    
	if( !live && loadPOP( "loader/desktop.pop", BAD_PID ) == BAD_PID ){
	    break;
	}

    }

    *((uint32_t*)0xE000ED0C) = 0x05FA0004; //issue system reset

}

int main () {
    for( uint32_t i=0; i<32; ++i )
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
	uint32_t other = procmap[i];
	if( other != BAD_PID )
	    killProcess( processes[ procmap[i] ].pid );
	procmap[i] = pid;
    }
    
}

uint8_t loadPOP( const char *fileName, uint32_t parentPID ){
    callerPID = parentPID;

    FS.init("sd");
    FILE *f = FS.fopen( fileName, "rb" );
    if( !f ){
	DBG(13);
	return BAD_PID;
    }

    uint32_t pid = 0;
    while( processes[pid].api && pid < MAX_PID )
	pid++;

    if( pid == MAX_PID ){
	DBG(18);
	return BAD_PID;
    }

    uint32_t h = hash( fileName );

    uint32_t targetAddr = 0, canExec = false;
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
	    canExec = true;
	    break;
	    
	case TAG_CODECHUNK:
	{
	    if( FS.fread( &targetAddr, 4, 1, f ) != 4 ){
		DBG(14);
		FS.fclose(f);
		return BAD_PID;
	    }
	    copyCode( targetAddr, tag.size-4, f, pid );
	    canExec = true;
	    break;
	}
	
	default:
	    FS.fseek( f, tag.size, SEEK_CUR );
	    break;
	}
    }

    FS.fclose(f);

    if( !canExec )
	return BAD_PID;

    if( targetAddr == 0 )
	*((uint32_t*)0xE000ED0C) = 0x05FA0004; //issue system reset
    
    if( targetAddr < 0x10000000 ){
	DBG(18);
	DBG(targetAddr);
	return BAD_PID;
    }

    using callType = uint32_t (*)( KAPI *, PAPI * );
    PAPI *callerAPI = nullptr;

    if( (callerPID&0xFF) != BAD_PID )
	callerAPI = processes[callerPID & 0xFF].api;

    targetAddr |= 1;
    callType call = (callType) targetAddr;
    processes[pid].api = (PAPI *) call( kapi, callerAPI );
	
    if( processes[pid].api ){
	processes[pid].hash = h;
	uint32_t lpid = pid + ((++nextPID)<<8);
	processes[pid].pid = lpid;
	processes[pid].api->parentPid = callerPID;
	processes[pid].api->pid = lpid;
    }
	
    return pid;
}

