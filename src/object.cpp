#include "object.h"
#include "globals.h"
#include "utils.h"
#include <cstdio>

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