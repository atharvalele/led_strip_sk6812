# RGBW LED Strip Controller

## Intro

This is a SK6812 based RGBW LED strip controller with a few features:

- 4 color modes:

    - Static color (choose R, G, B, W levels)
    - Warm white
    - Christmas (green-red animation)
    - Diwali (Indian festive colors)
    - Fireplace (fake fireplace effect)
- Configurable brightness in each mode
- Autosave selected color mode & settings to EEPROM

## Hardware

- ATmega328p (Arduino Nano)
- Push buttons
- 2m SK6812-based RGBW LED strip

## Software

The code does not use the Arduino ecosystem, but rather programs the 328p at
register level.

### Transmitting data to the LED strip

- [led_commands.h](app/led_commands.h) contains the commands used to send data
to the LED strip.
- The micro is bitbanging the data using GPIO and software delays
- Since the micro is clocked at 16MHz, we can calculate how long a NOP takes
and use NOPs to insert precise delays between IO toggles.