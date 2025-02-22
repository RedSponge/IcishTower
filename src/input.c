#include "input.h"
#include "raylib.h"


static bool input_source_keycode_is_pressed(void* config) {
	return IsKeyPressed((int)config);
}

static bool input_source_keycode_is_released(void* config) {
	return IsKeyReleased((int)config);
}

static bool input_source_keycode_is_down(void* config) {
	return IsKeyDown((int)config);
}

static bool input_source_keycode_is_up(void* config) {
	return IsKeyUp((int)config);
}

void input_source_init_from_keycode(input_source_t* source, int key)
{
	source->config = key;
	source->is_pressed = input_source_keycode_is_pressed;
	source->is_released = input_source_keycode_is_released;
	source->is_down = input_source_keycode_is_down;
	source->is_up = input_source_keycode_is_up;
}

bool input_source_is_pressed(input_source_t* source)
{
	return source->is_pressed(source->config);
}

bool input_source_is_released(input_source_t* source)
{
	return source->is_released(source->config);
}

bool input_source_is_down(input_source_t* source)
{
	return source->is_down(source->config);
}

bool input_source_is_up(input_source_t* source)
{
	return source->is_up(source->config);
}

float input_axis_get_value(input_axis_t* axis)
{
	return (input_source_is_down(&axis->positive) ? 1 : 0) - (input_source_is_down(&axis->negative) ? 1 : 0);
}
