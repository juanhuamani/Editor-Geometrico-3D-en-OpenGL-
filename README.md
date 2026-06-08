# Editor Geometrico 3D en OpenGL

**Laboratorio 7 - Computacion Grafica**

Editor grafico 3D interactivo estilo Blender desarrollado con OpenGL (FreeGLUT/GLU). Permite crear, seleccionar, transformar y gestionar primitivas 3D en una escena con interfaz tipo viewport.

## Caracteristicas

- **Primitivas 3D**: Cubo, Esfera, Toro y Tetera
- **Transformaciones interactivas**: Trasladar, Rotar y Escalar mediante gizmos 3D
- **Seleccion por color picking**: Click izquierdo para seleccionar objetos
- **Multiples camaras**: Perspectiva, Frontal, Superior y Lateral
- **Iluminacion y sombras**: Renderizado solido/wireframe con sombras planas
- **Interfaz estilo Blender**: Toolbar, Outliner, Panel de propiedades, Nav Gizmo
- **Guardar/Cargar escenas**: Persistencia a archivo `escena.lab7`
- **Menu contextual**: Click derecho para acceso rapido a todas las opciones

## Controles

| Tecla          | Accion                              |
|----------------|-------------------------------------|
| `1`            | Agregar Cubo                        |
| `2`            | Agregar Esfera                      |
| `3`            | Agregar Toro                        |
| `4`            | Agregar Tetera                     |
| `T`            | Modo Traslacion                     |
| `R`            | Modo Rotacion                       |
| `S`            | Modo Escalado                       |
| `W`            | Alternar Wireframe/Solido          |
| `D`            | Duplicar objeto seleccionado        |
| `SUPR / Backspace` | Eliminar objeto seleccionado  |
| `C`            | Cambiar camara                      |
| `[` / `]`      | Decrementar/Incrementar FOV         |
| `,` / `.`      | Decrementar/Incrementar Near Plane  |
| `;` / `'`      | Decrementar/Incrementar Far Plane   |
| `F5`           | Guardar escena                      |
| `F6`           | Cargar escena                       |
| `H`            | Mostrar/Ocultar ayuda               |
| `ESC`          | Salir                               |

### Mouse

| Accion                          | Resultado           |
|---------------------------------|---------------------|
| Click izquierdo                 | Seleccionar objeto  |
| Click derecho                   | Menu contextual     |
| Arrastrar izquierdo             | Trackball (rotar camara) |
| Arrastrar izquierdo + Shift     | Pan camara          |
| Click medio / Arrastrar medio   | Pan camara          |
| Rueda del mouse                 | Zoom                |
| Arrastrar sobre gizmo           | Transformar objeto  |

## Compilar y Ejecutar

### Requisitos

- `g++` (soporte C++11)
- Librerias: `libGL`, `libGLU`, `libglut` (FreeGLUT)

### Instalar dependencias (Debian/Ubuntu)

```bash
sudo apt-get install g++ freeglut3-dev
```

### Compilar

```bash
make
```

### Ejecutar

```bash
make run
```

O directamente:

```bash
./editor3d
```

### Limpiar

```bash
make clean
```

## Estructura del Proyecto

```
Lab 7/
├── main.cpp      # Codigo fuente principal (OpenGL + FreeGLUT)
├── Makefile       # Compilacion automatizada
├── editor3d       # Binario compilado
└── README.md      # Este archivo
```

## Detalles Tecnicos

- **Picking**: Implementado mediante color-coded rendering y `glReadPixels`
- **Gizmos**: Manejadores visuales para translate (flechas), rotate (toros) y scale (cubos)
- **Camara orbital**: Sistema de camara esferica con soporte para pan, zoom y rotacion
- **Sombras**: Proyeccion plana de sombras usando matriz de proyeccion sobre el plano Y=0
- **HUD**: Interfaz 2D overlay con toolbar, outliner, panel de propiedades y navigational gizmo
- **Escena**: Formato de archivo de texto propio para guardar/cargar estado de objetos y camara