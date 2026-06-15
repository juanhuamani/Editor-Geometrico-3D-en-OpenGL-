#include "scene.h"
#include "globals.h"
#include "camera.h"
#include <fstream>
#include <cstdio>

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