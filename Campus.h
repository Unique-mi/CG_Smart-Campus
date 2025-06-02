#pragma once

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

// Initialization and GLUT callback wrappers
void campusInit();
void campusDisplay();
void campusReshape(int w, int h);
void campusUpdate(int value);
void campusKeyboard(unsigned char key, int x, int y);
void campusSpecialKeys(int key, int x, int y);
void campusMouseButton(int button, int state, int x, int y);
void campusMouseMove(int x, int y);