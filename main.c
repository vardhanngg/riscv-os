extern void uart_putc(char c);
extern char uart_getc(void);
extern void uart_puts(const char *s);
extern void uart_hex(unsigned long h);
extern void enter_umode(unsigned long sp,unsigned long pc)__attribute__((noreturn));
enum process_status{
    idle,
    ready,
    blocked
};
struct process{
    int pid;
    enum process_status status;
    int hart;
    unsigned long sp;
    unsigned long pc;
};

#define MAX_PROCESSES 8
#define STACK_SIZE 4096
char process_stack[MAX_PROCESSES][STACK_SIZE]__attribute__((aligned(16)));
struct process p_table[MAX_PROCESSES]={};
void process_main(void){
    uart_puts("in process main,trying to access sstatus.spp");
    unsigned long sstatus;
    asm("csrr %0,sstatus":"=r"(sstatus));
    if((sstatus>>8)&1)
        uart_puts("in smode");
    else
        uart_puts("in umode");
    
    asm volatile("ecall");
    uart_puts("\nfinished ecall");
    while(1);
}
void schedule(void){
    for(int i=0;i<MAX_PROCESSES;i=i+1){
        if(p_table[i].status==ready){
            uart_puts("found a ready process with pid ");
            uart_hex(p_table[i].pid);
            //unsigned long sp=p_table[i].sp;
            //unsigned long pc=p_table[i].pc;
            //asm volatile("mv sp,%0"::"r"(sp));
            //asm volatile("csrw sepc,%0"::"r"(pc));
            enter_umode(p_table[i].sp,p_table[i].pc);
        }
    }
}
//void enter_umode(unsigned long sp,unsigned long pc);
void trap_handler(void)__attribute__((aligned(4)));
void trap_handler(void){
    unsigned long sstatus;
    asm("csrr %0,sstatus":"=r"(sstatus));
    if((sstatus>>8)&1)
        uart_puts("in smode");
    else
        uart_puts("in umode");
    unsigned long scause;
    asm volatile("csrr %0,scause":"=r"(scause));
    uart_puts("\n got the trap and scall is ");
    uart_hex(scause);
    while(1);
}

void start_kernel(void)
{
    uart_puts("My OS has started...");
    for(int i=0;i<MAX_PROCESSES;i=i+1){
        p_table[i].pid=i;
        p_table[i].status=idle;
        p_table[i].hart=0;
        p_table[i].sp=(unsigned long)&(process_stack[i][STACK_SIZE]);
        p_table[i].pc=(unsigned long)&process_main;
        uart_hex(p_table[i].pid);uart_puts("\n");
        uart_hex(p_table[i].sp);uart_puts("\n");
        uart_hex(p_table[i].pc);uart_puts("\n");
        uart_hex(p_table[i].pid);
        
    }

    //unsigned long value;
    //asm("csrr %0, sstatus":"=r"(value));
    //uart_puts("value is ");
    //uart_hex(value);
    //int x=((value>>8)& 1);
   // uart_puts("x is ");
   // uart_hex(x);
    unsigned long sstatus;
    asm volatile("csrr %0,sstatus":"=r"(sstatus));
    uart_puts("\nsstatus is ");uart_hex(sstatus);
    if(((sstatus>>8)&1)==0){
        uart_puts("o so umode next");
    }
    else
        uart_puts("1 so s mode next");
    unsigned long t=(unsigned long)&trap_handler;
    asm volatile("csrw stvec,%0"::"r"(t));
    unsigned long p;
    //uart_puts("\np is ");uart_hex(p);uart_puts("\n");
    asm volatile("csrr %0,stvec":"=r"(p));
    uart_puts("\nstvec is ");uart_hex(p);uart_puts("\n");
    //unsigned long pm=(unsigned long)&process_main;
    //asm volatile("csrw sepc,%0"::"r"(pm));

    p_table[0].status=ready;
    //p_table[1].status=ready;
    schedule();
    while(1);
}