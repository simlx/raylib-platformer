#include "raylib.h"

int main(void)
{
	InitWindow(320, 120, "raylib hello world");
	SetTargetFPS(30);

	while (!WindowShouldClose())
	{
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Hello World", 10, 10, 20, DARKGRAY);
        EndDrawing();
	}

    CloseWindow();

	return 0;
}
