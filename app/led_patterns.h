#ifndef LED_PATTERN_H
#define LED_PATTERN_H

#include <stdint.h>

#define PATTERN_NUM_LEDS        20

// christmas
uint8_t pattern_christmas[PATTERN_NUM_LEDS][4] = {
    {100, 0, 0, 0},
    {100, 0, 0, 0},
    {0, 100, 0, 0},
    {0, 100, 0, 0},

    {100, 0, 0, 0},
    {100, 0, 0, 0},
    {0, 100, 0, 0},
    {0, 100, 0, 0},

    {100, 0, 0, 0},
    {100, 0, 0, 0},
    {0, 100, 0, 0},
    {0, 100, 0, 0},

    {100, 0, 0, 0},
    {100, 0, 0, 0},
    {0, 100, 0, 0},
    {0, 100, 0, 0},

    {100, 0, 0, 0},
    {100, 0, 0, 0},
    {0, 100, 0, 0},
    {0, 100, 0, 0},

};

#endif  // LED_PATTERN_H