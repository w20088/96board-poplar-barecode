SECTIONS {
    . = 0x00000000;
    .text : { *(.text) }
    __text_end = .;
    
    . = ALIGN(4);
    .rodata : {*(.rodata*)} 
    
    . = ALIGN(4);
    .data : { *(.data) }

    . = ALIGN(4);
    .tail :
    {
       tail.o      (.tail)
    }

    . = ALIGN(4);
    __bss_start = .;
    .bss : { *(.bss)  *(COMMON) }
    __bss_end = .;

    . = ALIGN(0x100000);
    __page_table_start = .;
    . = . + 0x4000; /* 16K */
    __page_table_end = .;

    . = ALIGN(0x100000);
    __end = .;

}
