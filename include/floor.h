#ifndef __FLOOR_H__
#define __FLOOR_H__

#include <stdbool.h>

#include "raylib.h"
#include "collision_manager.h"

typedef struct _floor_t {
	collision_manager_t* collision_manager;
	int collision_agent;

	int number;
	bool show_number;
} floor_t;

void floor_init(floor_t* floor, collision_manager_t* collision_manager);
void floor_clean(floor_t* floor);

void floor_set(floor_t* floor, int number, Rectangle rectangle);

void floor_update(floor_t* floor, float delta);
void floor_draw(floor_t* floor);

#endif // __FLOOR_H__