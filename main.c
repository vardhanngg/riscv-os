extern void uart_putc(char c);
extern char uart_getc(void);
extern void uart_puts(const char *s);
extern void uart_hex(unsigned long h);
extern void enter_umode(unsigned long sp,unsigned long pc,unsigned long processaddress)__attribute__((noreturn));
enum process_status{
    idle,
    ready,
    blocked
};
typedef struct trapframe{
    unsigned long gp;
    unsigned long tp;
    unsigned long ra;
    unsigned long sp;//at time of trap
    unsigned long t0,t1,t2,t3,t4,t5,t6;
    unsigned long s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11;
    unsigned long a0,a1,a2,a3,a4,a5,a6,a7;
    unsigned long sepc;
    unsigned long sstatus;
}trapframe;
struct process{
    int pid;
    enum process_status status;
    int hart;
    unsigned long sp;//initial one
    unsigned long pc;
    trapframe tf;
};

#define MAX_PROCESSES 8
#define STACK_SIZE 4096
char process_stack[MAX_PROCESSES][STACK_SIZE]__attribute__((aligned(16)));
struct process p_table[MAX_PROCESSES]={};
void process_main(struct process *p){
    //uart_puts("in process main,trying to access sstatus.spp");
    //unsigned long sstatus;
    /*asm("csrr %0,sstatus":"=r"(sstatus));
    if((sstatus>>8)&1)
        uart_puts("in smode");
    else
        uart_puts("in umode");
    */
    uart_puts("\nin process main HI FROM process and pid is ");
    uart_hex(p->pid);
    asm volatile("ecall");
    uart_puts("\nfinished ecall");
    //while(1);
}
int current_running_process=0;
void schedule(void){
    for(int i=current_running_process;i<MAX_PROCESSES;i=i+1){
        if(p_table[i].status==ready){
            uart_puts("found a ready process with pid ");
            uart_hex(p_table[i].pid);
            //unsigned long sp=p_table[i].sp;
            //unsigned long pc=p_table[i].pc;
            //asm volatile("mv sp,%0"::"r"(sp));
            //asm volatile("csrw sepc,%0"::"r"(pc));
            current_running_process=i;
            enter_umode(p_table[i].sp,p_table[i].pc,(unsigned long)&(p_table[i]));
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
    //enter_umode();
    
    //enter_umode(p_table[current_running_process].sp,p_table[current_running_process].pc,(unsigned long)&p_table[current_running_process]);
    p_table[current_running_process].status=blocked;
    current_running_process++;
    schedule();
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
        //uart_hex(p_table[i].pid);uart_puts("\n");
        //uart_hex(p_table[i].sp);uart_puts("\n");
        //uart_hex(p_table[i].pc);uart_puts("\n");
        //uart_hex(p_table[i].pid);
        
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
    uart_puts("\naddress of trap handler is : ");uart_hex(t);
    asm volatile("csrw stvec,%0"::"r"(t));
    unsigned long p;
   // uart_puts("\np is ");uart_hex(p);uart_puts("\n");
    asm volatile("csrr %0,stvec":"=r"(p));
    uart_puts("\nstvec is ");uart_hex(p);uart_puts("\n");
    //unsigned long pm=(unsigned long)&process_main;
    //asm volatile("csrw sepc,%0"::"r"(pm));

    p_table[0].status=ready;
    p_table[1].status=ready;
    for(int i=0;i<MAX_PROCESSES;i=i+1){
        p_table[i].status=ready;
    }
    schedule();
    while(1);
}