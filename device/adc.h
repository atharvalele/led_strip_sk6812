#ifndef DEV_ADC_H
#define DEV_ADC_H

#include <stdint.h>

void adc_init();
uint16_t adc_get_ch(uint8_t ch);

#endif // DEV_ADC_H