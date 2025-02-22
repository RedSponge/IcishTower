#include "scene.h"
#include "raymath.h"
#include "project_utils.h"
#include <stdlib.h>
#include <stdio.h>

void scene_spawn_floor(scene_t* scene, int index, int floor_id);

int randint(int min, int max) {
	return (rand() % (max - min)) + min;
}

void scene_init(scene_t* scene)
{
	scene->width = GetScreenWidth();
	scene->height = GetScreenHeight();
	scene->gutter_width = 32;

	collision_manager_init(&scene->collision_manager);

	const player_config_t player_config = {
		.controls = {
			.jump_key = KEY_SPACE,
			.right_key = KEY_RIGHT,
			.left_key = KEY_LEFT,
		}
	};

	player_init(&scene->player, &player_config, &scene->collision_manager);
	
	scene->camera = (Camera2D){
		.zoom = 1.0,
		.offset = (Vector2){ 0, 0 },
		.target = (Vector2){ 0, 0 },
		.rotation = 0.0,
	};

	scene->scroll_speed = 50;
	scene->last_spawned_floor = 0;

	
	//scene->floors = (floor_t[10]){ 0 };

	for (int i = 0; i < ARRAY_SIZE(scene->floors); i++) {
		floor_init(&scene->floors[i], &scene->collision_manager);
		floor_set(&scene->floors[i], 0, (Rectangle) {0, scene->height - 30.f, scene->width, 1 });
	}

	for (int i = 1; i < ARRAY_SIZE(scene->floors); i++) {
		scene->last_spawned_floor++;
		scene_spawn_floor(scene, i, scene->last_spawned_floor);
	}

	collision_manager_register_agent(&scene->collision_manager, &scene->left_gutter_agent);
	collision_manager_register_agent(&scene->collision_manager, &scene->right_gutter_agent);

	collision_manager_set_shape(&scene->collision_manager, scene->left_gutter_agent, (collision_shape_t) {
		.rectangle = (Rectangle){ 0, 0, 10, scene->height },
	});
	collision_manager_set_shape(&scene->collision_manager, scene->right_gutter_agent, (collision_shape_t) {
		.rectangle = (Rectangle){ scene->width - 10, 0, 10, scene->height },
	});
}

void scene_spawn_floor(scene_t* scene, int index, int floor_id) {
	float max_y = scene->height;
	collision_shape_t shape = { 0 };
	for (size_t i = 0; i < ARRAY_SIZE(scene->floors); i++) {
		collision_manager_get_shape(&scene->collision_manager, scene->floors[i].collision_agent, &shape);
		if (shape.rectangle.y < max_y) {
			max_y = shape.rectangle.y;
		}
	}

	int width = randint(10, 20) * 10;
	int x = randint(0, (scene->width - width) / 10) * 10;

	if (floor_id % 50 == 0) {
		width = scene->width;
		x = 0;
	}

	floor_set(&scene->floors[index], floor_id, (Rectangle) { x, max_y - 80.f, width, 1 });
}

void scene_clean(scene_t* scene)
{
	player_clean(&scene->player);

	for (size_t i = 0; i < ARRAY_SIZE(scene->floors); i++) {
		floor_clean(&scene->floors[i]);
	}

	collision_manager_free_agent(&scene->collision_manager, scene->left_gutter_agent);
	collision_manager_free_agent(&scene->collision_manager, scene->right_gutter_agent);
}

static void camera_follow_player(scene_t* scene, float delta) {
	Vector2 pos = (Vector2){ 0, 0 };
	player_get_position(&scene->player, &pos);
}

void scene_update(scene_t* scene, float delta)
{
	float cam_shift = scene->scroll_speed * delta;
	collision_shape_t floor_shape = { 0 };
	Vector2 player_pos = { 0 };
	player_get_position(&scene->player, &player_pos);


	if (player_pos.y < scene->height / 2) {
		cam_shift += scene->height / 2 - player_pos.y;
	}

	for (size_t i = 0; i < ARRAY_SIZE(scene->floors); i++) {
		collision_manager_unchecked_translate(&scene->collision_manager, scene->floors[i].collision_agent, (Vector2) { 0, cam_shift });
	}
	collision_manager_unchecked_translate(&scene->collision_manager, scene->player.collision_agent, (Vector2) { 0, cam_shift });

	for (size_t i = 0; i < ARRAY_SIZE(scene->floors); i++) {
		collision_manager_get_shape(&scene->collision_manager, scene->floors[i].collision_agent, &floor_shape);

		if (floor_shape.rectangle.y > scene->height) {
			scene->last_spawned_floor++;
			scene_spawn_floor(scene, i, scene->last_spawned_floor);
		}
	}

	for (size_t i = 0; i < ARRAY_SIZE(scene->floors); i++) {
		floor_update(&scene->floors[i], delta);
	}
	player_update(&scene->player, delta);
}

void scene_draw(scene_t* scene)
{
	ClearBackground(RAYWHITE);
	BeginMode2D(scene->camera);
	collision_manager_debug_draw(&scene->collision_manager);
	for (size_t i = 0; i < ARRAY_SIZE(scene->floors); i++) {
		floor_draw(&scene->floors[i]);
	}
	player_draw(&scene->player);

	EndMode2D();
}
