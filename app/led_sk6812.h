#ifndef LED_SK6812_H
#define LED_SK6812_H

#include <stdint.h>

// number of LEDs in strip
#define NUM_LEDS        60

enum LED_PATTERN {
    LED_STATIC,
    LED_CHRISTMAS,
    LED_DIWALI,
    LED_FAKE_FIRE,
};

void led_sk6812_init(void);
void led_sk6812_set_color_all(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void led_sk6812_set_color_single(uint8_t r, uint8_t g, uint8_t b, uint8_t w);

void led_sk6812_task(void);

#endif