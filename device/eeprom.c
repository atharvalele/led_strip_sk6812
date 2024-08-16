#include <avr/io.h>
#include "eeprom.h"

void eeprom_write(uint8_t addr, uint8_t data)
{
    // Wait for completion of previous write
    while (EECR & (1 << EEPE));

    // Set up address and data registers
    EEAR = addr;
    EEDR = data;

    // Write 1 to EEMPE
    EECR |= (1 << EEMPE);

    // Start EEPROM write by setting EEPE
    EECR |= (1 << EEPE);
}

uint8_t eeprom_read(uint8_t addr)
{
    uint8_t data;

    // Wait for completion of previous write
    while (EECR & (1 << EEPE));

    // Set up address register
    EEAR = addr;

    // Start EEPROM read
    EECR |= (1 << EERE);

    // get data
    data = EEDR;
    return data;
}