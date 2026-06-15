#pragma once

const float PI = 3.14159265f;

enum ObjType  { CUBO, ESFERA, TORO, TETERA };
enum TMode    { TM_TRANSLATE, TM_ROTATE, TM_SCALE };
enum Axis     { AXIS_NONE, AXIS_X, AXIS_Y, AXIS_Z };

struct Object3D {
    int id;
    ObjType type;
    float pos[3];
    float rot[3];
    float scale[3];
    float color[3];
};

struct Camera {
    int id;
    char name[64];
    float camDist;
    float camAX;
    float camAY;
    float center[3];
    float fov;
    float nearP;
    float farP;
};