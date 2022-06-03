
OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)

ENTRY(_start)


MEMORY {
  rom : ORIGIN = DEFINED(ROM_ORIGIN) ? ROM_ORIGIN : 0x00000000, LENGTH = DEFINED(ROM_ORIGIN) ? 32K : 8K
  ram : ORIGIN = 0x02000000, LENGTH = 16K
}

STACK_SIZE = 1024;

CL_SIZE = 16;

/*__boot_START__ = 0xff;*/

SECTIONS {

	.text  : {
        PROVIDE(__TEXT_START__ = .);
        *crt0.o(.text .text.*)
		PROVIDE(__crt0_END__ = .);
		PROVIDE(__boot_START__ = .);
		/*. = __boot_START__;*/
		__boot_START__ = .;
		*(.boot)
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)      
        . = ALIGN(CL_SIZE);
        PROVIDE(__TEXT_END__ = .);
    } >rom 
	
	/* .boot 0xbd0 : {
		PROVIDE(__boot_START__ = .);
		*crt0.o(.text .text.*)
        *(.text)
        *(.text.*)
        *(.boot)
        *(.boot.*)  
		*(.rodata)
        *(.rodata.*) 
        . = ALIGN(CL_SIZE);
        PROVIDE(__boot_END__ = .);
    } >rom */ 
	
    .ramtext  : {
        PROVIDE(__RAMTEXT_START__ = .);
        *(.ramtext)
        *(.ramtext.*)
        *xprintf.o(.text .text.*)
        *(.ramrodata)
        *(.ramrodata.*)      
        . = ALIGN(CL_SIZE);
    } >ram AT >rom

    __RAMTEXT_IMAGE_START__ = LOADADDR(.ramtext);
    __RAMTEXT_IMAGE_END__ = LOADADDR(.ramtext) + SIZEOF(.ramtext);
    ASSERT(__RAMTEXT_IMAGE_END__ < ORIGIN(rom) + LENGTH(rom), "RAMcode image overflows rom section")

    .data : 
    {
        PROVIDE(__DATA_START__ = .);
        _gp = .;
        *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata*)
        *(.sdata .sdata.* .gnu.linkonce.s.*)
        *(.data .data.*)
        . = ALIGN(CL_SIZE);
    } >ram AT >rom
    
    __DATA_IMAGE_START__ = LOADADDR(.data);
    __DATA_IMAGE_END__ = LOADADDR(.data) + SIZEOF(.data);
    ASSERT(__DATA_IMAGE_END__ < ORIGIN(rom) + LENGTH(rom), "Data image overflows rom section")

    /* thread-local data segment */
    .tdata : {
        PROVIDE(_tls_data = .);
        PROVIDE(_tdata_begin = .);
        *(.tdata .tdata.*)
        PROVIDE(_tdata_end = .);
        . = ALIGN(CL_SIZE);
    } >ram

    .tbss : {
        PROVIDE(__BSS_START__ = .);
        *(.tbss .tbss.*)
        . = ALIGN(CL_SIZE);
        PROVIDE(_tbss_end = .);
    } >ram

    /* bss segment */
    .sbss : {
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.scommon)
    } >ram

    .bss : {
        *(.bss .bss.*)
        . = ALIGN(CL_SIZE);
        PROVIDE(__BSS_END__ = .);
    } >ram

    _end = .;
    PROVIDE(__end = .);

    /* End of uninitalized data segement */

    .stack ORIGIN(ram) + LENGTH(ram) - STACK_SIZE : {
        FILL(0);
        PROVIDE(__STACK_START__ = .);
        . += STACK_SIZE;
        PROVIDE(__C_STACK_TOP__ = .);
        PROVIDE(__STACK_END__ = .);
    } >ram
	


    /DISCARD/ : {
        *(.eh_frame .eh_frame.*)
    }
}
