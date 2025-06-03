#include <GL/glut.h>
#include <iostream>
#include "Campus.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("3D Smart Campus Simulation - Enhanced Realism");

    campusInit();

    glutDisplayFunc(campusDisplay);
    glutReshapeFunc(campusReshape);
    glutKeyboardFunc(campusKeyboard);
    glutSpecialFunc(campusSpecialKeys);
    glutMouseFunc(campusMouseButton);
    glutMotionFunc(campusMouseMove);
    glutTimerFunc(0, campusUpdate, 0);

    std::cout << "Interactive 3D Smart Campus - Enhanced Realism" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  N: Toggle Day/Night Mode" << std::endl;
    std::cout << "  Mouse Left Drag: Orbit Camera" << std::endl;
    std::cout << "  Mouse Right Drag: Pan Camera" << std::endl;
    std::cout << "  Mouse Wheel: Zoom Camera" << std::endl;
    std::cout << "  Arrow Keys: Pan Camera" << std::endl;
    std::cout << "  ESC: Exit" << std::endl;

    glutMainLoop();
    return 0;
}