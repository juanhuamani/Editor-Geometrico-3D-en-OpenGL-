#include "camera.h"
#include "globals.h"
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <cstdio>
#include <cmath>

Camera* getActiveCamera() {
    if (g_activeCam >= 0 && g_activeCam < (int)g_cameras.size())
        return &g_cameras[g_activeCam];
    return nullptr;
}

Camera createDefaultCamera() {
    Camera c;
    c.id = g_nextCamId++;
    std::snprintf(c.name, sizeof(c.name), "Perspectiva %d", c.id);
    c.camDist = 22.0f; c.camAX = 35.0f; c.camAY = 50.0f;
    c.center[0] = 0; c.center[1] = 0; c.center[2] = 0;
    c.fov = 55.0f; c.nearP = 0.1f; c.farP = 250.0f;
    return c;
}

Camera createFrontCamera() {
    Camera c;
    c.id = g_nextCamId++;
    std::snprintf(c.name, sizeof(c.name), "Frontal %d", c.id);
    c.camDist = 22.0f; c.camAX = 0.0f; c.camAY = 0.0f;
    c.center[0] = 0; c.center[1] = 0; c.center[2] = 0;
    c.fov = 55.0f; c.nearP = 0.1f; c.farP = 250.0f;
    return c;
}

Camera createTopCamera() {
    Camera c;
    c.id = g_nextCamId++;
    std::snprintf(c.name, sizeof(c.name), "Superior %d", c.id);
    c.camDist = 22.0f; c.camAX = 89.0f; c.camAY = 0.0f;
    c.center[0] = 0; c.center[1] = 0; c.center[2] = 0;
    c.fov = 55.0f; c.nearP = 0.1f; c.farP = 250.0f;
    return c;
}

Camera createSideCamera() {
    Camera c;
    c.id = g_nextCamId++;
    std::snprintf(c.name, sizeof(c.name), "Lateral %d", c.id);
    c.camDist = 22.0f; c.camAX = 0.0f; c.camAY = 90.0f;
    c.center[0] = 0; c.center[1] = 0; c.center[2] = 0;
    c.fov = 55.0f; c.nearP = 0.1f; c.farP = 250.0f;
    return c;
}

void setCamera() {
    Camera* cam = getActiveCamera();
    if (!cam) return;
    float rx = cam->camAX * PI / 180.0f;
    float ry = cam->camAY * PI / 180.0f;
    float ex = cam->center[0] + cam->camDist * std::cos(rx) * std::sin(ry);
    float ey = cam->center[1] + cam->camDist * std::sin(rx);
    float ez = cam->center[2] + cam->camDist * std::cos(rx) * std::cos(ry);
    gluLookAt(ex, ey, ez, cam->center[0], cam->center[1], cam->center[2], 0, 1, 0);
}