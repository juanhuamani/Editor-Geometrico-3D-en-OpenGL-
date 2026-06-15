#include "render.h"
#include "globals.h"
#include "utils.h"
#include "camera.h"
#include <GL/freeglut.h>
#include <GL/glu.h>

void drawGrid() {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glColor3f(0.11f, 0.11f, 0.13f);
    glBegin(GL_QUADS);
        glVertex3f(-30, 0, -30); glVertex3f(30, 0, -30);
        glVertex3f(30, 0, 30); glVertex3f(-30, 0, 30);
    glEnd();
    glDisable(GL_POLYGON_OFFSET_FILL);

    glColor3f(0.18f, 0.18f, 0.22f);
    glBegin(GL_LINES);
    for (int i = -20; i <= 20; ++i) {
        if (i % 5 != 0) {
            glVertex3f((float)i, 0.01f, -20.0f); glVertex3f((float)i, 0.01f, 20.0f);
            glVertex3f(-20.0f, 0.01f, (float)i); glVertex3f(20.0f, 0.01f, (float)i);
        }
    }
    glEnd();

    glColor3f(0.24f, 0.24f, 0.30f);
    glLineWidth(1.2f);
    glBegin(GL_LINES);
    for (int i = -20; i <= 20; i += 5) {
        glVertex3f((float)i, 0.02f, -20.0f); glVertex3f((float)i, 0.02f, 20.0f);
        glVertex3f(-20.0f, 0.02f, (float)i); glVertex3f(20.0f, 0.02f, (float)i);
    }
    glEnd();
    glLineWidth(1.0f);

    glLineWidth(2.8f);
    glBegin(GL_LINES);
        glColor3f(0.80f, 0.15f, 0.15f); glVertex3f(0,0.03f,0); glVertex3f(8,0.03f,0);
        glColor3f(0.15f, 0.80f, 0.15f); glVertex3f(0,0.03f,0); glVertex3f(0,8,0);
        glColor3f(0.15f, 0.35f, 0.90f); glVertex3f(0,0.03f,0); glVertex3f(0,0.03f,8);
    glEnd();

    glColor3f(0.80f, 0.15f, 0.15f);
    glRasterPos3f(8.5f, 0.12f, 0); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'X');
    glColor3f(0.15f, 0.80f, 0.15f);
    glRasterPos3f(0, 8.7f, 0); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Y');
    glColor3f(0.15f, 0.35f, 0.90f);
    glRasterPos3f(0, 0.12f, 8.5f); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Z');
    glLineWidth(1.0f);
}

void drawObject(const Object3D& o, bool highlight) {
    glPushMatrix();
    glTranslatef(o.pos[0], o.pos[1], o.pos[2]);
    glRotatef(o.rot[0], 1, 0, 0);
    glRotatef(o.rot[1], 0, 1, 0);
    glRotatef(o.rot[2], 0, 0, 1);
    glScalef(o.scale[0], o.scale[1], o.scale[2]);

    if (highlight) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.85f, 0.0f);
        glLineWidth(2.5f);
        glPushMatrix();
        glScalef(1.06f, 1.06f, 1.06f);
        switch (o.type) {
            case CUBO:   glutWireCube(1.5f); break;
            case ESFERA: glutWireSphere(1.0f, 24, 24); break;
            case TORO:   glutWireTorus(0.3f, 0.8f, 24, 24); break;
            case TETERA: glutWireTeapot(1.02f); break;
        }
        glPopMatrix();
        glLineWidth(1.0f);
    }

    if (g_wireframe) {
        glDisable(GL_LIGHTING);
        glColor3f(o.color[0], o.color[1], o.color[2]);
        switch (o.type) {
            case CUBO:   glutWireCube(1.5f); break;
            case ESFERA: glutWireSphere(1.0f, 24, 24); break;
            case TORO:   glutWireTorus(0.3f, 0.8f, 24, 24); break;
            case TETERA: glutWireTeapot(1.0f); break;
        }
    } else {
        glEnable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
        GLfloat matAmb[]  = { o.color[0]*0.4f, o.color[1]*0.4f, o.color[2]*0.4f, 1.0f };
        GLfloat matDiff[] = { o.color[0], o.color[1], o.color[2], 1.0f };
        GLfloat matSpec[] = { 0.4f, 0.4f, 0.4f, 1.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);
        switch (o.type) {
            case CUBO:   glutSolidCube(1.5f); break;
            case ESFERA: glutSolidSphere(1.0f, 32, 32); break;
            case TORO:   glutSolidTorus(0.3f, 0.8f, 32, 32); break;
            case TETERA: glutSolidTeapot(1.0f); break;
        }
        glEnable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);
    }
    glPopMatrix();
}

void drawShadows() {
    if (g_objects.empty()) return;
    float lx = 8.0f, ly = 16.0f, lz = 6.0f;
    GLfloat sm[16] = { ly,0,0,0, -lx,0,-lz,-1, 0,0,ly,0, 0,0,0,ly };

    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-2.0f, -2.0f);
    glDisable(GL_LIGHTING);
    glColor4f(0.0f, 0.0f, 0.0f, 0.28f);
    glMultMatrixf(sm);

    for (const auto& o : g_objects) {
        glPushMatrix();
        glTranslatef(o.pos[0], o.pos[1], o.pos[2]);
        glRotatef(o.rot[0], 1, 0, 0);
        glRotatef(o.rot[1], 0, 1, 0);
        glRotatef(o.rot[2], 0, 0, 1);
        glScalef(o.scale[0], o.scale[1], o.scale[2]);
        switch (o.type) {
            case CUBO:   glutSolidCube(1.5f); break;
            case ESFERA: glutSolidSphere(1.0f, 16, 16); break;
            case TORO:   glutSolidTorus(0.3f, 0.8f, 16, 16); break;
            case TETERA: glutSolidTeapot(1.0f); break;
        }
        glPopMatrix();
    }

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_BLEND);
    glPopMatrix();
}

void drawTranslateHandle(const Object3D& o, Axis axis, int pickId, bool picking) {
    float len = 2.5f;
    if (picking) {
        setPickColor(pickId);
    } else {
        bool active = (g_activeAxis == axis && g_draggingGizmo);
        if (active) glColor3f(1.0f, 1.0f, 0.2f);
        else {
            switch (axis) {
                case AXIS_X: glColor3f(1.0f, 0.15f, 0.15f); break;
                case AXIS_Y: glColor3f(0.15f, 1.0f, 0.15f); break;
                case AXIS_Z: glColor3f(0.15f, 0.4f, 1.0f); break;
                default: break;
            }
        }
    }
    glPushMatrix();
    glTranslatef(o.pos[0], o.pos[1], o.pos[2]);
    glLineWidth(2.8f);
    glBegin(GL_LINES);
    switch (axis) {
        case AXIS_X: glVertex3f(0,0,0); glVertex3f(len,0,0); break;
        case AXIS_Y: glVertex3f(0,0,0); glVertex3f(0,len,0); break;
        case AXIS_Z: glVertex3f(0,0,0); glVertex3f(0,0,len); break;
        default: break;
    }
    glEnd();
    glLineWidth(1.0f);
    switch (axis) {
        case AXIS_X: glTranslatef(len, 0, 0); glRotatef(90, 0, 1, 0); break;
        case AXIS_Y: glTranslatef(0, len, 0); glRotatef(-90, 1, 0, 0); break;
        case AXIS_Z: glTranslatef(0, 0, len); break;
        default: break;
    }
    glutSolidCone(0.14f, 0.45f, 12, 12);
    glPopMatrix();
}

void drawRotateHandle(const Object3D& o, Axis axis, int pickId, bool picking) {
    float rad = 2.2f;
    if (picking) {
        setPickColor(pickId);
    } else {
        bool active = (g_activeAxis == axis && g_draggingGizmo);
        if (active) glColor3f(1.0f, 1.0f, 0.2f);
        else {
            switch (axis) {
                case AXIS_X: glColor3f(1.0f, 0.25f, 0.25f); break;
                case AXIS_Y: glColor3f(0.25f, 1.0f, 0.25f); break;
                case AXIS_Z: glColor3f(0.25f, 0.5f, 1.0f); break;
                default: break;
            }
        }
    }
    glPushMatrix();
    glTranslatef(o.pos[0], o.pos[1], o.pos[2]);
    switch (axis) {
        case AXIS_X: glRotatef(90, 0, 1, 0); break;
        case AXIS_Y: glRotatef(90, 1, 0, 0); break;
        case AXIS_Z: break;
        default: break;
    }
    glutSolidTorus(0.06f, rad, 20, 40);
    glPopMatrix();
}

void drawScaleHandle(const Object3D& o, Axis axis, int pickId, bool picking) {
    float len = 2.2f;
    if (picking) {
        setPickColor(pickId);
    } else {
        bool active = (g_activeAxis == axis && g_draggingGizmo);
        if (active) glColor3f(1.0f, 1.0f, 0.2f);
        else {
            switch (axis) {
                case AXIS_X: glColor3f(1.0f, 0.2f, 0.2f); break;
                case AXIS_Y: glColor3f(0.2f, 1.0f, 0.2f); break;
                case AXIS_Z: glColor3f(0.2f, 0.4f, 1.0f); break;
                default: break;
            }
        }
    }
    glPushMatrix();
    glTranslatef(o.pos[0], o.pos[1], o.pos[2]);
    glLineWidth(2.8f);
    glBegin(GL_LINES);
    switch (axis) {
        case AXIS_X: glVertex3f(0,0,0); glVertex3f(len,0,0); break;
        case AXIS_Y: glVertex3f(0,0,0); glVertex3f(0,len,0); break;
        case AXIS_Z: glVertex3f(0,0,0); glVertex3f(0,0,len); break;
        default: break;
    }
    glEnd();
    glLineWidth(1.0f);
    switch (axis) {
        case AXIS_X: glTranslatef(len, 0, 0); break;
        case AXIS_Y: glTranslatef(0, len, 0); break;
        case AXIS_Z: glTranslatef(0, 0, len); break;
        default: break;
    }
    glutSolidCube(0.28f);
    glPopMatrix();
}

void drawHandles(const Object3D& o, bool picking) {
    if (g_tmode == TM_TRANSLATE) {
        drawTranslateHandle(o, AXIS_X, 901, picking);
        drawTranslateHandle(o, AXIS_Y, 902, picking);
        drawTranslateHandle(o, AXIS_Z, 903, picking);
    } else if (g_tmode == TM_ROTATE) {
        drawRotateHandle(o, AXIS_X, 911, picking);
        drawRotateHandle(o, AXIS_Y, 912, picking);
        drawRotateHandle(o, AXIS_Z, 913, picking);
    } else if (g_tmode == TM_SCALE) {
        drawScaleHandle(o, AXIS_X, 921, picking);
        drawScaleHandle(o, AXIS_Y, 922, picking);
        drawScaleHandle(o, AXIS_Z, 923, picking);
    }
}

void drawNavGizmo() {
    int sz = 80;
    int mx = g_winW - sz - 15;
    int my = sz + 45;

    glViewport(mx - 35, my - 35, sz + 10, sz + 10);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(30.0, 1.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    Camera* cam = getActiveCamera();
    if (cam) {
        glRotatef(-cam->camAX, 1, 0, 0);
        glRotatef(-cam->camAY, 0, 1, 0);
    }

    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glColor3f(0.85f, 0.18f, 0.18f); glVertex3f(0,0,0); glVertex3f(1.2f,0,0);
        glColor3f(0.18f, 0.85f, 0.18f); glVertex3f(0,0,0); glVertex3f(0,1.2f,0);
        glColor3f(0.18f, 0.40f, 0.90f); glVertex3f(0,0,0); glVertex3f(0,0,1.2f);
    glEnd();
    glLineWidth(1.0f);

    glColor3f(0.85f, 0.18f, 0.18f);
    glRasterPos3f(1.4f, 0, 0); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'X');
    glColor3f(0.18f, 0.85f, 0.18f);
    glRasterPos3f(0, 1.4f, 0); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Y');
    glColor3f(0.18f, 0.40f, 0.90f);
    glRasterPos3f(0, 0, 1.4f); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Z');

    glPushMatrix(); glTranslatef(1.2f, 0, 0); glRotatef(90, 0, 1, 0);
    glColor3f(0.85f, 0.18f, 0.18f); glutSolidCone(0.08f, 0.25f, 12, 4); glPopMatrix();
    glPushMatrix(); glTranslatef(0, 1.2f, 0); glRotatef(-90, 1, 0, 0);
    glColor3f(0.18f, 0.85f, 0.18f); glutSolidCone(0.08f, 0.25f, 12, 4); glPopMatrix();
    glPushMatrix(); glTranslatef(0, 0, 1.2f);
    glColor3f(0.18f, 0.40f, 0.90f); glutSolidCone(0.08f, 0.25f, 12, 4); glPopMatrix();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, g_winW, g_winH);
}