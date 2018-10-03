#include "../kernel/kapi.h"
#include "api.h"

namespace DESKTOP{
    extern API api;
}

extern "C" {

    void __libc_init_array(void);

    __attribute__ ((section(".after_vectors")))
    PAPI *onLoad( KAPI *kapi ){
	__libc_init_array();
	return &DESKTOP::api;
    }

}
