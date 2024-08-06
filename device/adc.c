#include <avr/io.h>
#include <stdio.h>
#include "adc.h"

#define ADMUX_CH_MASK   0xF

void adc_init(void)
{
    // select AVcc as reference
    ADMUX |= (1 << REFS0);

    // Turn on ADC
    ADCSRA |= (1 << ADEN);

    // set 1:128 prescalar
    ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

    // disable ADC power reduction
    // PRR |= (1 << PRADC);

    // Disable digital inputs for ADC0-4
    DIDR0 = (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D)
            | (1 << ADC3D) | (1 << ADC4D);
}

uint16_t adc_get_ch(uint8_t ch)
{
    volatile uint8_t tmp_sfr;
    volatile uint16_t result;

    // select channel by setting ADC mux
    tmp_sfr = ADMUX;
    tmp_sfr &= ~ADMUX_CH_MASK;
    tmp_sfr |= ch;
    ADMUX = tmp_sfr;

    // start conversion
    ADCSRA |= (1 << ADSC);

    // wait for conversion complete
    while (ADCSRA & (1 << ADSC));

    // read result
    // note that ADCL must be read before ADCH
    result = ADCL | (ADCH << 8);

    return result;
}