#define UART_BASE 0x10000000UL
#define UART_RBR ((volatile unsigned char *)(UART_BASE+0x0))
#define UART_THR ((volatile unsigned char *)(UART_BASE+0x0))
#define UART_LSR ((volatile unsigned char *)(UART_BASE+0x5))
#define LSR_DR (1 << 0)
#define LSR_TDRQ (1 << 5)

char uart_getc(){
    while(!( (*UART_LSR) & LSR_DR));
    char c= *UART_RBR;
    if(c=='\r')
        return '\n';
    return c;
}
void uart_putc(char c){
    if (c=='\n')
        uart_putc('\r');
    while(!((*UART_LSR)& LSR_TDRQ));
    *UART_THR= c;
}
void uart_puts(const char * s){
    for (int i=0;s[i]!='\0';i++){
        uart_putc(s[i]);
    }
}
