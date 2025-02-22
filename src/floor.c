#include "floor.h"

#include <stdio.h.>

void floor_init(floor_t* floor, collision_manager_t* collision_manager)
{
	floor->number = -1;
	floor->show_number = false;

	floor->collision_agent = INVALID_COLLISION_AGENT;
	floor->collision_manager = collision_manager;

	if (!collision_manager_register_agent(collision_manager, &floor->collision_agent)) {
		fprintf(stderr, __FILE__ " %d Failed to register collision agent!", __LINE__);
	}

	collision_manager_set_data(collision_manager, floor->collision_agent, floor);
}

void floor_clean(floor_t* floor)
{
	collision_manager_free_agent(floor->collision_manager, floor->collision_agent);
	floor->collision_agent = INVALID_COLLISION_AGENT;
}

void floor_set(floor_t* floor, int number, Rectangle rectangle) {
	if (!collision_manager_set_shape(floor->collision_manager, floor->collision_agent, (collision_shape_t) {
		.rectangle = rectangle
	})) {
		fprintf(stderr, __FILE__ " %d Failed to set collision shape!", __LINE__);
	}

	floor->number = number;
	floor->show_number = number % 10 == 0;
}

void floor_update(floor_t* floor, float delta)
{
}

static void floor_draw_number_sign(int x, int y, int number) {
	int sign_width = 35;
	int sign_height = 30;
	
	DrawRectangle(x - sign_width / 2, y - sign_height / 2, sign_width, sign_height, GOLD);
	
	Font font = GetFontDefault();
	const char* text = TextFormat("%d", number);
	int font_size = 14;
	int spacing = 2;

	Vector2 text_size = MeasureTextEx(font, text, font_size, spacing);

	DrawTextEx(font, text, (Vector2){x - text_size.x / 2, y - text_size.y / 2}, font_size, spacing, WHITE);
}

void floor_draw(floor_t* floor)
{
	collision_shape_t shape = {0};
	collision_manager_get_shape(floor->collision_manager, floor->collision_agent, &shape);

	DrawRectangle(shape.rectangle.x, shape.rectangle.y, shape.rectangle.width, shape.rectangle.height, MAROON);

	if (floor->show_number) {
		floor_draw_number_sign(shape.rectangle.x + shape.rectangle.width / 2, shape.rectangle.y + shape.rectangle.height, floor->number);
	}
}
