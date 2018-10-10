#include <SDFileSystem.h>
#include "fsapi.h"

extern "C" {

    void *__wrap__sbrk( int incr )
    {
	extern char   _pvHeapStart; /* Set by linker.  */
	static char * heap_end;
	char *        prev_heap_end;

	if (heap_end == 0)
	    heap_end = & _pvHeapStart;

	prev_heap_end = heap_end;
	heap_end += incr;

	return (void *) prev_heap_end;
    }
    
    extern unsigned int __data_section_table;
    extern unsigned int __data_section_table_end;
    extern unsigned int __bss_section_table;
    extern unsigned int __bss_section_table_end;
    void data_init(unsigned int romstart, unsigned int start, unsigned int len);
    void bss_init(unsigned int start, unsigned int len);
    void __libc_init_array(void);
    
}

extern SDFileSystem *sdfs;

uint32_t init(){
    LPC_SYSCON->SYSAHBCLKCTRL |= 1<<26;

    unsigned int LoadAddr, ExeAddr, SectionLen;
    unsigned int *SectionTableAddr;

    SectionTableAddr = &__data_section_table;

    while (SectionTableAddr < &__data_section_table_end) {
        LoadAddr = *SectionTableAddr++;
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        data_init(LoadAddr, ExeAddr, SectionLen);
    }
    while (SectionTableAddr < &__bss_section_table_end) {
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        bss_init(ExeAddr, SectionLen);
    }

    __libc_init_array();

    sdfs = new	SDFileSystem( P0_9, P0_8, P0_6, P0_7, "sd" );
    
    return 1;
}


bool load(){
    FS.init();

    FILE *fp = FS.fopen(".loader/boot.pop", "rb");
    if( !fp )
	return false;

    uint32_t addr = 0x10000100;
    FS.fread( (void *) addr, 1, 0x2000-0x100, fp );
    
    FS.fclose(fp);
    return true;
}

int main () {

    // __enable_irq();
    // SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; //enable systick

    if( !load() )
	while(true){;}


    {
	uint32_t SP = 0x10008000;
	asm volatile("mov sp, %[SP]" : : [SP] "l" (SP) : "cc" );
	((int (*)(void))0x10000101)();
    }
}

