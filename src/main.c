#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct Point
 * @brief Represents a 2D point with x as timestamp and y-values.
 */
typedef struct {
    char x[25];
    float* y_values;
    int y_count;
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

    char line[512];
    fgets(line, sizeof(line), file); // Read the header line
    int columns = 0;
    char* token = strtok(line, ",");
    while (token) {
        columns++;
        token = strtok(NULL, ",");
    }
    printf("Detected %d columns in CSV.\n", columns);

    while (fgets(line, sizeof(line), file)) {
        Point point;
        token = strtok(line, ",");
        sscanf(token, "%*d-%*d-%*d %2s:%2s", &point.x[0], &point.x[3]);
        point.x[2] = ':';
        point.x[5] = '\0';
        printf("Parsed timestamp: %s\n", point.x);

        point.y_count = columns - 1;
        point.y_values = (float*)malloc(point.y_count * sizeof(float));
        if (!point.y_values) {
            perror("Failed to allocate memory for y_values");
            fclose(file);
            return;
        }

        for (int i = 0; i < point.y_count; i++) {
            token = strtok(NULL, ",");
            point.y_values[i] = atof(token);
            printf("Parsed y_value[%d]: %f\n", i, point.y_values[i]);
        }

        points = realloc(points, (pointsCount + 1) * sizeof(Point));
        if (!points) {
            perror("Failed to allocate memory for points");
            fclose(file);
            return;
        }
        points[pointsCount++] = point;
    }
    printf("Finished loading CSV data. Total points: %d\n", pointsCount);
    fclose(file);
}

void ClearPreviousData() {
    printf("Clearing previous data...\n");
    for (int i = 0; i < pointsCount; i++) {
        free(points[i].y_values);
    }
    free(points);
    points = NULL;
    pointsCount = 0;
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const Color colors[] = {DARKBLUE, RED, GREEN, YELLOW, PURPLE, ORANGE, PINK, LIME, GOLD, DARKBROWN};
    const int LABEL_SKIP = 1; // Display every label

    // Initialize raylib window
    InitWindow(screenWidth, screenHeight, "raylib CSV scatter plot");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Check for file drop and load data
        if (IsFileDropped()) {
            printf("File drop detected.\n");

            ClearPreviousData(); // Clear any previously loaded data

            FilePathList droppedFilesList = LoadDroppedFiles();
            if (droppedFilesList.count > 0) {
                printf("Loading file: %s\n", droppedFilesList.paths[0]);
                LoadCSVData(droppedFilesList.paths[0]);
            }
        }

        const int offset = 30; // Offset from the bottom for drawing

        BeginDrawing();
        ClearBackground(RAYWHITE);

        printf("Starting to draw scatter plot points...\n");
        // Draw the scatter plot points
        for (int i = 0; i < pointsCount; i++) {
            for (int j = 0; j < points[i].y_count; j++) {
                Color pointColor = colors[j % (sizeof(colors) / sizeof(colors[0]))];
                DrawCircle((i + 1) * 100, screenHeight - points[i].y_values[j] - offset, 5, pointColor);
            }
        }
        printf("Finished drawing scatter plot points.\n");

        printf("Starting to draw x-axis labels...\n");
        // Draw x-axis labels with timestamps
        for (int i = 0; i < pointsCount; i += LABEL_SKIP) {
            Vector2 position = {(i + 1) * 100, screenHeight - offset + 10};
            DrawTextEx(GetFontDefault(), points[i].x, position, 10, 1, GRAY);
        }
        printf("Finished drawing x-axis labels.\n");

        // Draw x and y axes
        DrawLine(0, screenHeight - offset, screenWidth, screenHeight - offset, GRAY);
        DrawLine(0, 0, 0, screenHeight - offset, GRAY);

        // Define scale parameters
        int maxY = 450;
        int interval = 50;

        // Draw y-axis scale
        for (int i = 0; i <= maxY; i += interval) {
            DrawLine(-5, screenHeight - i - offset, 5, screenHeight - i - offset, GRAY);
            DrawText(TextFormat("%d", i), 10, screenHeight - i - offset - 5, 10, GRAY);
        }

        printf("Rendering frame...\n");

        EndDrawing();
    }

    // Cleanup
    ClearPreviousData();
    CloseWindow();

    return 0;
}
