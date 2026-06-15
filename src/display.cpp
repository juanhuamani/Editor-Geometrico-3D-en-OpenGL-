#include "display.h"
#include "globals.h"
#include "camera.h"
#include "utils.h"
#include "render.h"
#include "hud.h"
#include <GL/freeglut.h>
#include <GL/glu.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Camera* cam = getActiveCamera();
    if (cam) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(cam->fov, (double)g_winW / (double)g_winH, cam->nearP, cam->farP);
        glMatrixMode(GL_MODELVIEW);
    }
    glLoadIdentity();

    setCamera();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = {8.0f, 16.0f, 6.0f, 1.0f};
    GLfloat lightAmb[] = {0.22f, 0.22f, 0.25f, 1.0f};
    GLfloat lightDif[] = {0.88f, 0.85f, 0.80f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    glDisable(GL_LIGHTING);

    drawGrid();
    drawShadows();

    for (const auto& o : g_objects) {
        drawObject(o, o.id == g_selectedId);
    }

    glDisable(GL_LIGHTING);
    if (g_selectedId >= 0) {
        if (Object3D* o = findObject(g_selectedId)) {
            drawHandles(*o, false);
        }
    }

    drawNavGizmo();
    drawHUD();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    g_winW = w; g_winH = h;
    glViewport(0, 0, w, h);
    Camera* cam = getActiveCamera();
    if (cam) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(cam->fov, (double)w / (double)h, cam->nearP, cam->farP);
        glMatrixMode(GL_MODELVIEW);
    }
}