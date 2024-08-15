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

#define LEVEL_INCREMENT 5
#define LEVEL_DECREMENT 1

// 4 LEDs per "pixel"
#define PATTERN_NUM_LEDS        15

// saved LED levels
static uint8_t red_level;
static uint8_t blue_level;
static uint8_t green_level;
static uint8_t white_level;
static uint8_t brightness;

// saved pattern
static enum LED_PATTERN pattern;

// led pattern colors
uint8_t pattern_r[PATTERN_NUM_LEDS], pattern_g[PATTERN_NUM_LEDS], pattern_b[PATTERN_NUM_LEDS];
uint8_t pattern_tmp;

// pattern update tick rate
uint8_t pattern_update_tick;

// pre-set patterns
const uint16_t pattern_christmas[3] = {0xAAAA, 0x5555, 0x0}; // alternating sets of red & green

// increment decrement with check
static uint8_t led_update_level(uint8_t lvl, bool increment, uint8_t max_lvl)
{
    int16_t tmp_lvl = lvl;

    if (true == increment)
    {
        if (tmp_lvl < 50)
        {
            tmp_lvl += LEVEL_INCREMENT;
        }
        else
        {
            tmp_lvl += LEVEL_INCREMENT * 2;
        }
        // limit to max uint8_t value
        if (tmp_lvl > max_lvl)
        {
            tmp_lvl = max_lvl;
        }
    }
    else
    {
        tmp_lvl -= LEVEL_DECREMENT;
        // limit to zero
        if (tmp_lvl < 0)
        {
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

    // initial brightness
    brightness = 20;

    // set all colors to zero initially
    // essentially turning the strip off
    led_sk6812_set_color_all(0, 0, 0, 0);
}

// Note that this function assumes interrupts have been disabled prior to calling
void led_sk6812_set_color_single(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    for (int8_t bit = 7; bit >= 0; bit--)
    {
        if ((g & (1 << bit)))
        {
            LED_TX_1();
        }
        else
        {
            LED_TX_0();
        }
    }

    for (int8_t bit = 7; bit >= 0; bit--)
    {
        if ((r & (1 << bit)))
        {
            LED_TX_1();
        }
        else
        {
            LED_TX_0();
        }
    }

    for (int8_t bit = 7; bit >= 0; bit--)
    {
        if ((b & (1 << bit)))
        {
            LED_TX_1();
        }
        else
        {
            LED_TX_0();
        }
    }

    for (int8_t bit = 7; bit >= 0; bit--)
    {
        if ((w & (1 << bit)))
        {
            LED_TX_1();
        }
        else
        {
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

    for (led = 0; led < NUM_LEDS; led++)
    {
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

void led_sk6812_write_pattern()
{
    // local pattern copy to speed up execution
    uint8_t r[NUM_LEDS], g[NUM_LEDS], b[NUM_LEDS];
    uint8_t led;

    // copy the pattern
    for (led = 0; led < PATTERN_NUM_LEDS; led++)
    {
        r[led] = brightness * pattern_r[led];
        g[led] = brightness * pattern_g[led];
        b[led] = brightness * pattern_b[led];
    }

    interrupts_disable();

    for (led = 0; led < PATTERN_NUM_LEDS; led++) {
        led_sk6812_set_color_single(r[led], g[led], b[led], 0);
        led_sk6812_set_color_single(r[led], g[led], b[led], 0);
        led_sk6812_set_color_single(r[led], g[led], b[led], 0);
        led_sk6812_set_color_single(r[led], g[led], b[led], 0);
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

    // clear pattern tick
    pattern_update_tick = 0;
}

void led_sk6812_task(void)
{
    bool pattern_changed = false;

    // check for pattern change
    if (BUTTON_RELEASED == btn_get(5))
    {
        pattern++;
        pattern_changed = true;
        pattern_update_tick = 0;
        // rollover
        if (pattern >= LED_PATTERN_LAST)
        {
            pattern = LED_STATIC;
        }
    }

    // check brightness change
    if (BUTTON_RELEASED == btn_get(4))
    {
        brightness = led_update_level(brightness, true, 255);
    }
    else if (BUTTON_LONG_PRESSED == btn_get(4))
    {
        brightness = led_update_level(brightness, false, 255);
    }

    // execute pattern code
    switch (pattern)
    {
    case LED_STATIC:
        // check release state of each button and increment on click,
        // and decrement on long press
        if (BUTTON_RELEASED == btn_get(0))
        {
            red_level = led_update_level(red_level, true, 100);
        }
        else if (BUTTON_LONG_PRESSED == btn_get(0))
        {
            red_level = led_update_level(red_level, false, 100);
        }

        if (BUTTON_RELEASED == btn_get(1))
        {
            green_level = led_update_level(green_level, true, 100);
        }
        else if (BUTTON_LONG_PRESSED == btn_get(1))
        {
            green_level = led_update_level(green_level, false, 100);
        }

        if (BUTTON_RELEASED == btn_get(2))
        {
            blue_level = led_update_level(blue_level, true, 100);
        }
        else if (BUTTON_LONG_PRESSED == btn_get(2))
        {
            blue_level = led_update_level(blue_level, false, 100);
        }

        if (BUTTON_RELEASED == btn_get(3))
        {
            white_level = led_update_level(white_level, true, 100);
        }
        else if (BUTTON_LONG_PRESSED == btn_get(3))
        {
            white_level = led_update_level(white_level, false, 100);
        }

        // write data to LEDs
        led_sk6812_set_color_all(red_level, green_level, blue_level, white_level);

        break;

    case LED_CHRISTMAS:
        if (true == pattern_changed)
        {
            // load data for first time
            for (uint8_t led = 0; led < PATTERN_NUM_LEDS; led++) {
                pattern_r[led] = (pattern_christmas[0] & (1 << led)) >> led;
                pattern_g[led] = (pattern_christmas[1] & (1 << led)) >> led;
                pattern_b[led] = (pattern_christmas[2] & (1 << led)) >> led;

                // printf("R: 0x%X\t G: 0x%X\r\n ----- \r\n", pattern_r[led], pattern_g[led]);
            }

            pattern_changed = 0;
        }

        // update pattern for next iteration, change every 100 ticks
        pattern_update_tick++;
        if (pattern_update_tick >= 100)
        {
            // swap red & green values
            for (uint8_t led = 0; led < PATTERN_NUM_LEDS; led++) {
                pattern_tmp = pattern_r[led];
                pattern_r[led] = pattern_g[led];
                pattern_g[led] = pattern_tmp;
            }

            // write data to LEDs
            led_sk6812_write_pattern();
        }
        break;

    case LED_DIWALI:
        break;

    case LED_FAKE_FIRE:
        break;

    default:
        break;
    }
}