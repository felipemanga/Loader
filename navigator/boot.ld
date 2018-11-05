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
  MFlash256 (rx) : ORIGIN = 0x10002000, LENGTH = 0x3000 /* 8K bytes */
}

ENTRY(onLoad)

SECTIONS
{
    /* MAIN TEXT SECTION */    
    .text : ALIGN(4)
    {
        FILL(0xff)
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
	KEEP(*(.mtb*))
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
	
	   FILL(0xff)
	   _data = . ;
	   *(vtable)
	   *(.ramfunc*)
	   *(.data*)
	   . = ALIGN(4) ;
	   _edata = . ;

	   _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
        __end__ = .;

	_noinit = .;
        *(.noinit*) 
         . = ALIGN(4) ;
        _end_noinit = .;
    } > MFlash256
    
    PROVIDE(_pvHeapStart = . );
    PROVIDE(_vStackTop = 0x10000000 + 0x8000 );
}
