#include <avr/io.h>
#include "util/delay.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "led_sk6812.h"
#include "led_commands.h"
#include "led_patterns.h"
#include "../device/adc.h"
#include "../device/buttons.h"
#include "../device/eeprom.h"
#include "../device/interrupts.h"
#include "../device/led.h"

#define LEVEL_INCREMENT 5
#define LEVEL_DECREMENT 1

// saved LED levels
static uint8_t red_level;
static uint8_t blue_level;
static uint8_t green_level;
static uint8_t white_level;
static uint8_t brightness;
static uint8_t fire_brightness;

// saved pattern
static enum LED_PATTERN pattern;

// led pattern colors
uint8_t pattern_r[PATTERN_NUM_LEDS], pattern_g[PATTERN_NUM_LEDS],
        pattern_b[PATTERN_NUM_LEDS], pattern_w[PATTERN_NUM_LEDS];
uint8_t pattern_tmp;

// pattern update tick rate
uint8_t pattern_update_tick;

// time since last update
static bool init_success = false;
uint8_t data_save_tick;
bool data_updated;

// read saved LED data
static bool led_savedata_read(void)
{
    uint8_t chksum_eeprom, chksum_calc = 0;
    // uint8_t save_pattern,
    //         save_bright,
    //         save_r, save_g, save_b, save_w,
    //         save_fire_bright;

    uint8_t data[LED_SAVE_FIRE_BRIGHT+1];

    // start reading
    chksum_eeprom = eeprom_read(LED_SAVE_CHECKSUM);
    for (uint8_t addr = LED_SAVE_PATTERN; addr <= LED_SAVE_FIRE_BRIGHT; addr++) {
        data[addr] = eeprom_read(addr);
        chksum_calc += data[addr];
    }

    printf("Reading: chksum eeprom = 0x%X \t chksum calc = 0x%X\r\n", chksum_eeprom, chksum_calc);

    // validate checksum, if OK then set values return true, else return false
    if (chksum_calc == chksum_eeprom) {
        pattern =           data[LED_SAVE_PATTERN];
        brightness =        data[LED_SAVE_BRIGHT];
        red_level =         data[LED_SAVE_R];
        green_level =       data[LED_SAVE_G];
        blue_level =        data[LED_SAVE_B];
        white_level =       data[LED_SAVE_W];
        fire_brightness =   data[LED_SAVE_FIRE_BRIGHT];

        return true;
    } else {
        return false;
    }
}

// save data to EEPROM
static void led_savedata_write(void)
{
    uint8_t chksum = 0;
    chksum = pattern + brightness + red_level + green_level + blue_level + white_level \
                + fire_brightness;

    // write all to eeprom
    eeprom_write(LED_SAVE_CHECKSUM, chksum);
    eeprom_write(LED_SAVE_PATTERN, pattern);
    eeprom_write(LED_SAVE_BRIGHT, brightness);
    eeprom_write(LED_SAVE_R, red_level);
    eeprom_write(LED_SAVE_G, green_level);
    eeprom_write(LED_SAVE_B, blue_level);
    eeprom_write(LED_SAVE_W, white_level);
    eeprom_write(LED_SAVE_FIRE_BRIGHT, fire_brightness);

    printf("Writing: chksum = 0x%X\r\n", chksum);
}

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

    data_updated = true;
    data_save_tick = 0;

    return (uint8_t)tmp_lvl;
}

// rotate pattern
void led_rotate_pattern(uint8_t rotate_amt)
{
    uint8_t r_last, g_last, b_last, w_last;

    for (uint8_t i = 0; i < rotate_amt; i++) {
        r_last = pattern_r[0];
        g_last = pattern_g[0];
        b_last = pattern_b[0];
        w_last = pattern_w[0];

        for (uint8_t j = 0; j < PATTERN_NUM_LEDS - 1; j++) {
            pattern_r[j] = pattern_r[j + 1];
            pattern_g[j] = pattern_g[j + 1];
            pattern_b[j] = pattern_b[j + 1];
            pattern_w[j] = pattern_w[j + 1];
        }

        pattern_r[PATTERN_NUM_LEDS - 1] = r_last;
        pattern_g[PATTERN_NUM_LEDS - 1] = g_last;
        pattern_b[PATTERN_NUM_LEDS - 1] = b_last;
        pattern_w[PATTERN_NUM_LEDS - 1] = w_last;

    }
}

// initialize LED strip and maintain in reset
void led_sk6812_init(void)
{
    // set pin as output
    DDRB |= (1 << DDB3);

    // load data from EEPROM, else set defaults
    if (false == led_savedata_read()) {

        // set static pattern
        pattern = LED_STATIC;

        // initial brightness
        brightness = 20;
        fire_brightness = 150;

        // set all colors to zero initially
        // essentially turning the strip off
        led_sk6812_set_color_all(0, 0, 0, 0);

        init_success = false;
        data_updated = true;
    } else {
        init_success = true;
    }

    data_save_tick = 0;
    data_updated = false;
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
    uint8_t r[NUM_LEDS], g[NUM_LEDS], b[NUM_LEDS], w[NUM_LEDS];
    uint8_t led;

    // copy the pattern
    for (led = 0; led < PATTERN_NUM_LEDS; led++)
    {
        r[led] = brightness * pattern_r[led] / 100;
        g[led] = brightness * pattern_g[led] / 100;
        b[led] = brightness * pattern_b[led] / 100;
        w[led] = brightness * pattern_w[led] / 100;
    }

    interrupts_disable();

    for (led = 0; led < PATTERN_NUM_LEDS; led++) {
        led_sk6812_set_color_single(r[led], g[led], b[led], w[led]);
        led_sk6812_set_color_single(r[led], g[led], b[led], w[led]);
        led_sk6812_set_color_single(r[led], g[led], b[led], w[led]);
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
    bool brightness_changed = false;

    // check if init
    if (true == init_success) {
        // force update
        pattern_changed = true;
        brightness_changed = true;

        init_success = false;
    }

    // check for pattern change
    if (BUTTON_RELEASED == btn_get(5))
    {
        pattern++;
        pattern_changed = true;
        pattern_update_tick = 254;
        // rollover
        if (pattern >= LED_PATTERN_LAST)
        {
            pattern = LED_STATIC;
        }

        data_updated = true;
        data_save_tick = 0;
    }

    // check brightness change
    if (BUTTON_RELEASED == btn_get(4))
    {
        brightness = led_update_level(brightness, true, 255);
        brightness_changed = true;

        fire_brightness = led_update_level(fire_brightness, true, 230);
    }
    else if (BUTTON_LONG_PRESSED == btn_get(4))
    {
        brightness = led_update_level(brightness, false, 255);
        brightness_changed = true;

        fire_brightness = led_update_level(fire_brightness, false, 50);
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

    case LED_WARM_WHITE:
        if ((true == pattern_changed) || (true == brightness_changed)) {
            // write data to LEDs
            led_sk6812_set_color_all(20, 0, 10, 100);

            pattern_changed = false;
            brightness_changed = false;
        }
    break;

    case LED_CHRISTMAS:
        if (true == pattern_changed)
        {
            // load data for first time
            for (uint8_t led = 0; led < PATTERN_NUM_LEDS; led++) {
                pattern_r[led] = pattern_christmas[led][0];
                pattern_g[led] = pattern_christmas[led][1];
                pattern_b[led] = pattern_christmas[led][2];
                pattern_w[led] = pattern_christmas[led][3];
            }

            pattern_changed = 0;
        }

        // update pattern for next iteration, change every 100 ticks
        pattern_update_tick++;
        if (pattern_update_tick > 150)
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
            if (true == pattern_changed) {
                // load data for first time
                for (uint8_t led = 0; led < PATTERN_NUM_LEDS; led++) {
                    pattern_r[led] = pattern_diwali[led][0];
                    pattern_g[led] = pattern_diwali[led][1];
                    pattern_b[led] = pattern_diwali[led][2];
                    pattern_w[led] = pattern_diwali[led][3];
                }

                pattern_changed = 0;
            }

            if (true == brightness_changed) {
                brightness_changed = false;
            }

            // update pattern for next iteration, change every 100 ticks
            pattern_update_tick++;
            if (pattern_update_tick >= 100) {
                // rotate array by N random places (N between 0 to 20)
                led_rotate_pattern(rand() % 20);

                led_sk6812_write_pattern();
            }
        break;

    case LED_FAKE_FIRE:
            // set random brightness every 50ms
            pattern_update_tick++;
            if (true == pattern_changed) {
                led_sk6812_set_color_all(80, 10, 0, 10);
                pattern_changed = false;
            }

            if (pattern_update_tick >= (2 + (rand() % 8))) {
                brightness = fire_brightness + (rand() % 25);
                led_sk6812_set_color_all(80, 10, 0, 10);
                pattern_update_tick = 0;
            }
        break;

    default:
        break;
    }
}

void led_savedata_task(void)
{
    if (true == data_updated) {
        data_save_tick++;

        // save after 5 seconds
        if (data_save_tick >= 5) {
            onboard_led_on();
            led_savedata_write();
            printf("Data saved to EEPROM!\r\n");
            data_updated = false;
            data_save_tick = 0;
            onboard_led_off();
        }
    }
}