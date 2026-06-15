#include "src/types.h"
#include "src/globals.h"
#include "src/camera.h"
#include "src/object.h"
#include "src/utils.h"
#include "src/display.h"
#include "src/input.h"
#include "src/menu.h"
#include <GL/freeglut.h>
#include <GL/glu.h>

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(g_winW, g_winH);
    glutCreateWindow("Editor Grafico 3D - Lab 7 (Blender Style)");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.09f, 0.09f, 0.11f, 1.0f);

    g_cameras.push_back(createDefaultCamera());
    g_cameras.push_back(createFrontCamera());
    g_cameras.push_back(createTopCamera());
    g_cameras.push_back(createSideCamera());

    addObject(CUBO);
    if (Object3D* o = findObject(g_selectedId)) { o->pos[0] = -3.0f; o->pos[1] = 0.8f; o->color[0]=0.9f; o->color[1]=0.35f; o->color[2]=0.35f; }
    addObject(ESFERA);
    if (Object3D* o = findObject(g_selectedId)) { o->pos[0] = 3.0f; o->pos[1] = 1.0f; o->color[0]=0.35f; o->color[1]=0.6f; o->color[2]=0.9f; }
    addObject(TORO);
    if (Object3D* o = findObject(g_selectedId)) { o->pos[2] = 3.0f; o->pos[1] = 0.8f; o->color[0]=0.4f; o->color[1]=0.9f; o->color[2]=0.4f; }
    addObject(TETERA);
    if (Object3D* o = findObject(g_selectedId)) { o->pos[2] = -3.0f; o->pos[1] = 0.9f; o->color[0]=0.9f; o->color[1]=0.8f; o->color[2]=0.2f; }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    createMenu();

    glutMainLoop();
    return 0;
}