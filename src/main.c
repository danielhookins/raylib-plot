#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct Point
 * @brief Represents a 2D point with x and y coordinates.
 */
typedef struct {
    float x;
    float y;
} Point;

// Global variables to store the points and their count
Point* points = NULL;
int pointsCount = 0;

/**
 * @brief Loads data from a CSV file into the points array.
 * 
 * @param filename Name of the CSV file to load.
 */
void LoadCSVData(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char line[128];
    fgets(line, sizeof(line), file); // Skip the header line

    while (fgets(line, sizeof(line), file)) {
        Point point;
        if (sscanf(line, "%f,%f", &point.x, &point.y) == 2) {
            points = realloc(points, (pointsCount + 1) * sizeof(Point));
            if (!points) {
                perror("Failed to allocate memory");
                fclose(file);
                return;
            }
            points[pointsCount++] = point;
        }
    }
    fclose(file);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Initialize raylib window
    InitWindow(screenWidth, screenHeight, "raylib CSV scatter plot");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Check for file drop and load data
        if (IsFileDropped()) {
            FilePathList droppedFilesList = LoadDroppedFiles();
            if (droppedFilesList.count > 0) {
                LoadCSVData(droppedFilesList.paths[0]);
            }
        }

        const int offset = 30; // Offset from the bottom for drawing

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the scatter plot points
        for (int i = 0; i < pointsCount; i++) {
            DrawCircle(points[i].x, screenHeight - points[i].y - offset, 5, DARKBLUE);
        }

        // Draw x and y axes
        DrawLine(0, screenHeight - offset, screenWidth, screenHeight - offset, GRAY);
        DrawLine(0, 0, 0, screenHeight - offset, GRAY);

        // Define scale parameters
        int maxX = 800;
        int maxY = 450;
        int interval = 50;

        // Draw x-axis scale
        for (int i = 0; i <= maxX; i += interval) {
            DrawLine(i, screenHeight - offset - 5, i, screenHeight - offset + 5, GRAY);
            DrawText(TextFormat("%d", i), i - 10, screenHeight - offset + 10, 10, GRAY);
        }

        // Draw y-axis scale
        for (int i = 0; i <= maxY; i += interval) {
            DrawLine(-5, screenHeight - i - offset, 5, screenHeight - i - offset, GRAY);
            DrawText(TextFormat("%d", i), 10, screenHeight - i - offset - 5, 10, GRAY);
        }

        EndDrawing();
    }

    // Cleanup
    free(points);
    CloseWindow();

    return 0;
}
