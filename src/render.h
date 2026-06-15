#pragma once

struct Object3D;

void drawGrid();
void drawObject(const Object3D& o, bool highlight);
void drawShadows();
void drawHandles(const Object3D& o, bool picking);
void drawNavGizmo();