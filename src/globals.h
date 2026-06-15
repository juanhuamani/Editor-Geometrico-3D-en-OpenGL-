#pragma once

#include "types.h"
#include <vector>

extern std::vector<Object3D> g_objects;
extern int g_nextId;
extern int g_selectedId;
extern TMode g_tmode;
extern Axis g_activeAxis;
extern bool g_draggingGizmo;
extern bool g_leftDown;
extern bool g_middleDown;
extern int g_lastMX;
extern int g_lastMY;
extern int g_winW;
extern int g_winH;

extern std::vector<Camera> g_cameras;
extern int g_activeCam;
extern int g_nextCamId;

extern bool g_wireframe;
extern bool g_showHelp;
extern bool g_showProperties;
extern bool g_showOutliner;

extern bool g_dragStartedOnUI;