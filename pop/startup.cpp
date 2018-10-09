#include "kernel.h"
#include "api.h"

namespace NAVIGATOR{
    extern API api;
}

extern "C" {

    void __libc_init_array(void);

    __attribute__ ((section(".after_vectors")))
    Process *onLoad( Kernel *kapi ){
	__libc_init_array();
	return &NAVIGATOR::api;
    }

}
