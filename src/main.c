#include "raylib.h"

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib dataset plot");

    // Dataset
    Vector2 points[] = {
        {10, 10},
        {20, 20}
    };
    int pointsCount = sizeof(points) / sizeof(Vector2);

    SetTargetFPS(60); // Set desired framerate (frames-per-second)

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update

        // Draw
        BeginDrawing();

            ClearBackground(RAYWHITE);

            for (int i = 0; i < pointsCount; i++)
            {
                DrawCircleV(points[i], 5, DARKBLUE); // Draw each point as a circle
            }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}

