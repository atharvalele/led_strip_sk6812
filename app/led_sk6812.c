#include <avr/io.h>
#include "util/delay.h"
#include <stdio.h>
#include <stdbool.h>

#include "led_sk6812.h"
#include "led_commands.h"
#include "../device/adc.h"
#include "../device/interrupts.h"
#include "../device/led.h"

// saved LED levels
static uint8_t red_level;
static uint8_t blue_level;
static uint8_t green_level;
static uint8_t white_level;

// saved pattern
static enum LED_PATTERN pattern;

// initialize LED strip and maintain in reset
void led_sk6812_init(void)
{
    // set pin as output
    DDRB |= (1 << DDB3);

    // set static pattern
    pattern = LED_STATIC;

    // set all colors to zero initially
    // essentially turning the strip off
    led_sk6812_set_color_all(0, 0, 0, 0);
}

// Note that this function assumes interrupts have been disabled prior to calling
void led_sk6812_set_color_single(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    for (int8_t bit = 7; bit >= 0; bit--) {
        if ((g & (1 << bit))) {
            LED_TX_1();
        } else {
            LED_TX_0();
        }
    }

    for (int8_t bit = 7; bit >= 0; bit--) {
        if ((r & (1 << bit))) {
            LED_TX_1();
        } else {
            LED_TX_0();
        }
    }

    for (int8_t bit = 7; bit >= 0; bit--) {
        if ((b & (1 << bit))) {
            LED_TX_1();
        } else {
            LED_TX_0();
        }
    }

    for (int8_t bit = 7; bit >= 0; bit--) {
        if ((w & (1 << bit))) {
            LED_TX_1();
        } else {
            LED_TX_0();
        }
    }
}

void led_sk6812_set_color_all(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    // uint8_t tmp, i;
    uint8_t led;

    // save levels
    red_level = r;
    green_level = g;
    blue_level = b;
    white_level = w;
    
    interrupts_disable();

    for (led = 0; led < NUM_LEDS; led++) {
        led_sk6812_set_color_single(r, g, b, w);
    }

    // transmit comms reset sequence
    // disable interrupts, set pin low for 80us, enable interrupts
    // 80us = 1280 cpu cycles @ 16MHz clock
    // _delay_loop_2() has 4 cycles per iteration, not accounting for overhead
    // 1280 / 4 = 320
    // iterate this NUM_LEDS times
    asm("cbi 0x05, 3");
    _delay_loop_2(320);

    interrupts_enable();
}


void led_sk6812_task(void)
{
    switch (pattern) {
    case LED_STATIC:
    break;

    case LED_CHRISTMAS:
    break;

    case LED_DIWALI:
    break;

    case LED_FAKE_FIRE:
    break;

    default:
    break;
    }
}