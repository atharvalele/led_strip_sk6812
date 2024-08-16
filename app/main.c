#include <avr/io.h>

#include "util/delay.h"

#include "../app/led_sk6812.h"

#include "../device/adc.h"
#include "../device/buttons.h"
#include "../device/led.h"
#include "../device/interrupts.h"
#include "../device/tmr_tick.h"
#include "../device/uart_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint8_t pattern1[2] = {20, 0};
uint8_t pattern2[2] = {0, 20};

uint64_t pattern = 0x1;
uint8_t bright = 40;

int main(void)
{
    // init SRAND
    srand(time(NULL));

    // init rest of the system
    onboard_led_init();
    tmr_tick_init();
    uart_print_init();
    adc_init();
    btn_init();
    led_sk6812_init();
    interrupts_enable();

    while (1) {
        // 10ms tasks
        if (tmr_10ms_flag) {
            btn_task();
            led_sk6812_task();

            tmr_10ms_flag = 0;
        }

        // 1s tasks
        if (tmr_1s_flag) {

            tmr_1s_flag = 0;
        }

        
    }

    return 0;
}