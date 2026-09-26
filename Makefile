CC = riscv64-unknown-elf-gcc
AS = riscv64-unknown-elf-as
LD = riscv64-unknown-elf-ld
OBJCOPY = riscv64-unknown-elf-objcopy

CFLAGS = -mcmodel=medany -ffreestanding -nostdlib -g -Wall

kernel: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel

kernel.elf: start.o main.o uart.o enter_umode.o
	$(LD) -T link.ld -o kernel.elf start.o main.o uart.o enter_umode.o

start.o: start.S
	$(AS) start.S -o start.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

uart.o: uart.c
	$(CC) $(CFLAGS) -c uart.c -o uart.o

clean:
	rm -f *.o kernel kernel.elf
enter_umode.o: enter_umode.S
	$(AS) -o enter_umode.o enter_umode.S