/*
Linker script for STM32F10x_128K_8K

modified from

http://www.codesourcery.com/archives/arm-gnu/msg02972.html
*/

/*
There will be a link error if there is not this amount of RAM free at the
end.
*/
_Minimum_Stack_Size = 1024;

ENTRY(main)


/* Memory Spaces Definitions */

MEMORY
{
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 112K
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1M
}

/* highest address of the user mode stack */

_estack = 0x2001c000;

PROVIDE ( _Stack_Limit = _estack - _Minimum_Stack_Size );
PROVIDE(__cs3_stack = __cs3_region_start_ram + LENGTH(RAM));

/* Sections Definitions */

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))            /* Startup code */
        *(.text)                   /* code */
        *(.text.*)                 /* remaining code */
        *(.rodata)                 /* read-only data (constants) */
        *(.rodata.*)
        *(.glue_7)
        *(.glue_7t)
        *(.vfp11_veneer)
        *(.v4_bx)
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } >FLASH

    /* for exception handling/unwind - some Newlib functions (in
    common with C++ and STDC++) use this. */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)

    } > FLASH

     __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
        __exidx_end = .;

    . = ALIGN(4);
     _etext = .;
    /* This is used by the startup in order to initialize the .data secion */
    _sidata = _etext;

    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH (inidata).
    It is one task of the startup to copy the initial values from FLASH to RAM. */
    .data  : AT ( _sidata )
    {
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data secion */
        _sdata = . ;

        *(.data)
        *(.data.*)

        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data secion */
        _edata = . ;
    } >RAM


    /* This is the uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss secion */
        _sbss = .;
	__bss_start__ = _sbss;
        *(.bss)
        *(.bss.*)
        *(COMMON)

        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss secion */
        _ebss = . ;
	__bss_end__ = _ebss;
    } >RAM

    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );
    PROVIDE ( _exit = _ebss );
    /* This is the user stack section
    This is just to check that there is enough RAM left for the User mode stack
    It should generate an error if it's full.
     */
    ._usrstack :
    {
        . = ALIGN(4);
        _susrstack = . ;

        . = . + _Minimum_Stack_Size ;

        . = ALIGN(4);
        _eusrstack = . ;
    } >RAM
}
