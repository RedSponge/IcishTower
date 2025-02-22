#include "player.h"
#include <stdio.h>
#include <floor.h>

void controls_init_from_config(controls_t* controls, const controls_config_t* config)
{
	input_source_init_from_keycode(&controls->jump, config->jump_key);
	input_source_init_from_keycode(&controls->horizontal.positive, config->right_key);
	input_source_init_from_keycode(&controls->horizontal.negative, config->left_key);
}

float controls_get_horizontal(controls_t* controls)
{
	return input_axis_get_value(&controls->horizontal);
}

bool controls_get_jump_pressed(controls_t* controls)
{
	return input_source_is_pressed(&controls->jump);
}

bool controls_get_jump_down(controls_t* controls)
{
	return input_source_is_down(&controls->jump);
}

void player_init(player_t* player, const player_config_t *config, collision_manager_t *collision_manager)
{
	player->collision_agent = INVALID_COLLISION_AGENT;
	player->velocity = (Vector2){ 0, 0 };
	player->acceleration = (Vector2){ 0, 0 };

	controls_init_from_config(&player->controls, &config->controls);

	if (!collision_manager_register_agent(collision_manager, &player->collision_agent)) {
		fprintf(stderr, "Failed to register player collision agent!");
	}
	player->collision_manager = collision_manager;

	collision_manager_set_shape(collision_manager, player->collision_agent, (collision_shape_t) {
		.rectangle = {
			.x = GetScreenWidth() / 2,
			.y = GetScreenHeight() / 2,
			.width = 32,
			.height = 32,
		}
	});

	player->is_on_ground = false;
	player->jump_input_timer = (timer_t) { 0 };
	player->coyote_jump_timer = (timer_t){ 0 };
}

void player_clean(player_t* player)
{
	if (!collision_manager_free_agent(player->collision_manager, player->collision_agent)) {
		fprintf(stderr, "Failed to unregister collision agent!");
	}
	player->collision_agent = INVALID_COLLISION_AGENT;
}

void player_get_position(player_t* player, Vector2* position)
{
	collision_shape_t to_draw = { 0 };
	collision_manager_get_shape(player->collision_manager, player->collision_agent, &to_draw);

	position->x = to_draw.rectangle.x + to_draw.rectangle.width / 2;
	position->y = to_draw.rectangle.y + to_draw.rectangle.height / 2;
}

void player_draw(player_t* player)
{
	collision_shape_t to_draw = { 0 };
	collision_manager_get_shape(player->collision_manager, player->collision_agent, &to_draw);

	DrawRectangle((int)to_draw.rectangle.x, (int)to_draw.rectangle.y, to_draw.rectangle.width, to_draw.rectangle.height, player->is_on_ground ? GREEN : BLUE);
}

void player_update(player_t* player, float delta)
{
	player->acceleration.x = 400 * controls_get_horizontal(&player->controls);
	player->velocity.y += 800 * delta;

	bool is_jump_pressed = controls_get_jump_pressed(&player->controls);
	bool is_jump_held = controls_get_jump_down(&player->controls);

	if (is_jump_pressed) {
		timer_set(&player->jump_input_timer, 0.5f, TIMER_MODE_ONCE);
	}

	bool jump_timer_not_done = !timer_advance(&player->jump_input_timer, delta);
	bool coyote_timer_not_done = !timer_advance(&player->coyote_jump_timer, delta);

	if (jump_timer_not_done && coyote_timer_not_done) {
		player->velocity.y = -1000;
		timer_reset(&player->jump_input_timer);
		timer_reset(&player->coyote_jump_timer);
	}

	// If rising without holding jump, accelerate descent.
	if (player->velocity.y < 0 && !is_jump_held) {
		player->velocity.y += 1200 * delta;
	}
	else if (player->velocity.y > 0) {
		player->velocity.y += 600 * delta;
	}

	player->is_on_ground = false;

	player->velocity.x = player->acceleration.x;
	//player->velocity.y = player->acceleration.y;

	collision_t collision = { 0 };

	/*if (player->velocity.y < 0) {
		collision_manager_set_colliding(player->collision_manager, player->collision_agent, false);
	}*/

	if (!collision_manager_move_x(player->collision_manager, player->collision_agent, player->velocity.x * delta, &collision)) {
		if (collision.with_agent.collidee_data != NULL) {
			int floor_agent = ((floor_t*)collision.with_agent.collidee_data)->collision_agent;

			int floor_y = collision.with_agent.collidee->rectangle.y;
			int self_y = collision.with_agent.collider->rectangle.y;
			int self_height = collision.with_agent.collider->rectangle.height;

			if (floor_y > self_y && floor_y < self_y + self_height) {
				collision_manager_set_colliding(player->collision_manager, floor_agent, false);
				collision_manager_move_x(player->collision_manager, player->collision_agent, collision.with_agent.leftover_to_move, &collision);
				collision_manager_set_colliding(player->collision_manager, floor_agent, true);
			}
		}
	}

	if (!collision_manager_move_y(player->collision_manager, player->collision_agent, player->velocity.y * delta, &collision)) {
		if (collision.with_agent.collidee_data != NULL) {

			int floor_agent = ((floor_t*)collision.with_agent.collidee_data)->collision_agent;

			int floor_y = collision.with_agent.collidee->rectangle.y;
			int self_y = collision.with_agent.collider->rectangle.y;
			int self_height = collision.with_agent.collider->rectangle.height;

			if (floor_y > self_y && floor_y < self_y + self_height || player->velocity.y < 0) {

				collision_manager_set_colliding(player->collision_manager, floor_agent, false);
				collision_manager_move_y(player->collision_manager, player->collision_agent, collision.with_agent.leftover_to_move, &collision);
				collision_manager_set_colliding(player->collision_manager, floor_agent, true);
			}
			else {
				// Sufficiently high to make sure (vely * delta ) > 1 for the next iteration
				// to keep is_on_ground always correct
				player->velocity.y = 100;

				player->is_on_ground = true;
				timer_set(&player->coyote_jump_timer, 0.3f, TIMER_MODE_ONCE);
			}
		}
	}

	/*if (player->velocity.y < 0) {
		collision_manager_set_colliding(player->collision_manager, player->collision_agent, true);
	}*/
}
