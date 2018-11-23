#pragma once

#include <cstdint>
#include "fsapi.h"

#define DBG(value)

// EMULATOR ONLY!
// #define DBG(value)	((volatile uint32_t *) 0x400483F4)[0] = uint32_t(value)

#ifndef WEAK
#define WEAK __attribute__ ((weak))
#endif

struct Kernel;

struct Process {
    void (*run)( Kernel * );
    uint32_t pid, parentPid;
};

enum class ProcessState {
    error,
    pending,
    ready
};

struct ProcessHandle {
    ProcessState state;
    Process *api;
};


struct Progress {
    uint32_t total;
    uint32_t copied;
    uint32_t hash;
};

struct Kernel {
    void *ipcbuffer;
    ProcessHandle (*createProcess)( const char *name );
    void (*killProcess)( uint32_t pid );
    bool (*isAlive)( uint32_t pid );
};

extern Kernel *kapi;

#define isPressedA() 	    ((volatile uint8_t *) 0xA0000020)[9]
#define isPressedB() 	    ((volatile uint8_t *) 0xA0000020)[4]
#define isPressedC() 	    ((volatile uint8_t *) 0xA0000020)[10]
#define isPressedD() 	    ((volatile uint8_t *) 0xA0000000)[1]
#define isPressedUp() 	    ((volatile uint8_t *) 0xA0000020)[13]
#define isPressedDown()     ((volatile uint8_t *) 0xA0000020)[3]
#define isPressedLeft()     ((volatile uint8_t *) 0xA0000020)[25]
#define isPressedRight()    ((volatile uint8_t *) 0xA0000020)[7]
#define isPressedAny() !!(				\
	isPressedA() ||					\
	isPressedB() ||					\
	isPressedC() ||					\
	isPressedUp() ||				\
	isPressedDown() ||				\
	isPressedLeft() ||				\
	isPressedRight()				\
)    
