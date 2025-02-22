#include "timer.h"

void timer_set(timer_t* timer, float length, timer_mode_t mode)
{
	*timer = (timer_t){
		.counter = 0,
		.length = length,
		.mode = mode,
	};
}

void timer_reset(timer_t* timer)
{
	timer_set(timer, 0, timer->mode);
}

bool timer_advance(timer_t* timer, float delta)
{
	timer->counter += delta;

	if (timer->counter >= timer->length) {
		switch (timer->mode) {
		case TIMER_MODE_LOOP:
			timer->counter -= timer->length;
			return true;
		case TIMER_MODE_ONCE:
			return true;
		}
	}

	return false;
}

bool timer_is_done(timer_t* timer)
{
	switch (timer->mode) {
	case TIMER_MODE_ONCE:
		return timer->counter >= timer->length;
	case TIMER_MODE_LOOP:
		return false;
	default:
		return false;
	}
}
