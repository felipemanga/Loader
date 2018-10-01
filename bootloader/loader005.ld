/*Based on following file*/
/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-13
 * (c) NXP Semiconductors 2013-2014
 * Generated linker script file for LPC11U68
 * Created from LibIncTemplate.ld (LPCXpresso v7.2 (0 [Build 153] [2014-05-19] ))
 * By LPCXpresso v7.2.0 [Build 153] [2014-05-19]  on Sat Jun 14 15:26:54 JST 2014
 */

MEMORY
{
  /* Define each memory region */
  /* ORIGIN should be 0x39000*/
  MFlash256 (rx) : ORIGIN = 0x39000, LENGTH = 0x40000-0x39000-16*4 /* 256K bytes */
  BootableBlock (rx) : ORIGIN = 0x40000 - 16*4, LENGTH = 16*4

  Ram0_32 (rwx) : ORIGIN = 0x20004000, LENGTH = 0x800
}

  /* Define a symbol for the top of each memory region */
  __top_MFlash256 = 0x0 + 0x40000 - 0x7000 - 0xC;

/* * /
  __top_Ram0_32 = 0x10000000 + 0x8000 - 0x20;
/* */

ENTRY(ResetISR)

SECTIONS
{
    /* MAIN TEXT SECTION */    
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))

        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */
        

        *(.after_vectors*)
        
        *(.text*)
        *(.rodata .rodata.*)
        . = ALIGN(4);
        
        /* C++ constructors etc */
        . = ALIGN(4);
        KEEP(*(.init))
        
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;
        
        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;
        
        KEEP(*(.fini));
        
        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        
        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))
    } > MFlash256

    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this. 
     */
    .ARM.extab : ALIGN(4)
    {
    	*(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MFlash256
    __exidx_start = .;
    
    .ARM.exidx : ALIGN(4)
    {
    	*(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > MFlash256
    __exidx_end = .;
    
    _etext = .;
        
    /* BootableBlock section */
    .bootable : ALIGN(4)
    {	
	*(.bootable*);
	} > BootableBlock
	    
    /* MAIN DATA SECTION */
    
    /* Default MTB section */
    .mtb_buffer_default (NOLOAD) :
    {
       KEEP(*(.mtb*))
    } > Ram0_32 

    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > Ram0_32

	
	/* Main DATA section (Ram0_32) */
	.data : ALIGN(4)
	{
	   FILL(0xff)
	   _data = . ;
	   *(vtable)
	   *(.ramfunc*)
	   *(.data*)
	   . = ALIGN(4) ;
	   _edata = . ;
	} > Ram0_32 AT>MFlash256

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
        __end__ = .;
    } > Ram0_32
    
    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*) 
         . = ALIGN(4) ;
        _end_noinit = .;
    } > Ram0_32
    
    PROVIDE(_pvHeapStart = 0x20000000 );
    PROVIDE(_vStackTop = 0x10008000 - 0xC );
    PROVIDE(__valid_user_code_checksum = 0 - (
               _vStackTop +
           (ResetISR + 1) +
           (NMI_Handler + 1) +
           (HardFault_Handler + 1)
    ) );
}
