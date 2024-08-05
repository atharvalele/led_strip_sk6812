#ifndef DEV_UART_PRINT_H
#define DEV_UART_PRINT_H

#include <stdint.h>

void uart_print_init();
int uart_putchar(char data);

#endif // DEV_UART_PRINT_H