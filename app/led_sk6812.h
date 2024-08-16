#ifndef LED_SK6812_H
#define LED_SK6812_H

#include <stdint.h>

// number of LEDs in strip
#define NUM_LEDS        60

enum LED_SAVE_ADDR {
    LED_SAVE_CHECKSUM,
    LED_SAVE_PATTERN,
    LED_SAVE_BRIGHT,
    LED_SAVE_R,
    LED_SAVE_G,
    LED_SAVE_B,
    LED_SAVE_W,
    LED_SAVE_FIRE_BRIGHT,
};

enum LED_PATTERN {
    LED_STATIC,
    LED_WARM_WHITE,
    LED_CHRISTMAS,
    LED_DIWALI,
    LED_FAKE_FIRE,
    LED_PATTERN_LAST
};

void led_sk6812_init(void);
void led_sk6812_set_color_all(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void led_sk6812_set_color_single(uint8_t r, uint8_t g, uint8_t b, uint8_t w);

void led_sk6812_task(void);
void led_savedata_task(void);

#endif