#ifndef _TIMER_H
#define _TIMER_H

typedef enum {us, ms, sec} timeUnit_t;

void wait(uint16_t time, timeUnit_t unit);

#endif  // _TIMER_H
