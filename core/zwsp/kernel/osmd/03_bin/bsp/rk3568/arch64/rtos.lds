ENTRY(__text_start)

_stack_size = 0x10000;
_heap_size = 0x10000;

MEMORY
{
    RAM_IMG (rwx) : ORIGIN = 0x60000000, LENGTH = 0x800000
    RAM_IPC (rwx)  : ORIGIN = 0x70000000, LENGTH = 0x800000
}

SECTIONS
{
    /* 代码段 */
    __text_start = .;
    .text : 
	{
        obj/start.o
        *(.text)
    } > RAM_IMG

    . = ALIGN(8);
    .rodata :  
	{
        __rodata_start = .;
        *(.rodata)
        *(.rodata.*)

        . = ALIGN(8);
        __rodata_end = .;
    } > RAM_IMG

    . = ALIGN(8);
    .head (NOLOAD) : 
	{
        PROVIDE(__HEAP_INIT = .);
        . = . + _heap_size;

        . = ALIGN(8);
        PROVIDE (__HEAP_END = .);
    } > RAM_IMG


    . = ALIGN(8);
    .stack (NOLOAD) : 
	{
        PROVIDE ( __os_sys_sp_start = .);
        . = . + _stack_size;

        . = ALIGN(8);
        PROVIDE ( __os_sys_sp_end = .);
    } > RAM_IMG

    . = ALIGN(8);
    .data : 
	{
        PROVIDE(__data_start = .);
        *(.data)
		*(.data.*)

        . = ALIGN(8);
		PROVIDE(__data_end = .);
    } > RAM_IMG

	. = ALIGN(8);
    .bss (NOLOAD) : 
	{
        PROVIDE(__bss_start = .);
        *(.bss)
		*(.bss.*)

        . = ALIGN(8);
        PROVIDE(__bss_end = .);
    } > RAM_IMG
}
