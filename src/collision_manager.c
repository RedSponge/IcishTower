#include "collision_manager.h"

#include <stdio.h>
#include <stdlib.h>

#include "raymath.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(n) ((sizeof(n)) / sizeof(*n))
#endif

#define GET_AGENT_IF_VALID_AND_DELEGATED(manager, agent_id) (0 <= (agent_id) && (agent_id) < MAX_COLLISION_AGENTS) ? (((manager).agents[agent_id].delegated) ? &(manager).agents[agent_id] : NULL) : NULL

static int signum(int f) {
	return (f > 0 ? 1 : 0) - (f < 0 ? 1 : 0);
}

void collision_manager_init(collision_manager_t* manager)
{
	*manager = (collision_manager_t){ 0 };
}

void collision_manager_clean(collision_manager_t* manager)
{
}

bool collision_manager_register_agent(collision_manager_t* manager, int* agent_id)
{
	// O(n) for now because n is small and this will be rarely used.
	// may be a bottleneck later down the line.

	for (size_t i = 0; i < ARRAY_SIZE(manager->agents); i++) {
		if (!manager->agents[i].delegated) {
			*agent_id = i;
			manager->agents[i].delegated = true;

			manager->agents[i].colliding = true;
			manager->agents[i].shape = (collision_shape_t) { 0 };
			manager->agents[i].data = NULL;
			manager->agents[i].delta = (Vector2){ 0, 0 };

			int counter = 0;
			for (size_t i = 0; i < ARRAY_SIZE(manager->agents); i++) {
				if (manager->agents[i].delegated) counter++;
			}

			printf("[collision_manager_register_agent] delegated agents: %d\n", counter);

			return true;
		}
	}

	return false;
}

bool collision_manager_free_agent(collision_manager_t* manager, int agent_id)
{
	if (!manager->agents[agent_id].delegated) {
		return false;
	}

	manager->agents[agent_id].delegated = false;
	int counter = 0;
	for (size_t i = 0; i < ARRAY_SIZE(manager->agents); i++) {
		if (manager->agents[i].delegated) counter++;
	}

	printf("[collision_manager_free_agent] delegated agents: %d\n", counter);
	return true;
}

bool collision_manager_set_shape(collision_manager_t* manager, int agent_id, collision_shape_t shape)
{
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}

	agent->shape = shape;

	return true;
}

bool collision_manager_get_shape(collision_manager_t* manager, int agent_id, collision_shape_t* shape)
{
	
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}
	
	*shape = agent->shape;

	return true;
}

bool collision_manager_set_data(collision_manager_t* manager, int agent_id, void* data)
{
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}

	agent->data = data;

	return true;
}

bool collision_manager_get_data(collision_manager_t* manager, int agent_id, void** data)
{
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}

	*data = agent->data;

	return true;
}

bool collision_manager_set_colliding(collision_manager_t* manager, int agent_id, bool colliding)
{
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}

	agent->colliding = colliding;

	return true;
}

bool collision_manager_is_colliding(collision_manager_t* manager, int agent_id, bool* is_colliding)
{
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}

	*is_colliding = agent->colliding;

	return true;
}

static bool collision_manager_check_collision(collision_manager_t* manager, const Rectangle* rectangle, int self_to_skip, int *collided_with) {
	for (int i = 0; i < ARRAY_SIZE(manager->agents); i++) {
		if (i == self_to_skip) continue;

		collision_agent_t* agent = &manager->agents[i];
		if (!agent->delegated) continue;
		if (!agent->colliding) continue;

		if (CheckCollisionRecs(*rectangle, agent->shape.rectangle)) {
			*collided_with = i;
			return true;
		}
	}

	return false;
} 

static bool collision_manager_move_x_exact(collision_manager_t* manager, int agent_id, int to_move, collision_t* collision) {
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}

	int sign = signum(to_move);
	to_move = abs(to_move);

	int collided_with = INVALID_COLLISION_AGENT;

	while (to_move > 0) {
		agent->shape.rectangle.x += sign;
		if (agent->colliding && collision_manager_check_collision(manager, &agent->shape.rectangle, agent_id, &collided_with)) {
			agent->shape.rectangle.x -= sign;

			collision->type = WITH_AGENT;
			collision->with_agent.collider = &agent->shape;
			collision->with_agent.collidee = &manager->agents[collided_with].shape;
			collision->with_agent.collider_data = agent->data;
			collision->with_agent.collidee_data = manager->agents[collided_with].data;
			collision->with_agent.leftover_to_move = to_move * sign;

			return false;
		}
		to_move--;
	}

	return true;
}

bool collision_manager_unchecked_translate(collision_manager_t* manager, int agent_id, Vector2 to_move)
{
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}

	agent->shape.rectangle.x += to_move.x;
	agent->shape.rectangle.y += to_move.y;

	return true;
}

bool collision_manager_move_x(collision_manager_t* manager, int agent_id, float to_move, collision_t *collision)
{
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}

	agent->delta.x += to_move;

	int amount = (int) floorf(agent->delta.x);
	agent->delta.x -= amount;

	return collision_manager_move_x_exact(manager, agent_id, amount, collision);
}

static bool collision_manager_move_y_exact(collision_manager_t* manager, int agent_id, int to_move, collision_t* collision) {
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}

	int sign = signum(to_move);
	to_move = abs(to_move);

	int collided_with = INVALID_COLLISION_AGENT;

	while (to_move > 0) {
		agent->shape.rectangle.y += sign;
		if (agent->colliding && collision_manager_check_collision(manager, &agent->shape.rectangle, agent_id, &collided_with)) {
			agent->shape.rectangle.y -= sign;

			collision->type = WITH_AGENT;
			collision->with_agent.collider = &agent->shape;
			collision->with_agent.collidee = &manager->agents[collided_with].shape;
			collision->with_agent.collider_data = agent->data;
			collision->with_agent.collidee_data = manager->agents[collided_with].data;
			collision->with_agent.leftover_to_move = to_move * sign;

			return false;
		}

		to_move--;
	}

	return true;
}

bool collision_manager_move_y(collision_manager_t* manager, int agent_id, float to_move, collision_t* collision)
{
	collision_agent_t* agent = GET_AGENT_IF_VALID_AND_DELEGATED(*manager, agent_id);
	if (agent == NULL) {
		return false;
	}

	agent->delta.y += to_move;

	int amount = floorf(agent->delta.y);
	agent->delta.y -= amount;

	return collision_manager_move_y_exact(manager, agent_id, amount, collision);
}

void collision_manager_debug_draw(const collision_manager_t* manager) 
{
	for (size_t i = 0; i < ARRAY_SIZE(manager->agents); i++) {
		if (!manager->agents[i].delegated) continue;
		Rectangle rect = manager->agents[i].shape.rectangle;
		Color color = manager->agents[i].colliding ? GOLD : GRAY;

		DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, color);
	}
}