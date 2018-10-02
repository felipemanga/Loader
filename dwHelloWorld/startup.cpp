#include "../kernel/kapi.h"

namespace DT {
    #include "../desktop/api.h"
}

#include "api.h"

extern API api;

void setup( DT::API * );

extern "C" {

    void __libc_init_array(void);

    __attribute__ ((section(".after_vectors")))
    PAPI *__onLoad( DT::API *dtapi ){
	DBG(dtapi);
	// __libc_init_array();
	// setup( dtapi );
	return &api;
    }

}
