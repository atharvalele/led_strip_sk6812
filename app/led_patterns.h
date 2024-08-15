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

uint8_t pattern_diwali[PATTERN_NUM_LEDS][4] = {
    {75, 10, 0, 0},     // orange
    {25, 0, 50, 0},    // light purple
    {75, 0, 50, 0},    // magenta
    {80, 35, 0, 0},    // yellow
    {20, 0, 0, 100},    // warm white

    {75, 10, 0, 0},     // orange
    {25, 0, 50, 0},    // light purple
    {75, 0, 50, 0},    // magenta
    {80, 35, 0, 0},    // yellow
    {20, 0, 0, 100},    // warm white

    {75, 10, 0, 0},     // orange
    {25, 0, 50, 0},    // light purple
    {75, 0, 50, 0},    // magenta
    {80, 35, 0, 0},    // yellow
    {20, 0, 0, 100},    // warm white

    {75, 10, 0, 0},     // orange
    {25, 0, 50, 0},    // light purple
    {75, 0, 50, 0},    // magenta
    {80, 35, 0, 0},    // yellow
    {20, 0, 0, 100}    // warm white
};

#endif  // LED_PATTERN_H