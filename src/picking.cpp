#include "picking.h"
#include "globals.h"
#include "camera.h"
#include "utils.h"
#include "render.h"
#include <GL/freeglut.h>
#include <GL/glu.h>

int pickAt(int x, int y) {
    glDisable(GL_DITHER);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    Camera* cam = getActiveCamera();
    if (cam) gluPerspective(cam->fov, (double)g_winW / (double)g_winH, cam->nearP, cam->farP);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    setCamera();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& o : g_objects) {
        setPickColor(o.id);
        glPushMatrix();
        glTranslatef(o.pos[0], o.pos[1], o.pos[2]);
        glRotatef(o.rot[0], 1, 0, 0);
        glRotatef(o.rot[1], 0, 1, 0);
        glRotatef(o.rot[2], 0, 0, 1);
        glScalef(o.scale[0], o.scale[1], o.scale[2]);
        switch (o.type) {
            case CUBO:   glutSolidCube(1.5f); break;
            case ESFERA: glutSolidSphere(1.0f, 24, 24); break;
            case TORO:   glutSolidTorus(0.3f, 0.8f, 24, 24); break;
            case TETERA: glutSolidTeapot(1.0f); break;
        }
        glPopMatrix();
    }

    if (g_selectedId >= 0) {
        if (Object3D* o = findObject(g_selectedId)) {
            drawHandles(*o, true);
        }
    }

    glFlush();

    GLubyte pixel[3];
    glReadPixels(x, g_winH - 1 - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    int pickedId = pixel[0] | (pixel[1] << 8) | (pixel[2] << 16);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DITHER);
    return pickedId;
}