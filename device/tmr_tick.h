#ifndef DEV_TMR_TICK_H
#define DEV_TMR_TICK_H

extern volatile uint8_t tmr_10ms_flag;

void tmr_tick_init(void);

#endif // DEV_TMR_TICK_H