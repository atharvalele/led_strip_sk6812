#include <avr/io.h>
#include "led.h"

#define ONBOARD_LED     (1 << 5)

void onboard_led_init(void)
{
    // onboard LED is on PB5
    // configure it as an output
    DDRB |= ONBOARD_LED;
}

void onboard_led_on(void)
{
    // set PB5 = 1
    PORTB |= ONBOARD_LED;
}

void onboard_led_off(void)
{
    // R-M-W operation to avoid modifying other PORTB bits
    volatile uint8_t val = PORTB;
    val &= ~(ONBOARD_LED);

    PORTB = val;
}

void onboard_led_toggle(void)
{
    // writing a 1 to a bit in the PINx register
    // toggles that pin
    PINB |= ONBOARD_LED;
}