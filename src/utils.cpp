#include "utils.h"
#include "globals.h"
#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>
#include <cstdio>

const char* typeToString(ObjType t) {
    switch (t) {
        case CUBO:   return "Cubo";
        case ESFERA: return "Esfera";
        case TORO:   return "Toro";
        case TETERA: return "Tetera";
    }
    return "?";
}

const char* modeToString(TMode m) {
    switch (m) {
        case TM_TRANSLATE: return "TRASLADAR";
        case TM_ROTATE:    return "ROTAR";
        case TM_SCALE:     return "ESCALAR";
    }
    return "";
}

const char* axisToString(Axis a) {
    switch (a) {
        case AXIS_X: return "X";
        case AXIS_Y: return "Y";
        case AXIS_Z: return "Z";
        default: return "";
    }
    return "";
}

void randomColor(float c[3]) {
    c[0] = (rand() % 180 + 75) / 255.0f;
    c[1] = (rand() % 180 + 75) / 255.0f;
    c[2] = (rand() % 180 + 75) / 255.0f;
}

void normalize(float v[3]) {
    float len = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (len > 0.0001f) { v[0]/=len; v[1]/=len; v[2]/=len; }
}

void cross(const float a[3], const float b[3], float out[3]) {
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

Object3D* findObject(int id) {
    for (auto& o : g_objects) if (o.id == id) return &o;
    return nullptr;
}

void setPickColor(int id) {
    unsigned char r = (id & 0xFF);
    unsigned char g = ((id >> 8) & 0xFF);
    unsigned char b = ((id >> 16) & 0xFF);
    glColor3ub(r, g, b);
}