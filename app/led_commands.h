#ifndef LED_CMD_H
#define LED_CMD_H

/**
 * Assembly coded commands to send a 0 or a 1 bit to the LED strip.
 * This piece of code assumes the following:
 * ATmega328p is clocked at 16MHz, sbi/cbi instructions take 2 cycles
 * and nop is 1 cycle
 * 
 * To the LED IC,
 * 0 = 0.3us HIGH followed by 0.9us LOW (+/- 0.15us for each level)
 * 1 = 0.6us HIGH followed by 0.6us LOW (+/- 0.15us for each level)
 * 
 */

#define LED_TX_0()    \
    asm volatile(       \
        "sbi 0x05, 3\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "cbi 0x05, 3\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
    );

#define LED_TX_1()    \
    asm volatile(       \
        "sbi 0x05, 3\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "cbi 0x05, 3\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
    );

#endif  // LED_CMD_H