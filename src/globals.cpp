#include "globals.h"

std::vector<Object3D> g_objects;
int g_nextId = 1;
int g_selectedId = -1;
TMode g_tmode = TM_TRANSLATE;
Axis g_activeAxis = AXIS_NONE;
bool g_draggingGizmo = false;
bool g_leftDown = false;
bool g_middleDown = false;
int g_lastMX = 0, g_lastMY = 0;
int g_winW = 1280, g_winH = 720;

std::vector<Camera> g_cameras;
int g_activeCam = 0;
int g_nextCamId = 1;

bool g_wireframe = false;
bool g_showHelp = true;
bool g_showProperties = true;
bool g_showOutliner = true;

bool g_dragStartedOnUI = false;