#include "input.h"
#include "globals.h"
#include "utils.h"
#include "camera.h"
#include "object.h"
#include "scene.h"
#include "picking.h"
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>

void applyGizmoDrag(int dx, int dy) {
    Object3D* o = findObject(g_selectedId);
    if (!o) return;
    Camera* cam = getActiveCamera();
    float dist = cam ? cam->camDist : 22.0f;
    float sensT = 0.007f * dist;
    float sensR = 0.35f;
    float sensS = 0.006f;
    switch (g_tmode) {
        case TM_TRANSLATE:
            switch (g_activeAxis) {
                case AXIS_X: o->pos[0] += dx * sensT; break;
                case AXIS_Y: o->pos[1] -= dy * sensT; break;
                case AXIS_Z: o->pos[2] += dx * sensT; break;
                default: break;
            }
            break;
        case TM_ROTATE:
            switch (g_activeAxis) {
                case AXIS_X: o->rot[0] += dy * sensR; break;
                case AXIS_Y: o->rot[1] += dx * sensR; break;
                case AXIS_Z: o->rot[2] += dx * sensR; break;
                default: break;
            }
            break;
        case TM_SCALE:
            switch (g_activeAxis) {
                case AXIS_X: o->scale[0] *= (1.0f + dx * sensS); if(o->scale[0]<0.05f) o->scale[0]=0.05f; break;
                case AXIS_Y: o->scale[1] *= (1.0f - dy * sensS); if(o->scale[1]<0.05f) o->scale[1]=0.05f; break;
                case AXIS_Z: o->scale[2] *= (1.0f + dx * sensS); if(o->scale[2]<0.05f) o->scale[2]=0.05f; break;
                default: break;
            }
            break;
    }
    glutPostRedisplay();
}

bool handleUIClick(int x, int y) {
    if (y < 32) return true;
    if (y > g_winH - 38) return true;

    if (g_showProperties && x >= g_winW - 264 && x <= g_winW && y >= 32 && y <= g_winH - 38)
        return true;

    if (x >= g_winW - 130 && y <= 160)
        return true;

    if (g_showOutliner && x >= 2 && x <= 222 && y >= 32 && y <= g_winH - 38) {
        int topY_gl = g_winH - 32;
        int gl_y = g_winH - y;
        for (int i = 0; i < (int)g_objects.size(); ++i) {
            int item_top_gl = topY_gl - 30 - 26 * i;
            int item_bot_gl = topY_gl - 52 - 26 * i;
            if (gl_y >= item_bot_gl && gl_y <= item_top_gl) {
                g_selectedId = g_objects[i].id;
                g_draggingGizmo = false;
                g_activeAxis = AXIS_NONE;
                printf("[Seleccionado] ID=%d\n", g_selectedId);
                glutPostRedisplay();
                return true;
            }
        }
        return true;
    }

    return false;
}

void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    Camera* cam = getActiveCamera();
    switch (key) {
        case '1': addObject(CUBO); break;
        case '2': addObject(ESFERA); break;
        case '3': addObject(TORO); break;
        case '4': addObject(TETERA); break;
        case 't': case 'T': g_tmode = TM_TRANSLATE; printf("[Modo] Traslacion\n"); break;
        case 'r': case 'R': g_tmode = TM_ROTATE;    printf("[Modo] Rotacion\n"); break;
        case 's': case 'S': g_tmode = TM_SCALE;     printf("[Modo] Escalado\n"); break;
        case 'w': case 'W': g_wireframe = !g_wireframe; break;
        case 'h': case 'H': g_showHelp = !g_showHelp; break;
        case 'd': case 'D': duplicateSelected(); break;
        case 127: case 8: deleteSelected(); break;
        case '+': case '=': if(cam) { cam->camDist -= 1.0f; if(cam->camDist<2.5f) cam->camDist=2.5f; } break;
        case '-': case '_': if(cam) { cam->camDist += 1.0f; } break;
        case '[': if(cam) { cam->fov -= 2.0f; if(cam->fov<10.0f) cam->fov=10.0f; printf("[FOV] %.1f\n", cam->fov); } break;
        case ']': if(cam) { cam->fov += 2.0f; if(cam->fov>120.0f) cam->fov=120.0f; printf("[FOV] %.1f\n", cam->fov); } break;
        case ',': if(cam) { cam->nearP *= 0.9f; if(cam->nearP<0.01f) cam->nearP=0.01f; printf("[Near] %.3f\n", cam->nearP); } break;
        case '.': if(cam) { cam->nearP *= 1.1f; if(cam->nearP>cam->farP*0.5f) cam->nearP=cam->farP*0.5f; printf("[Near] %.3f\n", cam->nearP); } break;
        case ';': if(cam) { cam->farP *= 0.9f; if(cam->farP<cam->nearP*2.0f) cam->farP=cam->nearP*2.0f; printf("[Far] %.1f\n", cam->farP); } break;
        case '\'': if(cam) { cam->farP *= 1.1f; printf("[Far] %.1f\n", cam->farP); } break;
        case 'c': case 'C':
            g_activeCam = (g_activeCam + 1) % g_cameras.size();
            printf("[Camara] %s\n", g_cameras[g_activeCam].name);
            break;
        case 27: exit(0); break;
    }
    glutPostRedisplay();
}

void special(int key, int x, int y) {
    (void)x; (void)y;
    Camera* cam = getActiveCamera();
    if (!cam) return;
    switch (key) {
        case GLUT_KEY_LEFT:  cam->camAY -= 4.0f; break;
        case GLUT_KEY_RIGHT: cam->camAY += 4.0f; break;
        case GLUT_KEY_UP:    cam->camAX += 4.0f; if(cam->camAX>89.0f) cam->camAX=89.0f; break;
        case GLUT_KEY_DOWN:  cam->camAX -= 4.0f; if(cam->camAX<-89.0f) cam->camAX=-89.0f; break;
        case GLUT_KEY_F5: saveScene("escena.lab7"); break;
        case GLUT_KEY_F6: loadScene("escena.lab7"); break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            g_leftDown = true;
            g_lastMX = x; g_lastMY = y;

            if (handleUIClick(x, y)) {
                g_dragStartedOnUI = true;
                glutPostRedisplay();
            } else {
                g_dragStartedOnUI = false;
                int picked = pickAt(x, y);
                if (picked >= 900) {
                    g_draggingGizmo = true;
                    if      (picked >= 921 && picked <= 923) g_activeAxis = (Axis)(picked - 920);
                    else if (picked >= 911 && picked <= 913) g_activeAxis = (Axis)(picked - 910);
                    else if (picked >= 901 && picked <= 903) g_activeAxis = (Axis)(picked - 900);
                    else g_activeAxis = AXIS_NONE;
                } else if (picked > 0) {
                    g_selectedId = picked;
                    g_draggingGizmo = false;
                    g_activeAxis = AXIS_NONE;
                    printf("[Seleccionado] ID=%d\n", g_selectedId);
                } else {
                    g_selectedId = -1;
                    g_draggingGizmo = false;
                    g_activeAxis = AXIS_NONE;
                }
            }
        } else if (state == GLUT_UP) {
            g_leftDown = false;
            g_draggingGizmo = false;
            g_activeAxis = AXIS_NONE;
            g_dragStartedOnUI = false;
        }
    }
    if (button == GLUT_MIDDLE_BUTTON) {
        if (state == GLUT_DOWN) g_middleDown = true;
        else if (state == GLUT_UP) g_middleDown = false;
    }
    if (state == GLUT_DOWN) {
        Camera* c = getActiveCamera();
        if (button == 3) {
            if (c) { c->camDist -= 1.5f; if(c->camDist<2.5f) c->camDist=2.5f; }
        } else if (button == 4) {
            if (c) { c->camDist += 1.5f; }
        }
    }
    glutPostRedisplay();
}

void motion(int x, int y) {
    int dx = x - g_lastMX;
    int dy = y - g_lastMY;
    Camera* cam = getActiveCamera();
    if (!cam) { g_lastMX = x; g_lastMY = y; return; }

    if (g_draggingGizmo && g_selectedId >= 0 && g_activeAxis != AXIS_NONE) {
        applyGizmoDrag(dx, dy);
    } else if ((g_leftDown || g_middleDown) && !g_dragStartedOnUI) {
        bool shift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) != 0;
        if (shift || g_middleDown) {
            float rx = cam->camAX * PI / 180.0f;
            float ry = cam->camAY * PI / 180.0f;
            float fwd[3] = {-std::cos(rx)*std::sin(ry), -std::sin(rx), -std::cos(rx)*std::cos(ry)};
            float up[3] = {0,1,0};
            float right[3]; cross(fwd, up, right); normalize(right);
            float up2[3]; cross(right, fwd, up2); normalize(up2);
            float panS = 0.012f * cam->camDist;
            cam->center[0] += right[0]*dx*panS + up2[0]*(-dy)*panS;
            cam->center[1] += right[1]*dx*panS + up2[1]*(-dy)*panS;
            cam->center[2] += right[2]*dx*panS + up2[2]*(-dy)*panS;
        } else {
            cam->camAY += dx * 0.45f;
            cam->camAX += dy * 0.45f;
            if (cam->camAX > 89.0f) cam->camAX = 89.0f;
            if (cam->camAX < -89.0f) cam->camAX = -89.0f;
        }
        glutPostRedisplay();
    }

    g_lastMX = x; g_lastMY = y;
}