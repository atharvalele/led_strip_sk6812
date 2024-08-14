#include <avr/io.h>
#include "buttons.h"

#include <stdio.h>

#define NUM_BTNS            6
#define DEBOUNCE_COUNT      5
#define RELEASED_COUNT      3
#define LONGPRESS_COUNT     20

// save button states
struct button_obj btns[NUM_BTNS];

void btn_init(void)
{
    // set relevant IO pins as input
    // pins are input by default so no need to change
    // also enable internal weak pullups
    PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);

    // init state, counter, pin#
    // btns are connected PD2 - PD7, hence pin is (btn num + 2)
    for (uint8_t n = 0; n < NUM_BTNS; n++) {
        btns[n].pin = n + 2;
        btns[n].button_debounce = 0;
        btns[n].button_longpress_count = 0;
        btns[n].state = BUTTON_NOT_PRESSED;
    }
}

void btn_task(void)
{
    uint8_t pin_state;
    // read IO state of each button, and update state accordingly
    for (uint8_t n = 0; n < NUM_BTNS; n++) {
        // read IO state
        pin_state = (PIND & (1 << btns[n].pin)) ? 1 : 0;

        switch (btns[n].state) {
        case BUTTON_NOT_PRESSED:
            // if state is LOW, that means button will be pressed
            // increment debounce counter
            if (0 == pin_state) {
                btns[n].button_debounce++;
                // next state check
                if (btns[n].button_debounce >= DEBOUNCE_COUNT) {
                    btns[n].button_debounce = DEBOUNCE_COUNT;
                    btns[n].state = BUTTON_PRESSED;
                }
            } else {
                btns[n].button_debounce = 0;
            }
        break;

        case BUTTON_PRESSED:
            // increment long press counter as long as it is still reading 0
            if (0 == pin_state) {
                btns[n].button_longpress_count++;
                if (btns[n].button_longpress_count >= LONGPRESS_COUNT) {
                    btns[n].button_longpress_count = 0;
                    btns[n].state = BUTTON_LONG_PRESSED;
                }
            } else {
                btns[n].button_debounce++;
                if (btns[n].button_debounce >= RELEASED_COUNT) {
                    btns[n].button_debounce = 0;
                    btns[n].button_longpress_count = 0;
                    btns[n].state = BUTTON_RELEASED;
                }
            }
        break;

        case BUTTON_LONG_PRESSED:
            if (1 == pin_state) {
                btns[n].state = BUTTON_NOT_PRESSED;
                btns[n].button_debounce = 0;
                btns[n].button_longpress_count = 0;
            }
        break;

        case BUTTON_RELEASED:
            // do nothing
            // state will be changed when released state is access by a consumer
        break;

        default:
        break;
        }
    }
}

enum BUTTON_STATE btn_get(uint8_t n)
{
    enum BUTTON_STATE state = btns[n].state;

    if (BUTTON_RELEASED == btns[n].state) {
        btns[n].state = BUTTON_NOT_PRESSED;
    }

    return state;
}