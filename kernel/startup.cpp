#include <cstdint>

int main();

extern "C" {
    
void __libc_init_array(void);

__attribute__ ((section(".after_vectors")))
uint32_t onLoad(){
    __libc_init_array();
    return main();
}

}
