#include "../kernel/kapi.h"
#include "../desktop/api.h"
#include "api.h"

WEAK void setup( DESKTOP::API * );
WEAK void loop( KAPI *kernel );
API api = { loop };

extern "C" {

    void __libc_init_array(void);

    __attribute__ ((section(".after_vectors")))
    PAPI *__onLoad( KAPI *kapi, DESKTOP::API *dtapi ){
	__libc_init_array();
	if( setup )
	    setup( dtapi );
	return &api;
    }

}
