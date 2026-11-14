# Práctica Integradora: La Computación Gráfica más allá de los videojuegos

**Universidad Central del Este (UCE) · Recinto Punta Cana**  
**Asignatura:** Computación Gráfica I (ISW-411)  
**Profesor:** Ing. Richard A. Jiménez Rijo  
**Estudiante:** Julián Vázquez  
**Matrícula:** 2023-0715  
**Fecha:** Noviembre 2026  

---

## 1. Descripción del Proyecto

Este repositorio contiene las soluciones en **lenguaje C con OpenGL y GLUT** correspondientes a la **Práctica Integradora (Preparación para el Segundo Parcial)**. El objetivo es aplicar los fundamentos de la computación gráfica (primitivas geométricas, sistemas de coordenadas bidimensionales y tridimensionales, máquina de estados, transformaciones, pila de matrices, animación y proyecciones) a casos prácticos de dominios profesionales reales como analítica de datos, simulaciones físicas, robótica y visualización molecular.

---

## 2. Estructura del Repositorio

```text
practica-integradora-cg1/
├── caso1.c                  ← CASO 1: Panel de ocupación hotelera (Visualización de datos)
├── caso2.c                  ← CASO 2: Simulación de partículas de gas (En desarrollo)
├── caso3.c                  ← CASO 3: Brazo robótico articulado (En desarrollo)
├── caso4.c                  ← CASO 4: Modelo molecular 3D (En desarrollo)
├── capturas/                ← Capturas de pantalla de los programas en ejecución
│   └── caso1.png
└── README.md                ← Documentación técnica e instrucciones de compilación
```

---

## 3. CASO 1 · Visualización de Datos — Panel de Ocupación Hotelera

### Contexto y Dominio
Diseño e implementación de un dashboard gerencial para el departamento de TI del **Hotel Punta Cana Resort & Spa**. El visor proyecta la tasa de ocupación porcentual de la semana mediante un gráfico de barras coloreadas según un semáforo de ocupación.

### Datos Representados (Semana Completa)
| Día | Lunes | Martes | Miércoles | Jueves | Viernes | Sábado | Domingo |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| **Ocupación (%)** | 65 % | 72 % | 80 % | 95 % | 88 % | 60 % | 55 % |
| **Semáforo** | 🟢 Baja | 🟡 Media | 🟡 Media | 🔴 Alta | 🟡 Media | 🟢 Baja | 🟢 Baja |

### Conceptos Técnicos de OpenGL Aplicados
1. **Sistema de Coordenadas (`gluOrtho2D`):**  
   Se definió una ventana de proyección ortográfica `gluOrtho2D(-12.0, 76.0, -15.0, 125.0)` en la función `reshape()`. Esto garantiza que los valores de 0 a 100 de las barras queden enmarcados con márgenes equilibrados para las etiquetas porcentuales del eje Y y los nombres de los días en el eje X.
2. **Ejes del Gráfico con `GL_LINES`:**  
   Se utilizó la primitiva de líneas para dibujar el eje vertical Y (0 a 105) y el eje horizontal X (0 a 72), así como las líneas de referencia horizontales punteadas/tenues para cada múltiplo del 20 %.
3. **Barras Rectangulares con `GL_QUADS`:**  
   Cada barra se construye mediante 4 vértices con `GL_QUADS`. La base se ancla en `y = 0` y la altura corresponde exactamente al valor de ocupación del día `y = porcentaje`.
4. **Semáforo Condicional (`glColor3f` y Máquina de Estados):**  
   Antes de emitir los vértices de cada barra, el color de OpenGL se conmuta mediante una estructura condicional `if/else`:
   * **Verde (`< 70%`):** `glColor3f(0.18f, 0.72f, 0.32f)` — Ocupación normal/baja.
   * **Ámbar (`70% - 89%`):** `glColor3f(0.95f, 0.72f, 0.12f)` — Ocupación media.
   * **Rojo (`>= 90%`):** `glColor3f(0.88f, 0.18f, 0.18f)` — Ocupación alta / sobrecapacidad.
5. **Tipografía Bitmap con GLUT:**  
   Se implementó la función auxiliar `dibujarTexto()` utilizando `glRasterPos2f` y `glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c)` para rotular los días, las cifras exactas sobre las barras y la leyenda explicativa superior.

### Captura de Pantalla en Ejecución
![Panel de Ocupación Hotelera](capturas/caso1.png)

---

## 4. Instrucciones de Compilación y Ejecución

### Opción A: Compilación por Línea de Comandos (MinGW / GCC en Windows)
Si utilizas MinGW con la librería `freeglut` instalada:
```bash
gcc caso1.c -o caso1.exe -lfreeglut -lopengl32 -lglu32
./caso1.exe
```

### Opción B: Code::Blocks / Dev-C++ (Windows)
1. Crear un proyecto de tipo **GLUT Project** o **Console Application**.
2. Agregar `caso1.c` al proyecto.
3. En las opciones del compilador (**Project -> Build Options -> Linker Settings**), asegurar los enlaces a las librerías:
   * `-lfreeglut` (o `-lglut32`)
   * `-lopengl32`
   * `-lglu32`
4. Presionar **Build and Run** (`F9`).

### Opción C: Linux (Ubuntu / Debian)
```bash
sudo apt-get install freeglut3-dev
gcc caso1.c -o caso1 -lGL -lGLU -lglut
./caso1
```
