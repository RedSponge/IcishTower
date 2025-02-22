#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdbool.h>

typedef enum _timer_mode_t {
	TIMER_MODE_ONCE,
	TIMER_MODE_LOOP,
} timer_mode_t;

typedef struct _timer_t {
	timer_mode_t mode;
	float counter;
	float length;
} timer_t;

void timer_set(timer_t* timer, float length, timer_mode_t mode);

void timer_reset(timer_t* timer);
bool timer_advance(timer_t* timer, float delta);
bool timer_is_done(timer_t* timer);

#endif // __TIMER_H__