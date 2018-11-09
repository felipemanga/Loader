#include <stdio.h>
#include "tagtype.h"
#include "iap.h"
#include "kernel.h"

#define EESETTINGS_SKIPINTRO	    4020 // 0xFB4 Show Logo (0-Yes, 1-No, 2-Ye

#define BAD_PTR ((void *) ~0)
#define MAX_PID 8
#define BAD_PID 0xFF

struct ProcessInstance {
    Process *api;
    uint32_t hash;
    uint32_t pid;
} processes[MAX_PID];
uint32_t nextPID;
uint8_t procmap[32];
uint8_t ipcbuffer[256];
uint32_t killedpid;

uint32_t hash( const char *str ){
    uint32_t h = 5381;
    while( *str )
	h = ((h << 5) + h) + *str++;
    return h;
}

uint32_t loadPOP( const char *fileName, uint32_t parentPID );

ProcessHandle createProcess( const char *path );
void killProcess( uint32_t pid );
bool isAlive( uint32_t pid );

Kernel _kapi = {
    ipcbuffer,
    createProcess,
    killProcess,
    isAlive
};

Kernel *kapi = &_kapi;
uint32_t callerPID;

bool isAlive( uint32_t pid ){
    return ((pid & 0xFF) < MAX_PID) && processes[ pid&0xFF ].pid == pid;
}

void killProcess( uint32_t lpid ){

    uint8_t pid = lpid & 0xFF;
    if( pid >= MAX_PID || processes[pid].pid != lpid )
	return;

    DBG(12);
    DBG(lpid);

    if( callerPID == lpid )
	callerPID = BAD_PID;

    if( processes[ pid ].api ){
	for( uint32_t j=0; j<MAX_PID; ++j ){
	    if( processes[j].api && processes[j].api->parentPid == lpid )
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
	return ProcessHandle{ procToLoadState, nullptr };

    uint32_t i;
    for( i=0; i<MAX_PID; ++i )
	if( h == processes[i].hash && (processes[i].pid&0xFF) != BAD_PID )
	    return ProcessHandle{ ProcessState::ready, processes[i].api };

    for( i=0; i<256 && (procToLoad[i] = path[i]); ++i ){;}

    if( i == 256 )
	return ProcessHandle{ ProcessState::error, nullptr };

    procToLoadHash = h;
    procToLoadState = ProcessState::pending;
    
    return ProcessHandle{ procToLoadState, nullptr };
}

void loop(){
    
    while( true ){

	uint32_t live = 0;
	for( uint32_t i=0; i<MAX_PID; ++i ){
	    if( processes[i].api && processes[i].api->run ){
		live = 1;
		processes[i].api->run( kapi );

		if( procToLoadState == ProcessState::pending ){

		    FS.init();
		    uint32_t pid = loadPOP( procToLoad, processes[i].pid );

		    if( (pid & 0xFF) != BAD_PID ){
			procToLoadState = ProcessState::ready;
			DBG(11);
			DBG(pid);
		    }else{
			procToLoadState = ProcessState::error;
			DBG(15);
		    }
	    
		}
		
	    }
	}
    
	if( !live ){
	    FS.init();

	    if( ( loadPOP( ".loader/desktop.pop", BAD_PID )&0xFF) == BAD_PID ){
	    break;
	    }
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
    uint32_t readCount, start, end, size = count;
    Process *progress = nullptr;
    Progress ipcbuffer;

    if( addr >= 0x10000000 ){
	start = addr - 0x10000000;
	end = start + count;
	if( end & 0x3FF )
	    end += 0x400;
	start >>= 10;
	end >>= 10;
    }else{

	if( count == ~0UL ){
	    start = FS.ftell(f);
	    FS.fseek( f, 0, SEEK_END );
	    size = FS.ftell( f ) - start;
	    FS.fseek( f, start, SEEK_SET );
	}
	
	start = 0;
	end = 0;	
	ipcbuffer.total  = size;
	ipcbuffer.copied = 0;
	ipcbuffer.hash = procToLoadHash;
	kapi->ipcbuffer = &ipcbuffer;
	
	uint32_t pid = loadPOP(".loader/progress.pop", BAD_PID) & 0xFF;
	
	if( pid != BAD_PID )
	    progress = processes[pid].api;

    }

    while( count ){

	if( progress )
	    progress->run( kapi );
	
	if( addr < 0x10000000 ){
	    __attribute__ ((aligned)) char b[0x400];
	    readCount = FS.fread( b, 1, sizeof(b), f );
	    if( !readCount ) break;
	    CopyPageToFlash( addr, (uint8_t*) b, sizeof(b) );
	}else{
	    readCount = FS.fread( (char *) addr, 1, count, f );
	}
	
	if( !readCount ) break;
	ipcbuffer.copied += readCount;
	count -= readCount;
	addr += readCount;
	
    }

    if( progress ){
	ipcbuffer.copied = ipcbuffer.total;
	progress->run( kapi );
    }

    for( uint32_t i=start; i<end; ++i ){
	uint32_t other = procmap[i];
	if( other != BAD_PID )
	    killProcess( processes[ procmap[i] ].pid );
	procmap[i] = pid;
    }
    
}

uint32_t loadPOP( const char *fileName, uint32_t parentPID ){
    callerPID = parentPID;

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
	    FS.fseek( f, -8, SEEK_CUR );
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

    if( !canExec ){
	DBG(0);
	return BAD_PID;
    }

    if( targetAddr == 0 ){
	uint8_t introFlag = 3;
	writeEEPROM((uint16_t *)EESETTINGS_SKIPINTRO, &introFlag, 1);
	
	*((uint32_t*)0xE000ED0C) = 0x05FA0004; //issue system reset
    }
    
    if( targetAddr < 0x10000000 ){
	DBG(18);
	DBG(targetAddr);
	return BAD_PID;
    }

    using callType = uint32_t (*)( Kernel *, Process * );
    Process *callerAPI = nullptr;

    if( (callerPID&0xFF) != BAD_PID )
	callerAPI = processes[callerPID & 0xFF].api;

    targetAddr |= 1;
    callType call = (callType) targetAddr;
    processes[pid].api = (Process *) call( kapi, callerAPI );

    uint32_t lpid = pid + ((++nextPID)<<8);
    
    if( processes[pid].api ){
	processes[pid].hash = h;
	processes[pid].pid = lpid;
	processes[pid].api->parentPid = callerPID;
	processes[pid].api->pid = lpid;
    }
	
    return lpid;
}

