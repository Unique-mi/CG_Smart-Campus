#include "AcademicBlock.h"
#include <GL/glut.h>
#include <string>

// Helper to draw a rectangular prism
static void drawRectPrism(float w, float h, float d) {
    glPushMatrix();
    glScalef(w, h, d);
    glutSolidCube(1.0);
    glPopMatrix();
}

// Helper to draw 3D text above the building
static void renderText3D(float x, float y, float z, void* font, const std::string& text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos3f(x, y, z);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

void drawAcademicBlock(
    float x, float y, float z,
    float w, float h, float d,
    float r, float g, float b,
    int windowsX, int windowsZ_front, int windowsZ_side, int floors,
    const char* label
) {
    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(x, y + h/2.0f, z);
    drawRectPrism(w, h, d);

    // Roof
    glColor3f(r * 0.6f, g * 0.6f, b * 0.6f);
    glPushMatrix();
    glTranslatef(0, h/2.0f + 0.15f, 0);
    drawRectPrism(w + 0.5f, 0.3f, d + 0.5f);
    glPopMatrix();

    // Windows & Doors
    float floorHeight = h / floors;
    float windowWidth = w / (windowsX + 1) * 0.6f;
    float windowHeight = floorHeight * 0.5f;
    float windowDepth = 0.2f;

    float doorWidth = windowWidth * 1.5f;
    float doorHeight = floorHeight * 0.8f;

    for (int f = 0; f < floors; ++f) {
        float currentFloorY = -h/2.0f + f * floorHeight + floorHeight * 0.2f;

        // Front/Back Windows (along X axis, on Z faces)
        if (windowsZ_front > 0) {
            float winSpacingZ_front = d / (windowsZ_front +1) ;
            for (int i = 0; i < windowsZ_front; ++i) {
                float winZ = -d/2.0f + (i+1)*winSpacingZ_front - winSpacingZ_front/2.0f;
                // Front face
                glPushMatrix();
                glTranslatef(w/2.0f + windowDepth/2.0f, currentFloorY + windowHeight/2.0f, winZ);
                glColor3f(0.5f, 0.7f, 0.8f);
                drawRectPrism(windowDepth, windowHeight, windowWidth*0.8f);
                glColor3f(r*0.5f, g*0.5f, b*0.5f);
                drawRectPrism(windowDepth*1.2f, windowHeight+0.2f, windowWidth*0.8f+0.2f);
                glPopMatrix();
                // Back face
                glPushMatrix();
                glTranslatef(-w/2.0f - windowDepth/2.0f, currentFloorY + windowHeight/2.0f, winZ);
                glColor3f(0.5f, 0.7f, 0.8f);
                drawRectPrism(windowDepth, windowHeight, windowWidth*0.8f);
                glColor3f(r*0.5f, g*0.5f, b*0.5f);
                drawRectPrism(windowDepth*1.2f, windowHeight+0.2f, windowWidth*0.8f+0.2f);
                glPopMatrix();
            }
        }

        // Side Windows (along Z axis, on X faces)
        if (windowsX > 0) {
            float winSpacingX = w / (windowsX +1);
            for (int i = 0; i < windowsX; ++i) {
                float winX = -w/2.0f + (i+1)*winSpacingX - winSpacingX/2.0f;
                // Left face
                glPushMatrix();
                glTranslatef(winX, currentFloorY + windowHeight/2.0f, d/2.0f + windowDepth/2.0f);
                glColor3f(0.5f, 0.7f, 0.8f);
                drawRectPrism(windowWidth, windowHeight, windowDepth);
                glColor3f(r*0.5f, g*0.5f, b*0.5f);
                drawRectPrism(windowWidth+0.2f, windowHeight+0.2f, windowDepth*1.2f);
                glPopMatrix();
                // Right face
                glPushMatrix();
                glTranslatef(winX, currentFloorY + windowHeight/2.0f, -d/2.0f - windowDepth/2.0f);
                glColor3f(0.5f, 0.7f, 0.8f);
                drawRectPrism(windowWidth, windowHeight, windowDepth);
                glColor3f(r*0.5f, g*0.5f, b*0.5f);
                drawRectPrism(windowWidth+0.2f, windowHeight+0.2f, windowDepth*1.2f);
                glPopMatrix();
            }
        }
        // Door (only on ground floor, front face, center)
        if (f == 0) {
            glColor3f(r * 0.4f, g * 0.4f, b * 0.35f);
            glPushMatrix();
            glTranslatef(w/2.0f + windowDepth/2.0f, -h/2.0f + doorHeight/2.0f, 0);
            drawRectPrism(windowDepth*1.5f, doorHeight, doorWidth);
            glPopMatrix();
        }
    }
    glPopMatrix();

    // Label above building
    renderText3D(x, y + h + 2, z, GLUT_BITMAP_HELVETICA_12, label, 0.1f, 0.1f, 0.1f);
}