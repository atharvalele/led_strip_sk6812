#include <avr/io.h>
#include "interrupts.h"

void interrupts_enable(void)
{
    volatile uint8_t val;

    // enable interrupts
    val = AVR_STATUS_REG;
    val |= 0x80;
    AVR_STATUS_REG = val;
}

void interrupts_disable(void)
{
    volatile uint8_t val;    

    // disable interrupts
    val = AVR_STATUS_REG;
    val &= ~0x80;
    AVR_STATUS_REG = val;
}
