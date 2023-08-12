#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Include this for INFINITY

typedef struct {
    char x[25];
    float* y_values;
    int y_count;
} Point;

Point* points = NULL;
int pointsCount = 0;

int getAxisIncrement(float range) {
    if (range <= 10) return 1;
    else if (range <= 100) return 5;
    else if (range <= 500) return 50;
    else return 100; // default increment for very large ranges
}

/**
 * @brief Loads data from a CSV file into the points array.
 * 
 * @param filename Name of the CSV file to load.
 */
void LoadCSVData(const char* filename, float* maxYValue, float* minYValue) {
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

    // Calculate maxYValue and minYValue after loading the data
    *maxYValue = -INFINITY;
    *minYValue = INFINITY;
    for (int i = 0; i < pointsCount; i++) {
        for (int j = 0; j < points[i].y_count; j++) {
            if (points[i].y_values[j] > *maxYValue) *maxYValue = points[i].y_values[j];
            if (points[i].y_values[j] < *minYValue) *minYValue = points[i].y_values[j];
        }
    }

    // Adjust minYValue to be 0 if it's greater than 0
    if (*minYValue > 0) {
        *minYValue = 0;
    }
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
    const int LABEL_SKIP = 1;

    InitWindow(screenWidth, screenHeight, "raylib CSV scatter plot");
    SetTargetFPS(60);

    float maxYValue = -INFINITY;
    float minYValue = INFINITY;

    while (!WindowShouldClose()) {
        if (IsFileDropped()) {
            ClearPreviousData();

            FilePathList droppedFilesList = LoadDroppedFiles();
            if (droppedFilesList.count > 0) {
                LoadCSVData(droppedFilesList.paths[0], &maxYValue, &minYValue);
            }
        }

        const int offset = 30;
        float scale = (screenHeight - 2 * offset) / (maxYValue - minYValue); // Define the scale here

        BeginDrawing();
        ClearBackground(RAYWHITE);

        float xSpacing = (float)screenWidth / (pointsCount + 1); // Calculate spacing based on number of data points

        // Draw the scatter plot points with adjusted x-axis scaling
        for (int i = 0; i < pointsCount; i++) {
            for (int j = 0; j < points[i].y_count; j++) {
                Color pointColor = colors[j % (sizeof(colors) / sizeof(colors[0]))];
                float scaledY = screenHeight - offset - (points[i].y_values[j] - minYValue) * scale;
                DrawCircle((i + 1) * xSpacing, scaledY, 5, pointColor);
            }
        }

        // Draw x-axis labels with timestamps using adjusted x-axis scaling
        for (int i = 0; i < pointsCount; i += LABEL_SKIP) {
            Vector2 position = {(i + 1) * xSpacing, screenHeight - offset + 10};
            DrawTextEx(GetFontDefault(), points[i].x, position, 10, 1, GRAY);
        }

        DrawLine(0, screenHeight - offset, screenWidth, screenHeight - offset, GRAY);
        DrawLine(0, 0, 0, screenHeight - offset, GRAY);

        int maxY = 450;
        int interval = 50;

        // Adjusted y-axis scale
        if (pointsCount > 0) { // Only draw y-axis labels if data points have been loaded
            float range = maxYValue - minYValue;
            int yAxisIncrement = getAxisIncrement(range);
            int numLabels = range / yAxisIncrement;
            for (int i = 0; i <= numLabels; i++) {
                float value = minYValue + i * yAxisIncrement;
                float yPosition = screenHeight - offset - (value - minYValue) * scale;
                DrawLine(0, yPosition, screenWidth, yPosition, LIGHTGRAY); // Draw horizontal lines across the graph
                DrawLine(-5, yPosition, 5, yPosition, GRAY);
                DrawText(TextFormat("%.2f", value), 10, yPosition - 5, 10, GRAY);
            }
        }

        EndDrawing();
    }

    ClearPreviousData();
    CloseWindow();

    return 0;
}
