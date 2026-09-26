.global enter_umode
enter_umode:
    mv sp,a0
    csrw sepc,a1
    li t0,0b100000000
    csrc sstatus,t0
    sret