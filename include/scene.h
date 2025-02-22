#ifndef __SCENE_H__
#define __SCENE_H__

#include "player.h"
#include "floor.h"

typedef struct _scene_t {
	collision_manager_t collision_manager;
	player_t player;
	floor_t floors[10];
	Camera2D camera;
	float scroll_speed;
	int last_spawned_floor;

	// TODO: Add gutters
	int left_gutter_agent;
	int right_gutter_agent;

	int width;
	int height;
	int gutter_width;
} scene_t;

void scene_init(scene_t* scene);
void scene_clean(scene_t* scene);

void scene_update(scene_t* scene, float delta);
void scene_draw(scene_t* scene);

#endif // __SCENE_H__