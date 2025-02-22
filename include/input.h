#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdbool.h>

typedef struct _input_source_t {
	void* config;
	bool (*is_pressed)(void*);
	bool (*is_released)(void*);
	bool (*is_down)(void*);
	bool (*is_up)(void*);
} input_source_t;

void input_source_init_from_keycode(input_source_t* source, int key);

bool input_source_is_pressed(input_source_t* source);
bool input_source_is_released(input_source_t* source);
bool input_source_is_down(input_source_t* source);
bool input_source_is_up(input_source_t* source);


typedef struct _input_axis_t {
	input_source_t negative;
	input_source_t positive;
} input_axis_t;

float input_axis_get_value(input_axis_t* axis);

#endif // __INPUT_H__