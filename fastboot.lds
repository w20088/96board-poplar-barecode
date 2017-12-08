SECTIONS {
    . = 0x00000000;
    .text : { *(.text) }
    __text_end = .;
    
    . = ALIGN(4);
    .rodata : {*(.rodata*)} 
    
    . = ALIGN(4);
    .data : { *(.data) }
    
    . = ALIGN(4);
    __bss_start = .;
    .bss : { *(.bss)  *(COMMON) }
    __bss_end = .;
}
