#ifndef DEV_BTN_H
#define DEV_BTN_H

#include <stdint.h>

enum BUTTON_STATE {
    BUTTON_NOT_PRESSED,
    BUTTON_PRESSED,
    BUTTON_RELEASED,
    BUTTON_LONG_PRESSED,
};

struct button_obj {
    uint8_t pin;
    uint8_t button_debounce;
    uint8_t button_longpress_count;
    enum BUTTON_STATE state;
};

void btn_init(void);
void btn_task(void);
enum BUTTON_STATE btn_get(uint8_t n);


#endif  // DEV_BTN_H