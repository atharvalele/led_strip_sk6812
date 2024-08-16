#ifndef DEV_EEPROM_H
#define DEV_EEPROM_H

#include <stdint.h>

void eeprom_write(uint8_t addr, uint8_t data);
uint8_t eeprom_read(uint8_t addr);

#endif  // DEV_EEPROM_H