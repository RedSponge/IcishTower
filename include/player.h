#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "raylib.h"
#include "input.h"
#include "timer.h"

#include "collision_manager.h"

typedef struct _controls_config_t {
	int left_key;
	int right_key;
	int jump_key;
} controls_config_t;

typedef struct _controls_t {
	input_axis_t horizontal;
	input_source_t jump;
} controls_t;

void controls_init_from_config(controls_t* controls, const controls_config_t* config);

float controls_get_horizontal(controls_t* controls);
bool controls_get_jump_pressed(controls_t* controls);
bool controls_get_jump_down(controls_t* controls);

typedef struct _player_t {
	collision_manager_t* collision_manager;
	int collision_agent;

	Vector2 acceleration;
	Vector2 velocity;

	controls_t controls;
	bool is_on_ground;

	timer_t jump_input_timer;
	timer_t coyote_jump_timer;
} player_t;

typedef struct _player_config_t {
	controls_config_t controls;
} player_config_t;

void player_init(player_t* player, const player_config_t *config, collision_manager_t *collision_manager);
void player_clean(player_t* player);

void player_get_position(player_t* player, Vector2* position);

void player_draw(player_t* player);
void player_update(player_t* player, float delta);

#endif // __PLAYER_H__