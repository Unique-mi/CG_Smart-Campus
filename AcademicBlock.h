#pragma once

// Draws an academic block at the given position with the given parameters
void drawAcademicBlock(
    float x, float y, float z,
    float width, float height, float depth,
    float r, float g, float b,
    int windowsX, int windowsZ_front, int windowsZ_side, int floors,
    const char* label
);