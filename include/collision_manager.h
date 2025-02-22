#ifndef __COLLISION_MANAGER_H__
#define __COLLISION_MANAGER_H__

#ifndef MAX_COLLISION_AGENTS
#define MAX_COLLISION_AGENTS (32)
#endif

#include "raylib.h"
#include <stdbool.h>

#define INVALID_COLLISION_AGENT (-1)

typedef struct _collision_shape_t {
	Rectangle rectangle;
} collision_shape_t;

typedef enum _collision_type_t {
	NO_COLLISION = 0,
	WITH_AGENT = 1,
} collision_type_t;

typedef struct _collision_t {
	collision_type_t type;
	union collision {
		struct {
			collision_shape_t* collider;
			void* collider_data;
			collision_shape_t* collidee;
			void* collidee_data;
			int leftover_to_move;
			Rectangle collisionArea;
		} with_agent;
	};
} collision_t;

typedef struct _collision_agent_t {
	collision_shape_t shape;
	void* data;
	bool colliding;

	Vector2 delta;
	bool delegated;
} collision_agent_t;

typedef struct _collision_manager_t {
	collision_agent_t agents[MAX_COLLISION_AGENTS];
} collision_manager_t;

void collision_manager_init(collision_manager_t* manager);
void collision_manager_clean(collision_manager_t* manager);


bool collision_manager_register_agent(collision_manager_t *manager, int *agent_id);
bool collision_manager_free_agent(collision_manager_t* manager, int agent_id);

bool collision_manager_set_shape(collision_manager_t* manager, int agent_id, collision_shape_t shape);
bool collision_manager_get_shape(collision_manager_t* manager, int agent_id, collision_shape_t* shape);

bool collision_manager_set_data(collision_manager_t* manager, int agent_id, void *data);
bool collision_manager_get_data(collision_manager_t* manager, int agent_id, void** data);

bool collision_manager_set_colliding(collision_manager_t* manager, int agent_id, bool colliding);
bool collision_manager_is_colliding(collision_manager_t* manager, int agent_id, bool* is_colliding);

bool collision_manager_unchecked_translate(collision_manager_t* manager, int agent_id, Vector2 to_move);

bool collision_manager_move_x(collision_manager_t* manager, int agent_id, float amount, collision_t *collision);
bool collision_manager_move_y(collision_manager_t* manager, int agent_id, float amount, collision_t *collision);

void collision_manager_debug_draw(const collision_manager_t* manager);

#endif