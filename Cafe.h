#pragma once

// Draws a cafe building at the given position with all parameters and label
void drawCafe(
    float x, float y, float z,
    float width, float height, float depth,
    float r, float g, float b,
    int windowsX, int windowsZ_front, int windowsZ_side, int floors,
    const char* label
);