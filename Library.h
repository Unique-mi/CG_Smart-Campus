#pragma once

// Draws the central library at the given position with all parameters and label
void drawLibrary(
    float x, float y, float z,
    float width, float height, float depth,
    float r, float g, float b,
    int windowsX, int windowsZ_front, int windowsZ_side, int floors,
    const char* label
);