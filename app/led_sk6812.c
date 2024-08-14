#include <avr/io.h>
#include "util/delay.h"
#include <stdio.h>
#include <stdbool.h>

#include "led_sk6812.h"
#include "led_commands.h"
#include "../device/adc.h"
#include "../device/buttons.h"
#include "../device/interrupts.h"
#include "../device/led.h"

#define LEVEL_INCREMENT         5
#define LEVEL_DECREMENT         1

// saved LED levels
static uint8_t red_level;
static uint8_t blue_level;
static uint8_t green_level;
static uint8_t white_level;
static uint8_t brightness;

// saved pattern
static enum LED_PATTERN pattern;

// increment decrement with check
static uint8_t led_update_level(uint8_t lvl, bool increment, uint8_t max_lvl)
{
    int16_t tmp_lvl = lvl;

    if (true == increment) {
        if (tmp_lvl < 50) {
            tmp_lvl += LEVEL_INCREMENT;
        } else {
            tmp_lvl += LEVEL_INCREMENT * 2;
        }
        // limit to max uint8_t value
        if (tmp_lvl > max_lvl) {
            tmp_lvl = max_lvl;
        }
    } else {
        tmp_lvl -= LEVEL_DECREMENT;
        // limit to zero
        if (tmp_lvl < 0) {
            tmp_lvl = 0;
        }
    }

    return (uint8_t)tmp_lvl;
}

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
    // red_level = r;
    // green_level = g;
    // blue_level = b;
    // white_level = w;

    // scale levels
    r = (uint16_t)r * brightness / 100;
    g = (uint16_t)g * brightness / 100;
    b = (uint16_t)b * brightness / 100;
    w = (uint16_t)w * brightness / 100;
    
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
        // check release state of each button and increment on click,
        // and decrement on long press
        if (BUTTON_RELEASED == btn_get(0)) {
            red_level = led_update_level(red_level, true, 100);
        } else if (BUTTON_LONG_PRESSED == btn_get(0)) {
            red_level = led_update_level(red_level, false, 100);
        }

        if (BUTTON_RELEASED == btn_get(1)) {
            green_level = led_update_level(green_level, true, 100);
        } else if (BUTTON_LONG_PRESSED == btn_get(1)) {
            green_level = led_update_level(green_level, false, 100);
        }

        if (BUTTON_RELEASED == btn_get(2)) {
            blue_level = led_update_level(blue_level, true, 100);
        } else if (BUTTON_LONG_PRESSED == btn_get(2)) {
            blue_level = led_update_level(blue_level, false, 100);
        }

        if (BUTTON_RELEASED == btn_get(3)) {
            white_level = led_update_level(white_level, true, 100);
        } else if (BUTTON_LONG_PRESSED == btn_get(3)) {
            white_level = led_update_level(white_level, false, 100);
        }

        if (BUTTON_RELEASED == btn_get(4)) {
            brightness = led_update_level(brightness, true, 255);
        } else if (BUTTON_LONG_PRESSED == btn_get(4)) {
            brightness = led_update_level(brightness, false, 255);
        }

        // write data to LEDs
        led_sk6812_set_color_all(red_level, green_level, blue_level, white_level);
        
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