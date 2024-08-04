#include <avr/io.h>
#include <avr/interrupt.h>
#include "tmr_tick.h"
#include "led.h"

// 16 MHz time period = 63ns
// 63ns * 64 = 4us
// 1ms / 4us = 250
#define TMR_RST_VAL     (255u - 250u)

static volatile uint16_t tmr_1ms_tick = 0;

// Create a 1ms timer tick
// Fosc is 16MHz for Arduino nano
void tmr_tick_init(void)
{
    // Set initial timer value
    TCNT0 = TMR_RST_VAL;

    // Set clock source
    TCCR0B = 0b011;     // Clk(IO) / 64

    // Enable interrupt for timer overflow
    // bit 0 enabled TOIE
    TIMSK0 = 0x01;
}

// Timer 0 ISR
ISR (TIMER0_OVF_vect)
{
    tmr_1ms_tick++;

    TCNT0 = TMR_RST_VAL;

    // Interrupt flag is cleared by HW
}