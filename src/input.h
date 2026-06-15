#pragma once

void applyGizmoDrag(int dx, int dy);
bool handleUIClick(int x, int y);
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);