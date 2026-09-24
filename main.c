extern void uart_putc(char c);
extern char uart_getc(void);
extern void uart_puts(const char *s);
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
};

#define MAX_PROCESSES 8
struct process p_table[MAX_PROCESSES]={};

void start_kernel(void)
{
    uart_puts("My OS has started...");
    for(int i=0;i<MAX_PROCESSES;i=i+1){
        p_table[i].pid=i;
        p_table[i].status=0;
        p_table[i].hart=0;
        p_table[i].sp=0;
    }
    while(1);
}