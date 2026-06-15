#include "hud.h"
#include "globals.h"
#include "utils.h"
#include "camera.h"
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <cstdio>

static void drawString(int x, int y, const char* str, void* font) {
    glRasterPos2i(x, y);
    for (const char* c = str; *c; ++c) glutBitmapCharacter(font, *c);
}

static void drawRect(float x, float y, float w, float h, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
        glVertex2f(x, y); glVertex2f(x+w, y); glVertex2f(x+w, y+h); glVertex2f(x, y+h);
    glEnd();
}

static void drawRectBorder(float x, float y, float w, float h, float r, float g, float b, float a) {
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