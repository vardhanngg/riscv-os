.section ".text.boot"
.global _start
_start:
    la t0,_bss_start
    la t1,_bss_end
    ble t1,t0,bss_clear_done
clear_bss:
    sb zero,0(t0)
    addi t0,t0,1
    blt t0,t1,clear_bss
bss_clear_done:
    la sp,_end
    tail start_kernel
    