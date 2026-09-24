extern void uart_putc(char c);
extern char uart_getc(void);
extern void uart_puts(const char *s);
extern void uart_hex(unsigned long h);
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
    while(1);
}
void schedule(void){
    for(int i=0;i<MAX_PROCESSES;i=i+1){
        if(p_table[i].status==ready){
            uart_puts("found a ready process with pid ");
            uart_hex(p_table[i].pid);
        }
    }
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
        //uart_hex(p_table[i].pid);
        
    }
    int i=0;
    p_table[i].status=ready;
    schedule();
    while(1);
}