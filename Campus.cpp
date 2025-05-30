#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm> // For std::min/max

// --- Configuration & Global Variables ---
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// Define M_PI if not defined (for Windows MinGW)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Camera variables
float camAngleX = 20.0f; // Elevation angle
float camAngleY = -45.0f; // Azimuth angle
float camDistance = 150.0f; // Distance from origin
float camLookAtX = 0.0f, camLookAtY = 10.0f, camLookAtZ = 0.0f;
float camPosX, camPosY, camPosZ;

// Mouse interaction for camera
int lastMouseX, lastMouseY;
bool mouseLeftDown = false;
bool mouseRightDown = false;

// Animation
bool isNightMode = false;
float sunAngle = 0.0f; // For sun/moon movement
float cloudOffset = 0.0f;
const int NUM_CLOUDS = 10;
struct Cloud {
    float x, y, z;
    float scale;
    float speed;
};
std::vector<Cloud> clouds;

const int NUM_CARS = 5;
struct Car {
    float x, z;
    float angle; // For orientation along path
    float speed;
    float r, g, b;
    int pathPoint; // Current point in path
    bool movingForward; // Direction along path segment
};
std::vector<Car> cars;
std::vector<std::pair<float, float>> carPath = {
    {-80, 40}, {-30, 40}, {-30, -50}, {30, -50}, {30, 40}, {80, 40},
    {80, -50}, {-80, -50}, {-80,40} // Loop back
};


// --- Utility Functions ---
void updateCameraPosition() {
    float radX = camAngleX * M_PI / 180.0f;
    float radY = camAngleY * M_PI / 180.0f;

    camPosX = camLookAtX + camDistance * cos(radX) * sin(radY);
    camPosY = camLookAtY + camDistance * sin(radX);
    camPosZ = camLookAtZ + camDistance * cos(radX) * cos(radY);
}

void drawCube(float size) {
    glutSolidCube(size);
}

void drawRectPrism(float w, float h, float d) {
    glPushMatrix();
    glScalef(w, h, d);
    glutSolidCube(1.0); // Unit cube scaled
    glPopMatrix();
}

// Function to render bitmap text (simple version for 3D)
void renderText3D(float x, float y, float z, void* font, const std::string& text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos3f(x, y, z);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

// --- Initialization Functions ---
void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // Main sun/moon light
    glEnable(GL_COLOR_MATERIAL); // Use glColor for material properties
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE); // Keep normals unit length

    float globalAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f}; // Slightly brighter global ambient
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}

void initClouds() {
    clouds.clear();
    for (int i = 0; i < NUM_CLOUDS; ++i) {
        Cloud c;
        c.x = (rand() % 400) - 200.0f;
        c.y = 70.0f + (rand() % 30); // Higher clouds
        c.z = (rand() % 400) - 200.0f;
        c.scale = 6.0f + (rand() % 100) / 30.0f; // Larger clouds
        c.speed = 0.05f + (rand() % 100) / 2000.0f;
        clouds.push_back(c);
    }
}

/*
void initCars() {
    cars.clear();
    for (int i = 0; i < NUM_CARS; ++i) {
        Car car;
        car.pathPoint = rand() % (carPath.size() -1) ;
        car.x = carPath[car.pathPoint].first;
        car.z = carPath[car.pathPoint].second;
        car.angle = 0.0f; // Will be updated
        car.speed = 0.2f + (static_cast<float>(rand() % 100) / 150.0f); // 0.25 to ~0.9
        car.r = 0.2f + static_cast<float>(rand() % 80) / 100.0f; // More saturated colors
        car.g = 0.2f + static_cast<float>(rand() % 80) / 100.0f;
        car.b = 0.2f + static_cast<float>(rand() % 80) / 100.0f;
        car.movingForward = true;
        cars.push_back(car);
    }
}
*/

void init() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Initial sky blue
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    initLighting();
    updateCameraPosition();
    srand(static_cast<unsigned int>(time(nullptr)));
    initClouds();
    // initCars();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// --- Drawing Functions ---

void drawGroundPlane() {
    // --- Main grassy ground ---
    glColor3f(0.3f, 0.6f, 0.25f); // Green grass
    glPushMatrix();
    glTranslatef(0, -0.5f, 0);
    glScalef(250.0f, 1.0f, 250.0f);
    drawCube(1.0); // Main ground
    glPopMatrix();

    float halfSize = 125.0f;
    float fenceHeight = 4.0f;
    float fenceThickness = 0.2f;

    // --- Fence on all four sides ---
    glColor3f(0.5f, 0.5f, 0.5f);

    // Front
    glPushMatrix();
    glTranslatef(0, fenceHeight / 2, halfSize);
    drawRectPrism(250.0f, fenceHeight, fenceThickness);
    glPopMatrix();

    // Back
    glPushMatrix();
    glTranslatef(0, fenceHeight / 2, -halfSize);
    drawRectPrism(250.0f, fenceHeight, fenceThickness);
    glPopMatrix();

    // Left
    glPushMatrix();
    glTranslatef(-halfSize, fenceHeight / 2, 0);
    drawRectPrism(fenceThickness, fenceHeight, 250.0f);
    glPopMatrix();

    // Right
    glPushMatrix();
    glTranslatef(halfSize, fenceHeight / 2, 0);
    drawRectPrism(fenceThickness, fenceHeight, 250.0f);
    glPopMatrix();

    // --- Road patches near gates (gray) ---
    glColor3f(0.3f, 0.3f, 0.3f); // Asphalt road color
    float roadW = 10.0f;
    float roadL = 15.0f;
    float roadY = 0.01f;

    // NW gate road (top-left)
    glPushMatrix();
    glTranslatef(-halfSize + 5.0f, roadY, -halfSize + roadL / 2);
    drawRectPrism(roadW, 0.05f, roadL);
    glPopMatrix();

    // SE gate road (bottom-right)
    glPushMatrix();
    glTranslatef(halfSize - 5.0f, roadY, halfSize - roadL / 2);
    drawRectPrism(roadW, 0.05f, roadL);
    glPopMatrix();

    // --- Realistic steel blue gates ---
    float gateW = 4.0f, gateH = 2.5f, gateD = 0.2f, post = 0.4f;

    // Gate color
    glColor3f(0.3f, 0.4f, 0.5f); // Metal gray-blue

    // NW Gate
    float gateX_NW = -halfSize + 5.0f;
    float gateZ_NW = -halfSize + gateD / 2;

    glPushMatrix(); glTranslatef(gateX_NW - gateW / 2, gateH / 2, gateZ_NW); drawRectPrism(gateW, gateH, gateD); glPopMatrix();
    glPushMatrix(); glTranslatef(gateX_NW + gateW / 2, gateH / 2, gateZ_NW); drawRectPrism(gateW, gateH, gateD); glPopMatrix();

    glColor3f(0.4f, 0.4f, 0.4f); // Pillars
    glPushMatrix(); glTranslatef(gateX_NW - gateW - post / 2, gateH / 2, gateZ_NW); drawRectPrism(post, gateH, post); glPopMatrix();
    glPushMatrix(); glTranslatef(gateX_NW + gateW + post / 2, gateH / 2, gateZ_NW); drawRectPrism(post, gateH, post); glPopMatrix();

    // SE Gate
    float gateX_SE = halfSize - 5.0f;
    float gateZ_SE = halfSize - gateD / 2;

    glColor3f(0.3f, 0.4f, 0.5f);
    glPushMatrix(); glTranslatef(gateX_SE - gateW / 2, gateH / 2, gateZ_SE); drawRectPrism(gateW, gateH, gateD); glPopMatrix();
    glPushMatrix(); glTranslatef(gateX_SE + gateW / 2, gateH / 2, gateZ_SE); drawRectPrism(gateW, gateH, gateD); glPopMatrix();

    glColor3f(0.4f, 0.4f, 0.4f);
    glPushMatrix(); glTranslatef(gateX_SE - gateW - post / 2, gateH / 2, gateZ_SE); drawRectPrism(post, gateH, post); glPopMatrix();
    glPushMatrix(); glTranslatef(gateX_SE + gateW + post / 2, gateH / 2, gateZ_SE); drawRectPrism(post, gateH, post); glPopMatrix();

    glEnable(GL_LIGHTING);
}

void drawSkyAndSunMoon() {
    float skyR1, skyG1, skyB1, skyR2, skyG2, skyB2; // For gradient
    float sunR, sunG, sunB;
    float lightIntensity;

    if (isNightMode) {
        skyR1 = 0.02f; skyG1 = 0.02f; skyB1 = 0.1f; // Bottom night sky
        skyR2 = 0.1f; skyG2 = 0.1f; skyB2 = 0.3f;   // Top night sky
        sunR = 0.85f; sunG = 0.85f; sunB = 0.75f; // Moon color
        lightIntensity = 0.35f;
    } else {
        skyR1 = 0.6f; skyG1 = 0.8f; skyB1 = 1.0f;   // Bottom day sky (horizon)
        skyR2 = 0.3f; skyG2 = 0.6f; skyB2 = 0.9f;   // Top day sky (zenith)
        sunR = 1.0f; sunG = 0.85f; sunB = 0.2f; // Sun color
        lightIntensity = 1.0f;
    }
    // Set clear color to average sky color, actual gradient drawn with a large quad
    glClearColor((skyR1+skyR2)/2.0f, (skyG1+skyG2)/2.0f, (skyB1+skyB2)/2.0f, 1.0f);

    // Draw Sky Dome/Box (simple large quad for now)
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE); // Draw sky behind everything
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,1,0,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glBegin(GL_QUADS);
    glColor3f(skyR1, skyG1, skyB1); glVertex2f(0,0); glVertex2f(1,0);
    glColor3f(skyR2, skyG2, skyB2); glVertex2f(1,1); glVertex2f(0,1);
    glEnd();
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);


    // Sun/Moon position
    float sunX = 200.0f * cos(sunAngle * M_PI / 180.0f); // Further away
    float sunY = 200.0f * sin(sunAngle * M_PI / 180.0f);
    float sunZ = 0;

    // Update light0 position and properties
    GLfloat light_position[] = {sunX, sunY, sunZ, 1.0f};
    GLfloat light_diffuse[] = {lightIntensity * sunR, lightIntensity * sunG, lightIntensity * sunB, 1.0f};
    GLfloat light_ambient[] = {lightIntensity * 0.3f, lightIntensity * 0.3f, lightIntensity * 0.3f, 1.0f}; // Slightly more ambient from sun/moon

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

    // Draw the sun/moon object
    glDisable(GL_LIGHTING);
    glColor3f(sunR, sunG, sunB);
    glPushMatrix();
    // Place sun/moon relative to camera lookAt but very far, so it seems to be at infinity
    // This is a simplification. A true skybox or skydome would handle this better.
    glTranslatef(camLookAtX + sunX * 0.8f , camLookAtY + sunY * 0.8f, camLookAtZ + sunZ * 0.8f);
    glutSolidSphere(isNightMode ? 10.0 : 12.0, 20, 20); // Slightly larger
    glPopMatrix();
    glEnable(GL_LIGHTING);

    // Stars at night
    if (isNightMode) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 0.9f);
        glPointSize(1.5f + (rand()%10)/10.0f); // Twinkle effect by varying size slightly each frame
        glBegin(GL_POINTS);
        for (int i = 0; i < 150; ++i) { // More stars
            float r = 250.0f;
            float theta = (rand() % 360) * M_PI / 180.0f;
            float phi = (rand() % 180 - 90) * M_PI / 180.0f;
             if (sin(phi) > 0.05) {
                glVertex3f(camLookAtX + r * cos(phi) * cos(theta),
                           camLookAtY + r * sin(phi),
                           camLookAtZ + r * cos(phi) * sin(theta));
            }
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }
}

void drawSingleCloud(float x, float y, float z, float scale) {
    glColor4f(0.92f, 0.92f, 0.98f, 0.75f); // Slightly brighter, still semi-transparent
    glPushMatrix();
    glTranslatef(x, y, z);
    // Composite cloud from several spheres
    glutSolidSphere(1.0 * scale, 12, 12);
    glTranslatef(0.7f * scale, 0.15f * scale, 0.1f * scale);
    glutSolidSphere(0.85f * scale, 10, 10);
    glTranslatef(-1.5f * scale, -0.1f * scale, 0.25f * scale);
    glutSolidSphere(0.9f * scale, 10, 10);
    glTranslatef(0.5f * scale, -0.2f * scale, -0.3f * scale);
    glutSolidSphere(0.7f * scale, 8, 8);
    glPopMatrix();
}

void drawAnimatedClouds() {
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    for (const auto& cloud : clouds) {
        // Reduce the multiplier for slower movement
        drawSingleCloud(cloud.x + cloudOffset * cloud.speed * 2.0f, cloud.y, cloud.z, cloud.scale);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void drawRoads() {
    glColor3f(0.18f, 0.18f, 0.20f); // Darker asphalt color
    // Main horizontal road
    glPushMatrix();
    glTranslatef(0, 0.05f, 0); // Closer to ground
    drawRectPrism(180.0f, 0.1f, 12.0f); // Wider roads
    glPopMatrix();

    // Main vertical road
    glPushMatrix();
    glTranslatef(-30, 0.05f, 0);
    drawRectPrism(12.0f, 0.1f, 120.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(30, 0.05f, 0);
    drawRectPrism(12.0f, 0.1f, 120.0f);
    glPopMatrix();

    // Road lines (thinner, more off-white)
    glColor3f(0.85f, 0.85f, 0.8f);
    glDisable(GL_LIGHTING); // Make lines emissive-like
    for(int i = -80; i < 80; i+=12){ // Adjusted spacing
        glPushMatrix();
        glTranslatef(static_cast<float>(i), 0.1f, 2.5f); // Centered on a 2-lane road
        drawRectPrism(6.0f, 0.05f, 0.3f); // Thinner lines
        glTranslatef(0,0,-5.0f);
        drawRectPrism(6.0f, 0.05f, 0.3f);
        glPopMatrix();
    }
     for(int i = -50; i < 50; i+=12){ // Vertical road lines
        glPushMatrix();
        glTranslatef(-30 + 2.5f, 0.1f, static_cast<float>(i));
        glRotatef(90,0,1,0);
        drawRectPrism(6.0f, 0.05f, 0.3f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-30 - 2.5f, 0.1f, static_cast<float>(i));
        glRotatef(90,0,1,0);
        drawRectPrism(6.0f, 0.05f, 0.3f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(30 + 2.5f, 0.1f, static_cast<float>(i));
        glRotatef(90,0,1,0);
        drawRectPrism(6.0f, 0.05f, 0.3f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(30 - 2.5f, 0.1f, static_cast<float>(i));
        glRotatef(90,0,1,0);
        drawRectPrism(6.0f, 0.05f, 0.3f);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
}

void drawDetailedBuilding(float x, float y, float z, float w, float h, float d,
                         float r, float g, float b,
                         int windowsX, int windowsZ_front, int windowsZ_side, int floors) {
    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(x, y + h/2.0f, z);
    drawRectPrism(w, h, d); // Main structure

    // Roof
    glColor3f(r * 0.6f, g * 0.6f, b * 0.6f); // Darker roof
    glPushMatrix();
    glTranslatef(0, h/2.0f + 0.15f, 0); // Thinner roof lip
    drawRectPrism(w + 0.5f, 0.3f, d + 0.5f); // Slightly smaller overhang
    glPopMatrix();

    // Windows & Doors
    float floorHeight = h / floors;
    float windowWidth = w / (windowsX + 1) * 0.6f;
    float windowHeight = floorHeight * 0.5f;
    float windowDepth = 0.2f; // How much windows are inset/outset

    float doorWidth = windowWidth * 1.5f;
    float doorHeight = floorHeight * 0.8f;

    for (int f = 0; f < floors; ++f) {
        float currentFloorY = -h/2.0f + f * floorHeight + floorHeight * 0.2f; // Base Y for windows on this floor

        // Front/Back Windows (along X axis, on Z faces)
        if (windowsZ_front > 0) {
            float winSpacingZ_front = d / (windowsZ_front +1) ;
            for (int i = 0; i < windowsZ_front; ++i) {
                float winZ = -d/2.0f + (i+1)*winSpacingZ_front - winSpacingZ_front/2.0f;
                // Front face
                glPushMatrix();
                glTranslatef(w/2.0f + windowDepth/2.0f, currentFloorY + windowHeight/2.0f, winZ);
                if(isNightMode && (rand() % 3 == 0)) glColor3f(0.9f, 0.8f, 0.3f); // Lit window
                else glColor3f(0.5f, 0.7f, 0.8f); // Day window / Unlit
                drawRectPrism(windowDepth, windowHeight, windowWidth*0.8f); // Window pane
                glColor3f(r*0.5f, g*0.5f, b*0.5f); // Frame
                drawRectPrism(windowDepth*1.2f, windowHeight+0.2f, windowWidth*0.8f+0.2f); // Frame
                glPopMatrix();
                // Back face (optional, can skip if not visible)
                 glPushMatrix();
                glTranslatef(-w/2.0f - windowDepth/2.0f, currentFloorY + windowHeight/2.0f, winZ);
                if(isNightMode && (rand() % 3 == 0)) glColor3f(0.9f, 0.8f, 0.3f);
                else glColor3f(0.5f, 0.7f, 0.8f);
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
                 if(isNightMode && (rand() % 3 == 0)) glColor3f(0.9f, 0.8f, 0.3f);
                else glColor3f(0.5f, 0.7f, 0.8f);
                drawRectPrism(windowWidth, windowHeight, windowDepth);
                glColor3f(r*0.5f, g*0.5f, b*0.5f);
                drawRectPrism(windowWidth+0.2f, windowHeight+0.2f, windowDepth*1.2f);
                glPopMatrix();
                // Right face (optional)
                 glPushMatrix();
                glTranslatef(winX, currentFloorY + windowHeight/2.0f, -d/2.0f - windowDepth/2.0f);
                if(isNightMode && (rand() % 3 == 0)) glColor3f(0.9f, 0.8f, 0.3f);
                else glColor3f(0.5f, 0.7f, 0.8f);
                drawRectPrism(windowWidth, windowHeight, windowDepth);
                glColor3f(r*0.5f, g*0.5f, b*0.5f);
                drawRectPrism(windowWidth+0.2f, windowHeight+0.2f, windowDepth*1.2f);
                glPopMatrix();
            }
        }
        // Door (only on ground floor, front face, center)
        if (f == 0) {
            glColor3f(r * 0.4f, g * 0.4f, b * 0.35f); // Darker door color
            glPushMatrix();
            glTranslatef(w/2.0f + windowDepth/2.0f, -h/2.0f + doorHeight/2.0f, 0); // Centered on Z
            drawRectPrism(windowDepth*1.5f, doorHeight, doorWidth);
            glPopMatrix();
        }
    }
    glPopMatrix(); // End of building transformation
}

void drawTree(float x, float y, float z) {
    // Tree trunk
    glColor3f(0.4f, 0.26f, 0.13f); // Dark brown
    glPushMatrix();
    glTranslatef(x, y + 2.0f, z);
    glScalef(0.5f, 4.0f, 0.5f);
    drawCube(1.0);
    glPopMatrix();

    // Canopy layers (three overlapping green spheres for realism)
    glColor3f(0.0f, 0.5f, 0.0f); // Dark green
    glPushMatrix();
    glTranslatef(x, y + 6.0f, z);
    glutSolidSphere(2.0, 16, 16);
    glTranslatef(0.7f, 0.5f, 0.3f);
    glutSolidSphere(1.5, 16, 16);
    glTranslatef(-1.4f, 0.0f, -0.6f);
    glutSolidSphere(1.5, 16, 16);
    glPopMatrix();
}

void drawChair(float x, float y, float z) {
    glColor3f(0.6f, 0.4f, 0.2f); // Wooden color

    // Seat
    glPushMatrix();
    glTranslatef(x, y + 0.5f, z);
    glScalef(2.2f, 0.2f, 1.0f);
    drawCube(1.0);
    glPopMatrix();

    // Backrest
    glPushMatrix();
    glTranslatef(x, y + 1.0f, z - 0.45f);
    glScalef(2.2f, 1.0f, 0.2f);
    drawCube(1.0);
    glPopMatrix();

    // Armrests
    for (float dx = -0.55f; dx <= 0.55f; dx += 1.1f) {
        glPushMatrix();
        glTranslatef(x + dx, y + 0.75f, z);
        glScalef(0.1f, 0.1f, 1.0f);
        drawCube(1.0);
        glPopMatrix();
    }

    // Legs
    for (float dx = -0.55f; dx <= 0.55f; dx += 1.1f) {
        for (float dz = -0.45f; dz <= 0.45f; dz += 0.9f) {
            glPushMatrix();
            glTranslatef(x + dx, y, z + dz);
            glScalef(0.1f, 0.5f, 0.1f);
            drawCube(1.0);
            glPopMatrix();
        }
    }
}

void drawPathTile(float x, float y, float z) {
    glColor3f(0.5f, 0.5f, 0.5f); // Stone gray
    glPushMatrix();
    glTranslatef(x, y + 0.01f, z);
    glScalef(1.0f, 0.05f, 1.0f);
    drawCube(1.0);
    glPopMatrix();
}

void drawWalkingPath(float startX, float zCenter, int tileCount) {
    for (int i = 0; i < tileCount; ++i) {
        drawPathTile(startX + i * 1.1f, 0, zCenter);
    }
}

void drawGardenArea() {
    // Wider grass patch
    glColor3f(0.2f, 0.6f, 0.25f); // Grass green
    glPushMatrix();
    glTranslatef(-19, -0.5f, 85);
    glScalef(60.0f, 1.02f, 30.0f);
    drawCube(1.0);
    glPopMatrix();

    // Walking path
    drawWalkingPath(-48, 85, 46);

    // Trees around the area
    drawTree(-45, 0, 78);
    drawTree(-38, 0, 92);
    drawTree(-30, 0, 76);
    drawTree(-22, 0, 95);
    drawTree(-14, 0, 79);
    drawTree(-6, 0, 94);
    drawTree(2, 0, 76);
    drawTree(10, 0, 93);

    // === CHAIRS ===
    // 4 Chairs facing cafe (assuming cafe is in +Z direction)
    glPushMatrix(); glTranslatef(-43, 0, 82); drawChair(0, 0, 0); glPopMatrix();
    glPushMatrix(); glTranslatef(-30, 0, 82); drawChair(0, 0, 0); glPopMatrix();
    glPushMatrix(); glTranslatef(-17, 0, 82); drawChair(0, 0, 0); glPopMatrix();
    glPushMatrix(); glTranslatef(-6, 0, 82);  drawChair(0, 0, 0); glPopMatrix();

    // 4 Chairs facing away from cafe (rotated 180°)
    glPushMatrix(); glTranslatef(-38, 0, 89); glRotatef(180, 0, 1, 0); drawChair(0, 0, 0); glPopMatrix();
    glPushMatrix(); glTranslatef(-25, 0, 89); glRotatef(180, 0, 1, 0); drawChair(0, 0, 0); glPopMatrix();
    glPushMatrix(); glTranslatef(-12, 0, 89); glRotatef(180, 0, 1, 0); drawChair(0, 0, 0); glPopMatrix();
    glPushMatrix(); glTranslatef(-1, 0, 89);  glRotatef(180, 0, 1, 0); drawChair(0, 0, 0); glPopMatrix();
}


void drawCampusBuildings() {
    // Shared Academic Block parameters
    float abWidth = 35, abHeight = 30, abDepth = 18;
    float abR = 0.75f, abG = 0.65f, abB = 0.58f;
    int abXSeg = 3, abYSeg = 5, abZSeg = 2, abWindowDepth = 4;

    // Academic Blocks
    drawDetailedBuilding(-100, 0, -25, abWidth, abHeight, abDepth, abR, abG, abB, abXSeg, abYSeg, abZSeg, abWindowDepth);
    renderText3D(-100, abHeight + 2, -25, GLUT_BITMAP_HELVETICA_18, "Academic Block 3", 0.1f, 0.1f, 0.1f);

    drawDetailedBuilding(-60, 0, -25, abWidth, abHeight, abDepth, abR, abG, abB, abXSeg, abYSeg, abZSeg, abWindowDepth);
    renderText3D(-60, abHeight + 2, -25, GLUT_BITMAP_HELVETICA_18, "Academic Block 1", 0.1f, 0.1f, 0.1f);

    drawDetailedBuilding(-60, 0, 25, abWidth, abHeight, abDepth, abR, abG, abB, abXSeg, abYSeg, abZSeg, abWindowDepth);
    renderText3D(-60, abHeight + 2, 25, GLUT_BITMAP_HELVETICA_18, "Academic Block 2", 0.1f, 0.1f, 0.1f);

    drawDetailedBuilding(-100, 0, 25, abWidth, abHeight, abDepth, abR, abG, abB, abXSeg, abYSeg, abZSeg, abWindowDepth);
    renderText3D(-100, abHeight + 2, 25, GLUT_BITMAP_HELVETICA_18, "Academic Block 4", 0.1f, 0.1f, 0.1f);

    // Library
    drawDetailedBuilding(0, 0, -25, 35, 45, 28, 0.85f, 0.8f, 0.75f, 5, 4, 3, 5);
    renderText3D(0, 48, -25, GLUT_BITMAP_HELVETICA_18, "Central Library", 0.08f, 0.08f, 0.08f);

    // Hostels
    drawDetailedBuilding(70, 0, -35, 18, 24, 12, 0.72f, 0.72f, 0.65f, 2, 3, 2, 4);
    renderText3D(70, 26, -35, GLUT_BITMAP_HELVETICA_18, "Dormitory A", 0.1f, 0.1f, 0.1f);

    drawDetailedBuilding(70, 0, 0, 18, 24, 12, 0.7f, 0.7f, 0.62f, 2, 3, 2, 4);
    renderText3D(70, 26, 0, GLUT_BITMAP_HELVETICA_18, "Dormitory B", 0.1f, 0.1f, 0.1f);

    drawDetailedBuilding(70, 0, 35, 18, 24, 12, 0.75f, 0.75f, 0.68f, 2, 3, 2, 4);
    renderText3D(70, 26, 35, GLUT_BITMAP_HELVETICA_18, "Dormitory C", 0.1f, 0.1f, 0.1f);

    // Admin Block
    drawDetailedBuilding(0, 0, 25, 40, 50, 25, 0.6f, 0.65f, 0.7f, 4, 4, 3, 5);
    renderText3D(0, 53, 25, GLUT_BITMAP_HELVETICA_18, "Admin Block", 0.1f, 0.1f, 0.1f);

    // Cafe
    drawDetailedBuilding(-30, 0, 60, 22, 18, 15, 0.85f, 0.55f, 0.4f, 2, 2, 2, 2);
    renderText3D(-30, 20, 60, GLUT_BITMAP_HELVETICA_18, "Campus Cafe", 0.1f, 0.1f, 0.1f);

    // Garden behind Cafe
    drawGardenArea();
}

// Draws a single parking space with white marking
void drawParkingSpace(float x, float y, float z, float angle = 0.0f) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0, 1, 0);

    // Pavement for the space
    glColor3f(0.32f, 0.32f, 0.35f); // Dark gray
    glPushMatrix();
    glScalef(2.5f, 0.05f, 5.5f);
    drawCube(1.0);
    glPopMatrix();

    // White marking lines for the space
    glColor3f(1.0f, 1.0f, 1.0f);
    // Left line
    glPushMatrix();
    glTranslatef(-1.2f, 0.03f, 0);
    glScalef(0.08f, 0.02f, 5.4f);
    drawCube(1.0);
    glPopMatrix();
    // Right line
    glPushMatrix();
    glTranslatef(1.2f, 0.03f, 0);
    glScalef(0.08f, 0.02f, 5.4f);
    drawCube(1.0);
    glPopMatrix();
    // Back line
    glPushMatrix();
    glTranslatef(0, 0.03f, -2.7f);
    glScalef(2.5f, 0.02f, 0.07f);
    drawCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

// Draws the full parking lot for 20 cars, 2 rows of 10, facing each other
void drawParkingLot(float baseX, float baseY, float baseZ) {
    int carsPerRow = 17;
    float spaceWidth = 2.5f, spaceLength = 5.5f, gapBetweenRows = 2.0f;
    float lotWidth = carsPerRow * spaceWidth + (carsPerRow - 1) * 0.3f;

    // Draw ground lot area
    glColor3f(0.28f, 0.28f, 0.32f);
    glPushMatrix();
    glTranslatef(baseX, baseY - 0.03f, baseZ);
    glScalef(lotWidth, 0.07f, 2 * spaceLength + gapBetweenRows + 2.5f);
    drawCube(1.0);
    glPopMatrix();

    // Draw parking spaces: one row
    for (int i = 0; i < carsPerRow; ++i) {
        float x = baseX - lotWidth / 2 + spaceWidth / 2 + i * (spaceWidth + 0.3f);
        float z1 = baseZ - (gapBetweenRows + spaceLength) / 2;
        float z2 = baseZ + (gapBetweenRows + spaceLength) / 2;
        drawParkingSpace(x, baseY, z1);
        // Opposite row, rotated 180°
        drawParkingSpace(x, baseY, z2, 180.0f);
    }

    // Optional: Add some trees or light poles at the corners for realism
    drawTree(baseX - lotWidth / 2 - 2.5f, baseY, baseZ - spaceLength);
    drawTree(baseX + lotWidth / 2 + 2.5f, baseY, baseZ + spaceLength);

    // Optional: Label or sign
    glColor3f(0, 0, 0);
    glRasterPos3f(baseX, baseY + 0.2f, baseZ - spaceLength - 1.5f);
    const char* label = "PARKING";
    for (const char* c = label; *c; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawBasketballCourt(float x, float y, float z) {
    // --- Court base (Dark blue) ---
    glColor3f(0.0f, 0.0f, 0.5f);
    glPushMatrix();
    glTranslatef(x, y, z);
    drawRectPrism(28.0f, 0.05f, 15.0f); // Court size (X by Z)
    glPopMatrix();

    // --- Court boundary lines ---
    glDisable(GL_LIGHTING);
    glColor3f(1.2f, 1.2f, 1.2f); // White lines

    // Outer lines
    glPushMatrix(); glTranslatef(x + 14.0f, y + 0.06f, z); drawRectPrism(0.1f, 0.01f, 15.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(x - 14.0f, y + 0.06f, z); drawRectPrism(0.1f, 0.01f, 15.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(x, y + 0.06f, z + 7.5f); drawRectPrism(28.0f, 0.01f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef(x, y + 0.06f, z - 7.5f); drawRectPrism(28.0f, 0.01f, 0.1f); glPopMatrix();

    // Center line (Z axis)
    glPushMatrix(); glTranslatef(x, y + 0.06f, z); drawRectPrism(0.2f, 0.01f, 15.0f); glPopMatrix();

    // Paint areas on east and west
    for (float side = -1.0f; side <= 1.0f; side += 2.0f) {
        float laneX = x + side * (14.0f - 4.0f);
        glPushMatrix(); glTranslatef(laneX, y + 0.06f, z); drawRectPrism(6.0f, 0.01f, 4.0f); glPopMatrix();
    }

    // Free throw arcs (East/West)
    for (float dir = -1.0f; dir <= 1.0f; dir += 2.0f) {
        float arcX = x + dir * (14.0f - 4.0f);
        for (int i = 0; i <= 18; ++i) {
            float theta = M_PI * i / 18;
            float z1 = z + cos(theta) * 3.0f;
            float x1 = arcX + sin(theta) * 3.0f * dir;
            glPushMatrix(); glTranslatef(x1, y + 0.06f, z1); drawRectPrism(0.1f, 0.01f, 0.1f); glPopMatrix();
        }
    }

    // --- Hoops (east and west) ---
    for (float side = -1.0f; side <= 1.0f; side += 2.0f) {
        float hoopX = x + side * 13.8f;
        float poleX = x + side * 14.0f;
        float backboardX = x + side * 13.9f;
        float rimX = x + side * 13.75f;

        // Pole
        glColor3f(0.5f, 0.2f, 0.2f);
        glPushMatrix();
        glTranslatef(poleX, y + 1.0f, z);
        drawRectPrism(0.2f, 2.0f, 0.2f);
        glPopMatrix();

        // Backboard
        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
        glTranslatef(backboardX, y + 3.0f, z);
        drawRectPrism(0.05f, 1.0f, 1.8f);
        glPopMatrix();

        // Rim
        glColor3f(1.0f, 0.0f, 0.0f);
        glPushMatrix();
        glTranslatef(rimX, y + 2.6f, z);
        drawRectPrism(0.1f, 0.05f, 0.6f);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);

    // --- Fence ---
    glColor3f(0.5f, 0.0f, 0.0f);
    float fenceH = 2.5f;
    for (float fx = x - 14; fx <= x + 14; fx += 2.0f) {
        for (float fz = z - 7.5f; fz <= z + 7.5f; fz += 15.0f) {
            glPushMatrix();
            glTranslatef(fx, y + fenceH / 2.0f, fz);
            drawRectPrism(0.1f, fenceH, 0.1f);
            glPopMatrix();
        }
    }
    for (float fz = z - 7.5f + 2.0f; fz <= z + 7.5f - 2.0f; fz += 2.0f) {
        for (float fx = x - 14; fx <= x + 14; fx += 28.0f) {
            glPushMatrix();
            glTranslatef(fx, y + fenceH / 2.0f, fz);
            drawRectPrism(0.1f, fenceH, 0.1f);
            glPopMatrix();
        }
    }
}


void drawFootballCourt() {
    // Scaled-down dimensions
    float length = 60.0f; // Z direction
    float width = 30.0f;  // X direction

    // Field position
    float centerZ = 0.0f;     // Aligned with hostels (middle of Hostel A, B, C)
    float centerX = 105.0f;   // To the right of hostels (hostels are at x = 70)
    float fieldY = 0.1f;

    // Draw green field
    glColor3f(0.1f, 0.4f, 0.1f);
    glPushMatrix();
    glTranslatef(centerX, fieldY - 0.02f, centerZ);
    drawRectPrism(width, 0.05f, length);
    glPopMatrix();

    // Field markings
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    float halfL = length / 2.0f;
    float halfW = width / 2.0f;

    // Outer boundary
    glBegin(GL_LINE_LOOP);
    glVertex3f(centerX - halfW, fieldY, centerZ - halfL);
    glVertex3f(centerX + halfW, fieldY, centerZ - halfL);
    glVertex3f(centerX + halfW, fieldY, centerZ + halfL);
    glVertex3f(centerX - halfW, fieldY, centerZ + halfL);
    glEnd();

    // Center line
    glBegin(GL_LINES);
    glVertex3f(centerX - halfW, fieldY, centerZ);
    glVertex3f(centerX + halfW, fieldY, centerZ);
    glEnd();

    // Center circle
    float centerRadius = 6.0f;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 36; ++i) {
        float angle = 2.0f * M_PI * i / 36;
        glVertex3f(centerX + centerRadius * cos(angle), fieldY, centerZ + centerRadius * sin(angle));
    }
    glEnd();

    // Penalty areas
    float boxW = 18.0f, boxD = 9.0f;

    // Left penalty box
    glBegin(GL_LINE_LOOP);
    glVertex3f(centerX - boxW / 2, fieldY, centerZ - halfL);
    glVertex3f(centerX + boxW / 2, fieldY, centerZ - halfL);
    glVertex3f(centerX + boxW / 2, fieldY, centerZ - halfL + boxD);
    glVertex3f(centerX - boxW / 2, fieldY, centerZ - halfL + boxD);
    glEnd();

    // Right penalty box
    glBegin(GL_LINE_LOOP);
    glVertex3f(centerX - boxW / 2, fieldY, centerZ + halfL);
    glVertex3f(centerX + boxW / 2, fieldY, centerZ + halfL);
    glVertex3f(centerX + boxW / 2, fieldY, centerZ + halfL - boxD);
    glVertex3f(centerX - boxW / 2, fieldY, centerZ + halfL - boxD);
    glEnd();

    // Penalty spots
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex3f(centerX, fieldY, centerZ - halfL + 7.5f);
    glVertex3f(centerX, fieldY, centerZ + halfL - 7.5f);
    glEnd();

    // Arcs at penalty areas
    float arcRadius = 6.0f;
    glBegin(GL_LINE_STRIP);
    for (int i = -6; i <= 6; ++i) {
        float angle = M_PI * i / 18.0f;
        glVertex3f(centerX + arcRadius * sin(angle), fieldY, centerZ - halfL + 7.5f + arcRadius * cos(angle));
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = -6; i <= 6; ++i) {
        float angle = M_PI * i / 18.0f;
        glVertex3f(centerX + arcRadius * sin(angle), fieldY, centerZ + halfL - 7.5f - arcRadius * cos(angle));
    }
    glEnd();

    // Goals
    float goalW = 6.0f, postH = 2.0f;
    float postThickness = 0.1f;

    // Left goal
    glPushMatrix();
    glTranslatef(centerX - goalW / 2, fieldY, centerZ - halfL - 0.3f);
    drawRectPrism(postThickness, postH, postThickness);
    glTranslatef(goalW, 0, 0);
    drawRectPrism(postThickness, postH, postThickness);
    glTranslatef(-goalW / 2, postH, 0);
    drawRectPrism(goalW, postThickness, postThickness);
    glPopMatrix();

    // Right goal
    glPushMatrix();
    glTranslatef(centerX - goalW / 2, fieldY, centerZ + halfL + 0.3f);
    drawRectPrism(postThickness, postH, postThickness);
    glTranslatef(goalW, 0, 0);
    drawRectPrism(postThickness, postH, postThickness);
    glTranslatef(-goalW / 2, postH, 0);
    drawRectPrism(goalW, postThickness, postThickness);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}


void drawSingleCar(const Car& car) {
    glColor3f(car.r, car.g, car.b);
    glPushMatrix();
    glTranslatef(car.x, 0.6f, car.z); // Base height for wheels on ground
    glRotatef(car.angle, 0, 1, 0); // Orient car

    // Main Body
    glPushMatrix();
    glTranslatef(0, 0.7f, 0); // Body center y
    drawRectPrism(4.5f, 1.4f, 2.2f);
    glPopMatrix();

    // Cabin
    glColor3f(car.r * 0.8f, car.g * 0.8f, car.b * 0.8f);
    glPushMatrix();
    glTranslatef(0.3f, 1.4f + 0.2f, 0); // Cabin slightly back and on top
    drawRectPrism(2.5f, 1.0f, 2.0f);
    glPopMatrix();

    // Wheels (Cylinders)
    glColor3f(0.1f, 0.1f, 0.1f); // Black wheels
    GLUquadric* q = gluNewQuadric();
    // Front-left
    glPushMatrix();
    glTranslatef(1.5f, 0.0f, 1.1f); glRotatef(90, 0,1,0); gluCylinder(q, 0.5, 0.5, 0.3, 10, 1);
    glPopMatrix();
    // Front-right
    glPushMatrix();
    glTranslatef(1.5f, 0.0f, -1.1f -0.3f);  glRotatef(90, 0,1,0); gluCylinder(q, 0.5, 0.5, 0.3, 10, 1);
    glPopMatrix();
    // Rear-left
    glPushMatrix();
    glTranslatef(-1.5f, 0.0f, 1.1f);  glRotatef(90, 0,1,0); gluCylinder(q, 0.5, 0.5, 0.3, 10, 1);
    glPopMatrix();
    // Rear-right
    glPushMatrix();
    glTranslatef(-1.5f, 0.0f, -1.1f -0.3f);  glRotatef(90, 0,1,0); gluCylinder(q, 0.5, 0.5, 0.3, 10, 1);
    glPopMatrix();
    gluDeleteQuadric(q);

    // Headlights & Taillights
    glDisable(GL_LIGHTING); // Make lights emissive
    if (isNightMode) {
        glColor3f(1.0f, 1.0f, 0.7f); // Bright yellow headlights
    } else {
        glColor3f(0.8f, 0.8f, 0.7f); // Dimmer day headlights
    }
    glPushMatrix(); glTranslatef(2.25f, 0.8f, 0.7f); drawRectPrism(0.2f, 0.3f, 0.3f); glPopMatrix(); // Left headlight
    glPushMatrix(); glTranslatef(2.25f, 0.8f, -0.7f); drawRectPrism(0.2f, 0.3f, 0.3f); glPopMatrix(); // Right headlight

    if (isNightMode) {
        glColor3f(1.0f, 0.2f, 0.2f); // Bright red taillights
    } else {
        glColor3f(0.7f, 0.1f, 0.1f); // Dimmer day taillights
    }
    glPushMatrix(); glTranslatef(-2.25f, 0.8f, 0.6f); drawRectPrism(0.2f, 0.3f, 0.25f); glPopMatrix(); // Left taillight
    glPushMatrix(); glTranslatef(-2.25f, 0.8f, -0.6f); drawRectPrism(0.2f, 0.3f, 0.25f); glPopMatrix(); // Right taillight
    glEnable(GL_LIGHTING);

    glPopMatrix(); // End car transformation
}

void drawCars() {
    for(const auto& car : cars) {
        drawSingleCar(car);
    }
}

void drawSimplifiedBirds() {
    // Example: a few "V" shaped birds, animated slightly
    if(!isNightMode) {
        glColor3f(0.15f, 0.15f, 0.15f);
        glDisable(GL_LIGHTING);
        glLineWidth(2.5f);
        for(int i=0; i < 3; ++i) { // 3 birds
            float birdX = 20.0f + i * 15 + sin(cloudOffset * 0.1f + i) * 5; // Move side to side
            float birdY = 60.0f + sin(cloudOffset * 0.05f + i*0.5f) * 3; // Move up and down
            float birdZ = 20.0f + i * 10;
            float wingAngle = sin(cloudOffset * 0.2f + i) * 15.0f; // Flapping motion

            glPushMatrix();
            glTranslatef(birdX, birdY, birdZ);
            glBegin(GL_LINES);
            glVertex3f(0, 0, 0); glVertex3f(2 * cos(wingAngle * M_PI/180.0f), 2 * sin(wingAngle * M_PI/180.0f), 0);
            glVertex3f(0, 0, 0); glVertex3f(-2 * cos(wingAngle * M_PI/180.0f), 2 * sin(wingAngle * M_PI/180.0f), 0);
            glEnd();
            glPopMatrix();
        }
        glEnable(GL_LIGHTING);
    }
}

// --- GLUT Callbacks ---
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color set by drawSkyAndSunMoon

    drawSkyAndSunMoon(); // Call this first to set sky color and light

    glLoadIdentity();
    gluLookAt(camPosX, camPosY, camPosZ,   // Camera position
              camLookAtX, camLookAtY, camLookAtZ,    // Look at point
              0.0f, 1.0f, 0.0f); // Up vector

    drawGroundPlane();
    drawRoads();
    drawCampusBuildings();
    drawParkingLot(0, 0, -67);
    drawBasketballCourt(102, 5.0f, -80.0f);

    drawFootballCourt();
    //drawCars();
    drawSimplifiedBirds();
    drawAnimatedClouds();

    // Draw some text UI for mode
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    renderText3D(10, WINDOW_HEIGHT - 25, 0, GLUT_BITMAP_HELVETICA_18, isNightMode ? "Night Mode" : "Day Mode", 1,1,1);
    renderText3D(10, WINDOW_HEIGHT - 45, 0, GLUT_BITMAP_HELVETICA_12, "N:Toggle Day/Night | Mouse:Orbit/Zoom | Arrows/RMB:Pan", 1,1,1);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);


    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = 1.0f * w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, ratio, 1.0f, 1000.0f); // Slightly wider FOV
    glMatrixMode(GL_MODELVIEW);
}

void update(int value) {
    // Day/Night cycle
    if (!isNightMode) {
        sunAngle += 0.08f; // Slower sun movement for longer day
        if (sunAngle > 180.0f) sunAngle = 0.0f;
    } else {
        sunAngle = 225.0f;
    }

    // Cloud animation (individual speeds are now handled in drawAnimatedClouds based on cloud.speed)
    cloudOffset += 0.1f;
    if (cloudOffset > 800.0f) cloudOffset = -800.0f; // Wider loop for clouds

    // Car animation
    for(auto& car : cars) {
        int currentTargetIdx = (car.pathPoint + 1) % carPath.size();
        float targetX = carPath[currentTargetIdx].first;
        float targetZ = carPath[currentTargetIdx].second;

        float dirX = targetX - car.x;
        float dirZ = targetZ - car.z;
        float dist = sqrt(dirX*dirX + dirZ*dirZ);

        // Update car angle for orientation
        car.angle = atan2(dirX, dirZ) * 180.0f / M_PI;


        if (dist < car.speed * 1.8f) { // Increased threshold for smoother turning
            car.pathPoint = currentTargetIdx;
            car.x = targetX;
            car.z = targetZ;
        } else {
            car.x += (dirX / dist) * car.speed;
            car.z += (dirZ / dist) * car.speed;
        }
    }


    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'n':
        case 'N':
            isNightMode = !isNightMode;
            if (!isNightMode) sunAngle = 0;
            break;
        case 27: // ESC key
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    float panSpeed = 2.5f; // Slightly faster pan
    float radY = camAngleY * M_PI / 180.0f;
    float viewDirX_ortho = cos(radY); // Orthogonal to view Z for left/right pan
    float viewDirZ_ortho = sin(radY);

    float forwardDirX = sin(radY); // Aligned with view Z for forward/back pan
    float forwardDirZ = cos(radY);


    switch (key) {
        case GLUT_KEY_UP: // Pan "forward" (move lookAt opposite to view direction)
            camLookAtX -= forwardDirX * panSpeed;
            camLookAtZ -= forwardDirZ * panSpeed;
            break;
        case GLUT_KEY_DOWN: // Pan "backward"
            camLookAtX += forwardDirX * panSpeed;
            camLookAtZ += forwardDirZ * panSpeed;
            break;
        case GLUT_KEY_LEFT: // Pan "left"
            camLookAtX -= viewDirX_ortho * panSpeed;
            camLookAtZ += viewDirZ_ortho * panSpeed;
            break;
        case GLUT_KEY_RIGHT: // Pan "right"
            camLookAtX += viewDirX_ortho * panSpeed;
            camLookAtZ -= viewDirZ_ortho * panSpeed;
            break;
    }
    updateCameraPosition();
    glutPostRedisplay();
}


void mouseButton(int button, int state, int x, int y) {
    lastMouseX = x;
    lastMouseY = y;
    if (button == GLUT_LEFT_BUTTON) {
        mouseLeftDown = (state == GLUT_DOWN);
    } else if (button == GLUT_RIGHT_BUTTON) {
        mouseRightDown = (state == GLUT_DOWN);
    } else if (button == 3) {
        camDistance -= 4.0f; // Finer zoom
        if (camDistance < 5.0f) camDistance = 5.0f; // Min zoom closer
        updateCameraPosition();
        glutPostRedisplay();
    } else if (button == 4) {
        camDistance += 4.0f;
        if (camDistance > 600.0f) camDistance = 600.0f;
        updateCameraPosition();
        glutPostRedisplay();
    }
}

void mouseMove(int x, int y) {
    float dx = x - lastMouseX;
    float dy = y - lastMouseY;

    if (mouseLeftDown) { // Orbit
        camAngleY += dx * 0.2f; // Slower orbit
        camAngleX += dy * 0.2f;
        camAngleX = std::max(1.0f, std::min(89.0f, camAngleX));
    }

    if (mouseRightDown) { // Pan (improved screen-aligned like)
        float panFactor = 0.05f * (camDistance / 150.0f); // Scale pan speed with distance

        float radCamY_deg = camAngleY * M_PI / 180.0f;
        // Screen X-axis in world (camera's right vector)
        float screenRightX = cos(radCamY_deg);
        float screenRightZ = -sin(radCamY_deg);

        camLookAtX -= screenRightX * dx * panFactor;
        camLookAtZ -= screenRightZ * dx * panFactor;

        // Screen Y-axis in world (camera's up vector - simplified)
        // For a more accurate screen-Y pan, you'd need the full camera's up vector.
        // This approximation moves along global Y, which is often good enough for this style.
        camLookAtY += dy * panFactor;
    }

    lastMouseX = x;
    lastMouseY = y;
    updateCameraPosition();
    glutPostRedisplay();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("3D Smart Campus Simulation - Enhanced Realism");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);
    glutTimerFunc(0, update, 0);

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



