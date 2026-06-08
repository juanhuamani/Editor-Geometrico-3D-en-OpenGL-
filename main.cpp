#include <GL/freeglut.h>
#include <GL/glu.h>
#include <vector>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>

const float PI = 3.14159265f;

enum ObjType   { CUBO, ESFERA, TORO, TETERA };
enum TMode     { TM_TRANSLATE, TM_ROTATE, TM_SCALE };
enum Axis      { AXIS_NONE, AXIS_X, AXIS_Y, AXIS_Z };

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

Camera* getActiveCamera() {
    if (g_activeCam >= 0 && g_activeCam < (int)g_cameras.size())
        return &g_cameras[g_activeCam];
    return nullptr;
}

Camera createDefaultCamera() {
    Camera c;
    c.id = g_nextCamId++;
    std::snprintf(c.name, sizeof(c.name), "Perspectiva %d", c.id);
    c.camDist = 22.0f; c.camAX = 35.0f; c.camAY = 50.0f;
    c.center[0] = 0; c.center[1] = 0; c.center[2] = 0;
    c.fov = 55.0f; c.nearP = 0.1f; c.farP = 250.0f;
    return c;
}

Camera createFrontCamera() {
    Camera c;
    c.id = g_nextCamId++;
    std::snprintf(c.name, sizeof(c.name), "Frontal %d", c.id);
    c.camDist = 22.0f; c.camAX = 0.0f; c.camAY = 0.0f;
    c.center[0] = 0; c.center[1] = 0; c.center[2] = 0;
    c.fov = 55.0f; c.nearP = 0.1f; c.farP = 250.0f;
    return c;
}

Camera createTopCamera() {
    Camera c;
    c.id = g_nextCamId++;
    std::snprintf(c.name, sizeof(c.name), "Superior %d", c.id);
    c.camDist = 22.0f; c.camAX = 89.0f; c.camAY = 0.0f;
    c.center[0] = 0; c.center[1] = 0; c.center[2] = 0;
    c.fov = 55.0f; c.nearP = 0.1f; c.farP = 250.0f;
    return c;
}

Camera createSideCamera() {
    Camera c;
    c.id = g_nextCamId++;
    std::snprintf(c.name, sizeof(c.name), "Lateral %d", c.id);
    c.camDist = 22.0f; c.camAX = 0.0f; c.camAY = 90.0f;
    c.center[0] = 0; c.center[1] = 0; c.center[2] = 0;
    c.fov = 55.0f; c.nearP = 0.1f; c.farP = 250.0f;
    return c;
}

void addObject(ObjType type) {
    Object3D obj;
    obj.id = g_nextId++;
    obj.type = type;
    obj.pos[0] = 0; obj.pos[1] = 0; obj.pos[2] = 0;
    obj.rot[0] = 0; obj.rot[1] = 0; obj.rot[2] = 0;
    obj.scale[0] = 1; obj.scale[1] = 1; obj.scale[2] = 1;
    randomColor(obj.color);
    g_objects.push_back(obj);
    g_selectedId = obj.id;
    printf("[Creado] ID=%d  Tipo=%s\n", obj.id, typeToString(obj.type));
}

void deleteSelected() {
    if (g_selectedId < 0) return;
    for (size_t i = 0; i < g_objects.size(); ++i) {
        if (g_objects[i].id == g_selectedId) {
            printf("[Eliminado] ID=%d\n", g_selectedId);
            g_objects.erase(g_objects.begin() + i);
            g_selectedId = -1;
            return;
        }
    }
}

void duplicateSelected() {
    Object3D* src = findObject(g_selectedId);
    if (!src) return;
    Object3D obj = *src;
    obj.id = g_nextId++;
    obj.pos[0] += 2.5f;
    g_objects.push_back(obj);
    g_selectedId = obj.id;
    printf("[Duplicado] nuevo ID=%d\n", obj.id);
}

void saveScene(const char* filename) {
    std::ofstream f(filename);
    if (!f.is_open()) { printf("[Error] No se pudo abrir %s\n", filename); return; }
    f << "# Escena Editor 3D Lab 7\n";
    f << g_objects.size() << "\n";
    for (const auto& o : g_objects) {
        f << o.id << " " << o.type << " "
          << o.pos[0] << " " << o.pos[1] << " " << o.pos[2] << " "
          << o.rot[0] << " " << o.rot[1] << " " << o.rot[2] << " "
          << o.scale[0] << " " << o.scale[1] << " " << o.scale[2] << " "
          << o.color[0] << " " << o.color[1] << " " << o.color[2] << "\n";
    }
    Camera* cam = getActiveCamera();
    if (cam) {
        f << "# Camara activa\n";
        f << cam->camDist << " " << cam->camAX << " " << cam->camAY << " "
          << cam->center[0] << " " << cam->center[1] << " " << cam->center[2] << " "
          << cam->fov << " " << cam->nearP << " " << cam->farP << "\n";
    }
    f.close();
    printf("[Guardado] %s (%zu objetos)\n", filename, g_objects.size());
}

void loadScene(const char* filename) {
    std::ifstream f(filename);
    if (!f.is_open()) { printf("[Error] No se pudo abrir %s\n", filename); return; }
    std::string line;
    std::getline(f, line);
    size_t n;
    f >> n;
    g_objects.clear();
    g_selectedId = -1;
    for (size_t i = 0; i < n; ++i) {
        Object3D o;
        int typeInt;
        f >> o.id >> typeInt
          >> o.pos[0] >> o.pos[1] >> o.pos[2]
          >> o.rot[0] >> o.rot[1] >> o.rot[2]
          >> o.scale[0] >> o.scale[1] >> o.scale[2]
          >> o.color[0] >> o.color[1] >> o.color[2];
        o.type = (ObjType)typeInt;
        g_objects.push_back(o);
        if (o.id >= g_nextId) g_nextId = o.id + 1;
    }
    Camera* cam = getActiveCamera();
    if (cam) {
        f >> cam->camDist >> cam->camAX >> cam->camAY
          >> cam->center[0] >> cam->center[1] >> cam->center[2]
          >> cam->fov >> cam->nearP >> cam->farP;
    }
    f.close();
    printf("[Cargado] %s (%zu objetos)\n", filename, g_objects.size());
}

void setCamera() {
    Camera* cam = getActiveCamera();
    if (!cam) return;
    float rx = cam->camAX * PI / 180.0f;
    float ry = cam->camAY * PI / 180.0f;
    float ex = cam->center[0] + cam->camDist * std::cos(rx) * std::sin(ry);
    float ey = cam->center[1] + cam->camDist * std::sin(rx);
    float ez = cam->center[2] + cam->camDist * std::cos(rx) * std::cos(ry);
    gluLookAt(ex, ey, ez, cam->center[0], cam->center[1], cam->center[2], 0, 1, 0);
}

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

void setPickColor(int id) {
    unsigned char r = (id & 0xFF);
    unsigned char g = ((id >> 8) & 0xFF);
    unsigned char b = ((id >> 16) & 0xFF);
    glColor3ub(r, g, b);
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

int pickAt(int x, int y) {
    glDisable(GL_DITHER);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    Camera* cam = getActiveCamera();
    if (cam) gluPerspective(cam->fov, (double)g_winW / (double)g_winH, cam->nearP, cam->farP);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    setCamera();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& o : g_objects) {
        setPickColor(o.id);
        glPushMatrix();
        glTranslatef(o.pos[0], o.pos[1], o.pos[2]);
        glRotatef(o.rot[0], 1, 0, 0);
        glRotatef(o.rot[1], 0, 1, 0);
        glRotatef(o.rot[2], 0, 0, 1);
        glScalef(o.scale[0], o.scale[1], o.scale[2]);
        switch (o.type) {
            case CUBO:   glutSolidCube(1.5f); break;
            case ESFERA: glutSolidSphere(1.0f, 24, 24); break;
            case TORO:   glutSolidTorus(0.3f, 0.8f, 24, 24); break;
            case TETERA: glutSolidTeapot(1.0f); break;
        }
        glPopMatrix();
    }

    if (g_selectedId >= 0) {
        if (Object3D* o = findObject(g_selectedId)) {
            drawHandles(*o, true);
        }
    }

    glFlush();

    GLubyte pixel[3];
    glReadPixels(x, g_winH - 1 - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    int pickedId = pixel[0] | (pixel[1] << 8) | (pixel[2] << 16);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DITHER);
    return pickedId;
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

void drawString(int x, int y, const char* str, void* font) {
    glRasterPos2i(x, y);
    for (const char* c = str; *c; ++c) glutBitmapCharacter(font, *c);
}

void drawRect(float x, float y, float w, float h, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
        glVertex2f(x, y); glVertex2f(x+w, y); glVertex2f(x+w, y+h); glVertex2f(x, y+h);
    glEnd();
}

void drawRectBorder(float x, float y, float w, float h, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y); glVertex2f(x+w, y); glVertex2f(x+w, y+h); glVertex2f(x, y+h);
    glEnd();
}

void drawHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, g_winW, 0, g_winH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    char buf[180];

    drawRect(0, g_winH - 30, g_winW, 30, 0.10f, 0.10f, 0.12f, 0.97f);
    drawRectBorder(0, g_winH - 30, g_winW, 30, 0.18f, 0.18f, 0.22f, 1.0f);

    drawRect(0, g_winH - 30, 120, 30, 0.22f, 0.40f, 0.65f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawString(12, g_winH - 20, "3D Viewport", GLUT_BITMAP_HELVETICA_12);

    Camera* cam = getActiveCamera();
    if (cam) {
        std::snprintf(buf, sizeof(buf), "%s | FOV:%.0f Near:%.2f Far:%.0f Obj:%zu",
            cam->name, cam->fov, cam->nearP, cam->farP, g_objects.size());
        glColor3f(0.70f, 0.72f, 0.78f);
        drawString(140, g_winH - 20, buf, GLUT_BITMAP_9_BY_15);
    }

    {
        float mr, mg, mb;
        const char* mlabel;
        if (g_tmode == TM_TRANSLATE) { mr=0.18f; mg=0.75f; mb=0.18f; mlabel = "MOVER"; }
        else if (g_tmode == TM_ROTATE) { mr=0.80f; mg=0.65f; mb=0.10f; mlabel = "ROTAR"; }
        else { mr=0.60f; mg=0.25f; mb=0.85f; mlabel = "ESCALAR"; }
        int mw = 100;
        drawRect(g_winW - mw - 10, g_winH - 28, mw, 26, mr, mg, mb, 0.90f);
        glColor3f(1,1,1);
        drawString(g_winW - mw, g_winH - 20, mlabel, GLUT_BITMAP_HELVETICA_12);
    }

    if (g_showOutliner) {
        int pw = 220;
        int topY = g_winH - 32;
        int botY = 38;
        int ph = topY - botY;

        drawRect(2, botY, pw, ph, 0.09f, 0.09f, 0.11f, 0.95f);
        drawRectBorder(2, botY, pw, ph, 0.20f, 0.20f, 0.25f, 1.0f);

        // Outliner header
        drawRect(2, topY - 24, pw, 24, 0.14f, 0.20f, 0.14f, 1.0f);
        glColor3f(0.60f, 0.90f, 0.60f);
        drawString(12, topY - 16, "SCENE COLLECTION", GLUT_BITMAP_HELVETICA_12);

        int oy = topY - 48;
        for (const auto& o : g_objects) {
            bool sel = (o.id == g_selectedId);
            if (sel) {
                drawRect(6, oy - 4, pw - 12, 22, 0.15f, 0.30f, 0.55f, 0.92f);
                drawRectBorder(6, oy - 4, pw - 12, 22, 0.30f, 0.55f, 0.90f, 0.8f);
                glColor3f(1.0f, 0.95f, 0.35f);
            } else {
                drawRect(6, oy - 4, pw - 12, 22, 0.12f, 0.12f, 0.14f, 0.80f);
                glColor3f(0.68f, 0.70f, 0.76f);
            }
            const char* icon = "?";
            switch (o.type) {
                case CUBO: icon = "\x03"; break;
                case ESFERA: icon = "\x04"; break;
                case TORO: icon = "\x05"; break;
                case TETERA: icon = "\x06"; break;
            }
            (void)icon;
            std::snprintf(buf, sizeof(buf), "%s  [ID %d]", typeToString(o.type), o.id);
            drawString(14, oy, buf, GLUT_BITMAP_9_BY_15);
            // Color swatch
            drawRect(pw - 32, oy - 2, 14, 14, o.color[0], o.color[1], o.color[2], 1.0f);
            drawRectBorder(pw - 32, oy - 2, 14, 14, 0.4f, 0.4f, 0.4f, 0.6f);
            oy -= 26;
            if (oy < botY + 10) break;
        }
        if (g_objects.empty()) {
            glColor3f(0.45f, 0.45f, 0.50f);
            drawString(20, topY - 60, "(Escena vacia)", GLUT_BITMAP_9_BY_15);
        }
    }

    if (g_showProperties) {
        int pw = 260;
        int topY = g_winH - 32;
        int botY = 38;
        int ph = topY - botY;
        int px = g_winW - pw - 2;

        drawRect(px, botY, pw, ph, 0.09f, 0.09f, 0.11f, 0.95f);
        drawRectBorder(px, botY, pw, ph, 0.20f, 0.20f, 0.25f, 1.0f);

        drawRect(px, topY - 24, pw, 24, 0.14f, 0.14f, 0.28f, 1.0f);
        glColor3f(0.70f, 0.72f, 0.95f);
        drawString(px + 10, topY - 16, "PROPERTIES", GLUT_BITMAP_HELVETICA_12);

        int iy = topY - 48;

        if (g_selectedId >= 0) {
            Object3D* o = findObject(g_selectedId);
            if (o) {
                drawRect(px + 4, iy - 6, pw - 8, 22, 0.18f, 0.25f, 0.40f, 0.85f);
                glColor3f(1.0f, 0.92f, 0.30f);
                std::snprintf(buf, sizeof(buf), "%s  [ID %d]", typeToString(o->type), o->id);
                drawString(px + 10, iy, buf, GLUT_BITMAP_HELVETICA_12);
                iy -= 32;

                drawRect(px + 4, iy - 4, pw - 8, 18, 0.14f, 0.22f, 0.14f, 0.95f);
                glColor3f(0.55f, 0.90f, 0.55f);
                drawString(px + 10, iy, "Transform", GLUT_BITMAP_HELVETICA_12);
                iy -= 24;

                glColor3f(0.85f, 0.30f, 0.30f);
                std::snprintf(buf, sizeof(buf), "  Location X  %8.2f", o->pos[0]);
                drawString(px + 10, iy, buf, GLUT_BITMAP_9_BY_15); iy -= 18;
                glColor3f(0.30f, 0.85f, 0.30f);
                std::snprintf(buf, sizeof(buf), "  Location Y  %8.2f", o->pos[1]);
                drawString(px + 10, iy, buf, GLUT_BITMAP_9_BY_15); iy -= 18;
                glColor3f(0.30f, 0.50f, 0.95f);
                std::snprintf(buf, sizeof(buf), "  Location Z  %8.2f", o->pos[2]);
                drawString(px + 10, iy, buf, GLUT_BITMAP_9_BY_15); iy -= 24;

                glColor3f(0.90f, 0.72f, 0.18f);
                std::snprintf(buf, sizeof(buf), "  Rotation    %.1f / %.1f / %.1f", o->rot[0], o->rot[1], o->rot[2]);
                drawString(px + 10, iy, buf, GLUT_BITMAP_9_BY_15); iy -= 20;

                glColor3f(0.72f, 0.48f, 0.90f);
                std::snprintf(buf, sizeof(buf), "  Scale       %.2f / %.2f / %.2f", o->scale[0], o->scale[1], o->scale[2]);
                drawString(px + 10, iy, buf, GLUT_BITMAP_9_BY_15); iy -= 28;

                drawRect(px + 4, iy - 4, pw - 8, 18, 0.14f, 0.14f, 0.22f, 0.95f);
                glColor3f(0.70f, 0.70f, 0.95f);
                drawString(px + 10, iy, "Material", GLUT_BITMAP_HELVETICA_12);
                iy -= 24;

                drawRect(px + 14, iy - 1, 22, 16, o->color[0], o->color[1], o->color[2], 1.0f);
                drawRectBorder(px + 14, iy - 1, 22, 16, 0.5f, 0.5f, 0.5f, 1.0f);
                glColor3f(0.78f, 0.78f, 0.82f);
                std::snprintf(buf, sizeof(buf), "  R %.2f  G %.2f  B %.2f", o->color[0], o->color[1], o->color[2]);
                drawString(px + 42, iy + 1, buf, GLUT_BITMAP_9_BY_15);
                iy -= 28;

                if (g_draggingGizmo && g_activeAxis != AXIS_NONE) {
                    drawRect(px + 4, iy - 4, pw - 8, 18, 0.40f, 0.35f, 0.10f, 0.95f);
                    glColor3f(1.0f, 0.92f, 0.10f);
                    std::snprintf(buf, sizeof(buf), "Editando eje %s...", axisToString(g_activeAxis));
                    drawString(px + 10, iy, buf, GLUT_BITMAP_9_BY_15);
                } else {
                    glColor3f(0.50f, 0.50f, 0.55f);
                    drawString(px + 10, iy, "Arrastra gizmo para editar", GLUT_BITMAP_8_BY_13);
                }
            }
        } else {
            glColor3f(0.45f, 0.45f, 0.50f);
            drawString(px + 30, topY - 70, "Sin objeto seleccionado", GLUT_BITMAP_9_BY_15);
        }
    }

    {
        int tbH = 36;
        int tbY = 0;
        drawRect(0, tbY, g_winW, tbH, 0.10f, 0.10f, 0.12f, 0.97f);
        drawRectBorder(0, tbY, g_winW, tbH, 0.20f, 0.20f, 0.25f, 1.0f);

        int bx = 8;
        int by = 7;

        drawRect(bx, by, 62, 22, 0.18f, 0.42f, 0.18f, 0.92f);
        drawRectBorder(bx, by, 62, 22, 0.28f, 0.55f, 0.28f, 1.0f);
        glColor3f(0.85f, 0.95f, 0.85f);
        drawString(bx + 4, by + 5, "[1]Cube", GLUT_BITMAP_8_BY_13);
        bx += 68;

        drawRect(bx, by, 68, 22, 0.18f, 0.30f, 0.52f, 0.92f);
        drawRectBorder(bx, by, 68, 22, 0.28f, 0.42f, 0.65f, 1.0f);
        glColor3f(0.85f, 0.90f, 0.95f);
        drawString(bx + 4, by + 5, "[2]Sphere", GLUT_BITMAP_8_BY_13);
        bx += 74;

        drawRect(bx, by, 52, 22, 0.50f, 0.28f, 0.18f, 0.92f);
        drawRectBorder(bx, by, 52, 22, 0.60f, 0.38f, 0.28f, 1.0f);
        glColor3f(0.95f, 0.88f, 0.80f);
        drawString(bx + 4, by + 5, "[3]Tor", GLUT_BITMAP_8_BY_13);
        bx += 58;

        drawRect(bx, by, 58, 22, 0.52f, 0.32f, 0.14f, 0.92f);
        drawRectBorder(bx, by, 58, 22, 0.62f, 0.42f, 0.24f, 1.0f);
        glColor3f(0.95f, 0.90f, 0.78f);
        drawString(bx + 4, by + 5, "[4]Tea", GLUT_BITMAP_8_BY_13);
        bx += 66;

        drawRect(bx, by + 2, 2, 18, 0.35f, 0.35f, 0.40f, 0.7f);
        bx += 8;

        float tmr, tmg, tmb;
        if (g_tmode == TM_TRANSLATE) { tmr=0.12f; tmg=0.58f; tmb=0.12f; }
        else { tmr=0.22f; tmg=0.22f; tmb=0.25f; }
        drawRect(bx, by, 55, 22, tmr, tmg, tmb, 0.95f);
        drawRectBorder(bx, by, 55, 22, 0.30f, 0.55f, 0.30f, g_tmode==TM_TRANSLATE?1.0f:0.4f);
        glColor3f(1,1,1);
        drawString(bx + 4, by + 5, "[T]Move", GLUT_BITMAP_8_BY_13);
        bx += 61;

        if (g_tmode == TM_ROTATE) { tmr=0.58f; tmg=0.48f; tmb=0.10f; }
        else { tmr=0.22f; tmg=0.22f; tmb=0.25f; }
        drawRect(bx, by, 50, 22, tmr, tmg, tmb, 0.95f);
        drawRectBorder(bx, by, 50, 22, 0.60f, 0.55f, 0.20f, g_tmode==TM_ROTATE?1.0f:0.4f);
        drawString(bx + 4, by + 5, "[R]Rot", GLUT_BITMAP_8_BY_13);
        bx += 56;

        if (g_tmode == TM_SCALE) { tmr=0.45f; tmg=0.20f; tmb=0.60f; }
        else { tmr=0.22f; tmg=0.22f; tmb=0.25f; }
        drawRect(bx, by, 48, 22, tmr, tmg, tmb, 0.95f);
        drawRectBorder(bx, by, 48, 22, 0.55f, 0.30f, 0.70f, g_tmode==TM_SCALE?1.0f:0.4f);
        drawString(bx + 4, by + 5, "[S]Scl", GLUT_BITMAP_8_BY_13);
        bx += 56;

        drawRect(bx, by + 2, 2, 18, 0.35f, 0.35f, 0.40f, 0.7f);
        bx += 8;

        float wr, wg, wb;
        if (g_wireframe) { wr=0.55f; wg=0.22f; wb=0.22f; }
        else { wr=0.22f; wg=0.22f; wb=0.25f; }
        drawRect(bx, by, 62, 22, wr, wg, wb, 0.92f);
        drawRectBorder(bx, by, 62, 22, 0.5f, 0.3f, 0.3f, g_wireframe?1.0f:0.4f);
        glColor3f(1,1,1);
        drawString(bx + 4, by + 5, g_wireframe ? "[W]Wire" : "[W]Solid", GLUT_BITMAP_8_BY_13);
        bx += 68;

        drawRect(bx, by, 52, 22, 0.18f, 0.32f, 0.48f, 0.92f);
        drawRectBorder(bx, by, 52, 22, 0.28f, 0.42f, 0.58f, 1.0f);
        drawString(bx + 4, by + 5, "[D]Dup", GLUT_BITMAP_8_BY_13);
        bx += 58;

        drawRect(bx, by, 56, 22, 0.50f, 0.16f, 0.16f, 0.92f);
        drawRectBorder(bx, by, 56, 22, 0.60f, 0.26f, 0.26f, 1.0f);
        drawString(bx + 4, by + 5, "[Del]Del", GLUT_BITMAP_8_BY_13);
        bx += 64;

        drawRect(bx, by, 52, 22, 0.28f, 0.42f, 0.28f, 0.92f);
        drawRectBorder(bx, by, 52, 22, 0.38f, 0.55f, 0.38f, 1.0f);
        drawString(bx + 4, by + 5, "F5=Save", GLUT_BITMAP_8_BY_13);
        bx += 58;

        drawRect(bx, by, 52, 22, 0.28f, 0.32f, 0.48f, 0.92f);
        drawRectBorder(bx, by, 52, 22, 0.38f, 0.42f, 0.58f, 1.0f);
        drawString(bx + 4, by + 5, "F6=Load", GLUT_BITMAP_8_BY_13);
    }

    {
        int sz = 80;
        int mx = g_winW - sz - 15;
        int my = sz + 45;
        drawRect(mx - 42, my - 42, sz + 24, sz + 24, 0.06f, 0.06f, 0.08f, 0.85f);
        drawRectBorder(mx - 42, my - 42, sz + 24, sz + 24, 0.18f, 0.18f, 0.22f, 1.0f);
    }

    if (g_showHelp) {
        int pw = 460;
        int ph = 155;
        int px = 228;
        int py = ph + 38;
        drawRect(px, py - ph, pw, ph, 0.05f, 0.05f, 0.08f, 0.93f);
        drawRectBorder(px, py - ph, pw, ph, 0.25f, 0.25f, 0.30f, 1.0f);

        int hy = py - 18;
        glColor3f(0.95f, 0.90f, 0.25f);
        drawString(px + 10, hy, "ATAJOS DE TECLADO", GLUT_BITMAP_HELVETICA_12); hy -= 20;
        glColor3f(0.72f, 0.82f, 1.0f);
        drawString(px + 10, hy, "Click izq = Seleccionar   |   Click der = Menu crear", GLUT_BITMAP_8_BY_13); hy -= 17;
        drawString(px + 10, hy, "Arrastrar izq = Rotar Trackball  |  Shift/Mid=Pan", GLUT_BITMAP_8_BY_13); hy -= 17;
        drawString(px + 10, hy, "Rueda = Zoom   |   T/R/S = Modo Mover/Rotar/Escalar", GLUT_BITMAP_8_BY_13); hy -= 17;
        drawString(px + 10, hy, "D = Duplicar  |  SUPR = Borrar  |  W = Wireframe/Solido", GLUT_BITMAP_8_BY_13); hy -= 17;
        drawString(px + 10, hy, "[/] = FOV  |  ,/.  = Near  |  ;/' = Far  |  C = Camara", GLUT_BITMAP_8_BY_13); hy -= 17;
        drawString(px + 10, hy, "F5 = Guardar   |   F6 = Cargar   |   H = Ayuda   |   ESC = Salir", GLUT_BITMAP_8_BY_13);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Camera* cam = getActiveCamera();
    if (cam) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(cam->fov, (double)g_winW / (double)g_winH, cam->nearP, cam->farP);
        glMatrixMode(GL_MODELVIEW);
    }
    glLoadIdentity();

    setCamera();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = {8.0f, 16.0f, 6.0f, 1.0f};
    GLfloat lightAmb[] = {0.22f, 0.22f, 0.25f, 1.0f};
    GLfloat lightDif[] = {0.88f, 0.85f, 0.80f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    glDisable(GL_LIGHTING);

    drawGrid();
    drawShadows();

    for (const auto& o : g_objects) {
        drawObject(o, o.id == g_selectedId);
    }

    glDisable(GL_LIGHTING);
    if (g_selectedId >= 0) {
        if (Object3D* o = findObject(g_selectedId)) {
            drawHandles(*o, false);
        }
    }

    drawNavGizmo();
    drawHUD();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    g_winW = w; g_winH = h;
    glViewport(0, 0, w, h);
    Camera* cam = getActiveCamera();
    if (cam) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(cam->fov, (double)w / (double)h, cam->nearP, cam->farP);
        glMatrixMode(GL_MODELVIEW);
    }
}

bool g_dragStartedOnUI = false;

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

void menuCallback(int val) {
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

void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    GLfloat ambient[] = {0.18f, 0.18f, 0.20f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glDisable(GL_LIGHTING);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(g_winW, g_winH);
    glutCreateWindow("Editor Grafico 3D - Lab 7 (Blender Style)");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.09f, 0.09f, 0.11f, 1.0f);
    initLighting();

    g_cameras.push_back(createDefaultCamera());
    g_cameras.push_back(createFrontCamera());
    g_cameras.push_back(createTopCamera());
    g_cameras.push_back(createSideCamera());

    printf("========================================================\n");
    printf("  Editor Grafico 3D estilo Blender - Lab 7\n");
    printf("========================================================\n\n");
    printf("Click izq = Seleccionar | Click der = Menu\n");
    printf("Arrastrar = Trackball | Shift/Mid = Pan | Rueda = Zoom\n");
    printf("T/R/S = Modo | D = Duplicar | SUPR = Borrar | W = Wire\n");
    printf("[/] = FOV | ,/. = Near | ;/' = Far | C = Camara\n");
    printf("F5 = Guardar | F6 = Cargar | H = Ayuda | ESC = Salir\n\n");

    addObject(CUBO);
    if (Object3D* o = findObject(g_selectedId)) { o->pos[0] = -3.0f; o->pos[1] = 0.8f; o->color[0]=0.9f; o->color[1]=0.35f; o->color[2]=0.35f; }
    addObject(ESFERA);
    if (Object3D* o = findObject(g_selectedId)) { o->pos[0] = 3.0f; o->pos[1] = 1.0f; o->color[0]=0.35f; o->color[1]=0.6f; o->color[2]=0.9f; }
    addObject(TORO);
    if (Object3D* o = findObject(g_selectedId)) { o->pos[2] = 3.0f; o->pos[1] = 0.8f; o->color[0]=0.4f; o->color[1]=0.9f; o->color[2]=0.4f; }
    addObject(TETERA);
    if (Object3D* o = findObject(g_selectedId)) { o->pos[2] = -3.0f; o->pos[1] = 0.9f; o->color[0]=0.9f; o->color[1]=0.8f; o->color[2]=0.2f; }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    createMenu();

    glutMainLoop();
    return 0;
}
