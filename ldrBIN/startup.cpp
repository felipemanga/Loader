#include "kernel.h"
#include "loader.h"

Loader api = {
    nullptr, 0, 0,
    getIcon, activate
};

extern "C" {

    void __libc_init_array(void);

    __attribute__ ((section(".after_vectors")))
    Loader *__onLoad( Kernel *kapi, Process *dtapi ){
	__libc_init_array();
	
	if( *((uint8_t*)kapi->ipcbuffer) )
	    activate( kapi );
	
	return &api;
    }

}
