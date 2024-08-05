#include <avr/io.h>
#include <stdio.h>
#include "uart_print.h"

#define BAUDRATE                19200ul
#define BAUDRATE_REG_VAL        ((F_CPU/16/BAUDRATE)-1)

// declarations for stdio
int usart_putchar_printf(char var, FILE *stream);
static FILE stdout_instance = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

void uart_print_init()
{
    // Set baudrate registers
    UBRR0H = (uint8_t)(BAUDRATE_REG_VAL >> 8);
    UBRR0L = (uint8_t)(BAUDRATE_REG_VAL);

    // Enable Rx/Tx
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Bind output to stdio
    stdout = &stdout_instance;
}

// this function is called by printf as a stream handler
int usart_putchar_printf(char var, FILE *stream) {
    uart_putchar(var);
    return 0;
}

// write single byte to uart
int uart_putchar(char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    // Start transmission
    UDR0 = data; 

    return 0;
}