#pragma once

#include "types.h"

const char* typeToString(ObjType t);
const char* modeToString(TMode m);
const char* axisToString(Axis a);
void randomColor(float c[3]);
void normalize(float v[3]);
void cross(const float a[3], const float b[3], float out[3]);
Object3D* findObject(int id);
void setPickColor(int id);