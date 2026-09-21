extern void uart_putc(char c);
extern char uart_getc(void);
extern void uart_puts(const char *s);
void start_kernel(void)
{
    uart_puts("My OS has started...");
    while(1);
}