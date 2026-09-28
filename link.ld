OUTPUT_ARCH(riscv)
ENTRY(_start)
SECTIONS{
    /* for virt  0x80200000  ,  for the pi  0x40000000  */
    . = 0x40000000;
    _start = .;
    .text :{
        *(.text.boot)*(.text)
    }
    .rodata :{
        *(.rodata)
    }
    .data :{
        *(.data)
    }
    .bss :{
        _bss_start = .;
        *(.bss)*(.sbss)
        _bss_end = . ;
    }
    . = ALIGN(16);
    . = . + 0x4000;
    _end =.;
}