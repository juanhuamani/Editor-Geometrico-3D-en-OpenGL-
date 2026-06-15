#include "menu.h"
#include "globals.h"
#include "object.h"
#include "scene.h"
#include "camera.h"
#include <GL/freeglut.h>
#include <cstdio>

static void menuCallback(int val) {
    switch (val) {
        case 1: addObject(CUBO); break;
        case 2: addObject(ESFERA); break;
        case 3: addObject(TORO); break;
        case 4: addObject(TETERA); break;
        case 5: g_tmode = TM_TRANSLATE; printf("[Modo] Traslacion\n"); break;
        case 6: g_tmode = TM_ROTATE;    printf("[Modo] Rotacion\n"); break;
        case 7: g_tmode = TM_SCALE;     printf("[Modo] Escalado\n"); break;
        case 8: duplicateSelected(); break;
        case 9: deleteSelected(); break;
        case 10: g_objects.clear(); g_selectedId = -1; printf("[Escena] Vaciada\n"); break;
        case 11: g_wireframe = !g_wireframe; break;
        case 12: saveScene("escena.lab7"); break;
        case 13: loadScene("escena.lab7"); break;
        case 14: g_activeCam = (g_activeCam + 1) % g_cameras.size(); printf("[Camara] %s\n", g_cameras[g_activeCam].name); break;
        case 20: g_cameras.push_back(createDefaultCamera()); g_activeCam = g_cameras.size()-1; printf("[Camara] %s\n", g_cameras.back().name); break;
        case 21: g_cameras.push_back(createFrontCamera()); g_activeCam = g_cameras.size()-1; printf("[Camara] %s\n", g_cameras.back().name); break;
        case 22: g_cameras.push_back(createTopCamera()); g_activeCam = g_cameras.size()-1; printf("[Camara] %s\n", g_cameras.back().name); break;
        case 23: g_cameras.push_back(createSideCamera()); g_activeCam = g_cameras.size()-1; printf("[Camara] %s\n", g_cameras.back().name); break;
        case 99: exit(0); break;
    }
    glutPostRedisplay();
}

void createMenu() {
    int mainMenu = glutCreateMenu(menuCallback);
    glutAddMenuEntry("Agregar Cubo", 1);
    glutAddMenuEntry("Agregar Esfera", 2);
    glutAddMenuEntry("Agregar Toro", 3);
    glutAddMenuEntry("Agregar Tetera", 4);
    glutAddMenuEntry("-------------------", 0);
    glutAddMenuEntry("Modo Trasladar (T)", 5);
    glutAddMenuEntry("Modo Rotar (R)", 6);
    glutAddMenuEntry("Modo Escalar (S)", 7);
    glutAddMenuEntry("-------------------", 0);
    glutAddMenuEntry("Duplicar (D)", 8);
    glutAddMenuEntry("Eliminar (SUPR)", 9);
    glutAddMenuEntry("Eliminar todos", 10);
    glutAddMenuEntry("Wireframe/Solido (W)", 11);
    glutAddMenuEntry("-------------------", 0);
    glutAddMenuEntry("Guardar (F5)", 12);
    glutAddMenuEntry("Cargar (F6)", 13);
    glutAddMenuEntry("Siguiente camara (C)", 14);
    glutAddMenuEntry("-------------------", 0);

    int camMenu = glutCreateMenu(menuCallback);
    glutAddMenuEntry("Perspectiva", 20);
    glutAddMenuEntry("Vista Frontal", 21);
    glutAddMenuEntry("Vista Superior", 22);
    glutAddMenuEntry("Vista Lateral", 23);

    glutSetMenu(mainMenu);
    glutAddSubMenu("Crear Camara...", camMenu);
    glutAddMenuEntry("-------------------", 0);
    glutAddMenuEntry("Salir (ESC)", 99);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}