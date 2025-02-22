#include "raylib.h"
#include "resource_dir.h"
#include "scene.h"
#include "input.h"
#include "collision_manager.h"
#include <stdlib.h>

int main ()
{
	srand(time(NULL));
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	InitWindow(600, 600, "Icish Tower");
	SearchAndSetResourceDir("resources");


	SetTargetFPS(60);

	scene_t scene;
	scene_init(&scene);
	
	while (!WindowShouldClose())
	{
		scene_update(&scene, GetFrameTime());

		BeginDrawing();
		scene_draw(&scene);
		EndDrawing();
	}

	scene_clean(&scene);

	CloseWindow();
	return 0;
}
