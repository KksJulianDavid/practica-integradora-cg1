/**
 * ============================================================================
 * UNIVERSIDAD CENTRAL DEL ESTE (UCE) · RECINTO PUNTA CANA
 * Computación Gráfica I (ISW-411) — Práctica Integradora (Segundo Parcial)
 * Profesor: Ing. Richard A. Jiménez Rijo
 * 
 * CASO 1: Visualización de datos — Panel de ocupación hotelera
 * Dominio: Análisis de datos y dashboards empresariales
 * Estudiante: Julián Vázquez | Matrícula: 2023-0715
 * ============================================================================
 * 
 * CONCEPTOS EVALUADOS:
 * 1. Definición del sistema de coordenadas con gluOrtho2D (mundo 2D).
 * 2. Primitiva GL_LINES para el trazado de ejes coordenados y líneas guía.
 * 3. Primitiva GL_QUADS para el dibujo de barras rectangulares.
 * 4. Máquina de estados de OpenGL: fijar glColor3f ANTES de cada dibujo.
 * 5. Lógica condicional de semáforo visual según porcentaje de ocupación:
 *    - Verde  (< 70%): Ocupación baja / controlada.
 *    - Ámbar (70% - 89%): Ocupación media / advertencia.
 *    - Rojo   (>= 90%): Ocupación alta / crítica.
 * ============================================================================
 */

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <string.h>

/* Dimensiones iniciales de la ventana en píxeles */
const int ANCHO_VENTANA = 900;
const int ALTO_VENTANA = 650;

/* Datos oficiales del mandato: Porcentaje de ocupación de Lunes a Domingo */
const float ocupacion[7] = {65.0f, 72.0f, 80.0f, 95.0f, 88.0f, 60.0f, 55.0f};
const char* diasSemana[7] = {"LUN", "MAR", "MIE", "JUE", "VIE", "SAB", "DOM"};

/**
 * Función auxiliar para renderizar texto en pantalla usando fuentes bitmap de GLUT.
 * @param x Coordenada X en el mundo virtual
 * @param y Coordenada Y en el mundo virtual
 * @param string Cadena de texto a mostrar
 * @param font Puntero a la fuente bitmap de GLUT
 */
void dibujarTexto(float x, float y, const char* string, void* font) {
    glRasterPos2f(x, y);
    int len = (int)strlen(string);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]);
    }
}

/**
 * Función principal de dibujo (Callback de visualización de GLUT).
 * Se encarga de limpiar el búfer, trazar guías, ejes, barras y leyendas.
 */
void display(void) {
    /* 1. Limpiar la pantalla con el color de fondo establecido (blanco/gris tenue) */
    glClear(GL_COLOR_BUFFER_BIT);

    /* ======================================================================
       A. LÍNEAS GUÍA Y LECTURA DEL EJE VERTICAL (0% a 100%)
       ====================================================================== */
    glLineWidth(1.0f);
    for (int nivel = 20; nivel <= 100; nivel += 20) {
        /* Color gris claro tenue para las líneas guía de fondo */
        glColor3f(0.85f, 0.85f, 0.85f);
        glBegin(GL_LINES);
            glVertex2f(0.0f, (float)nivel);
            glVertex2f(72.0f, (float)nivel);
        glEnd();

        /* Etiqueta numérica del porcentaje en el eje Y */
        glColor3f(0.35f, 0.35f, 0.35f);
        char etiquetaNivel[10];
        sprintf(etiquetaNivel, "%d%%", nivel);
        dibujarTexto(-7.5f, (float)nivel - 1.2f, etiquetaNivel, GLUT_BITMAP_HELVETICA_12);
    }

    /* Etiqueta del 0% */
    glColor3f(0.35f, 0.35f, 0.35f);
    dibujarTexto(-6.0f, -1.2f, "0%", GLUT_BITMAP_HELVETICA_12);

    /* ======================================================================
       B. EJES DEL GRÁFICO (Primitiva GL_LINES)
       ====================================================================== */
    glLineWidth(2.5f);
    glColor3f(0.15f, 0.15f, 0.15f); /* Color gris oscuro formal para ejes */
    glBegin(GL_LINES);
        /* Eje Y: Va desde el origen hasta más allá del 100% */
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.0f, 105.0f);

        /* Eje X: Base horizontal para los 7 días de la semana */
        glVertex2f(0.0f, 0.0f);
        glVertex2f(72.0f, 0.0f);
    glEnd();

    /* ======================================================================
       C. DIBUJO DE BARRAS CON GL_QUADS Y SEMÁFORO CONDICIONAL
       ====================================================================== */
    /*
       Parámetros de distribución:
       - 7 días en total.
       - Ancho de cada barra = 6.0 unidades del mundo.
       - Separación / margen entre barras = 4.0 unidades del mundo.
       - Comienzo de la primera barra = x = 3.0.
    */
    float anchoBarra = 6.0f;
    float separacion = 4.0f;
    float xInicio = 3.0f;

    for (int i = 0; i < 7; i++) {
        float x1 = xInicio + i * (anchoBarra + separacion);
        float x2 = x1 + anchoBarra;
        float yBase = 0.0f;
        float yTope = ocupacion[i];

        /* LÓGICA DE SEMÁFORO (Máquina de estados de OpenGL):
           El color se define ANTES de enviar los vértices de la barra. */
        if (yTope < 70.0f) {
            /* Verde: Ocupación baja (< 70%) */
            glColor3f(0.18f, 0.72f, 0.32f);
        } else if (yTope < 90.0f) {
            /* Ámbar: Ocupación media (70% - 89%) */
            glColor3f(0.95f, 0.72f, 0.12f);
        } else {
            /* Rojo: Ocupación alta / casi lleno (>= 90%) */
            glColor3f(0.88f, 0.18f, 0.18f);
        }

        /* Dibujo del rectángulo de la barra mediante GL_QUADS */
        glBegin(GL_QUADS);
            glVertex2f(x1, yBase); /* Vértice 1: Inferior izquierdo */
            glVertex2f(x2, yBase); /* Vértice 2: Inferior derecho */
            glVertex2f(x2, yTope); /* Vértice 3: Superior derecho */
            glVertex2f(x1, yTope); /* Vértice 4: Superior izquierdo */
        glEnd();

        /* Borde exterior sutil para que las barras luzcan nítidas */
        glColor3f(0.15f, 0.15f, 0.15f);
        glLineWidth(1.2f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(x1, yBase);
            glVertex2f(x2, yBase);
            glVertex2f(x2, yTope);
            glVertex2f(x1, yTope);
        glEnd();

        /* Etiqueta del valor porcentual arriba de cada barra */
        char etiquetaValor[10];
        sprintf(etiquetaValor, "%.0f%%", yTope);
        glColor3f(0.10f, 0.10f, 0.10f);
        dibujarTexto(x1 + 0.6f, yTope + 2.0f, etiquetaValor, GLUT_BITMAP_HELVETICA_12);

        /* Etiqueta del día de la semana debajo del eje X */
        glColor3f(0.20f, 0.20f, 0.20f);
        dibujarTexto(x1 + 0.9f, -6.0f, diasSemana[i], GLUT_BITMAP_HELVETICA_12);
    }

    /* ======================================================================
       D. TÍTULO, SUBTÍTULO Y LEYENDA GERENCIAL DEL SEMÁFORO
       ====================================================================== */
    /* Título principal */
    glColor3f(0.08f, 0.18f, 0.36f);
    dibujarTexto(10.0f, 114.0f, "PANEL DE OCUPACION SEMANAL - REUNION DE GERENCIA", GLUT_BITMAP_HELVETICA_18);

    /* Subtítulo */
    glColor3f(0.40f, 0.40f, 0.40f);
    dibujarTexto(18.0f, 108.5f, "Hotel Punta Cana Resort & Spa | Departamento de TI", GLUT_BITMAP_HELVETICA_12);

    /* Cuadros de la Leyenda del Semáforo en la parte superior derecha */
    float yLeyenda = 114.0f;
    float xLeyenda = 55.0f;

    /* Cuadro Verde */
    glColor3f(0.18f, 0.72f, 0.32f);
    glBegin(GL_QUADS);
        glVertex2f(xLeyenda, yLeyenda);
        glVertex2f(xLeyenda + 2.2f, yLeyenda);
        glVertex2f(xLeyenda + 2.2f, yLeyenda + 2.2f);
        glVertex2f(xLeyenda, yLeyenda + 2.2f);
    glEnd();
    glColor3f(0.25f, 0.25f, 0.25f);
    dibujarTexto(xLeyenda + 3.0f, yLeyenda + 0.5f, "< 70% Baja", GLUT_BITMAP_HELVETICA_10);

    /* Cuadro Ámbar */
    yLeyenda -= 4.0f;
    glColor3f(0.95f, 0.72f, 0.12f);
    glBegin(GL_QUADS);
        glVertex2f(xLeyenda, yLeyenda);
        glVertex2f(xLeyenda + 2.2f, yLeyenda);
        glVertex2f(xLeyenda + 2.2f, yLeyenda + 2.2f);
        glVertex2f(xLeyenda, yLeyenda + 2.2f);
    glEnd();
    glColor3f(0.25f, 0.25f, 0.25f);
    dibujarTexto(xLeyenda + 3.0f, yLeyenda + 0.5f, "70-89% Media", GLUT_BITMAP_HELVETICA_10);

    /* Cuadro Rojo */
    yLeyenda -= 4.0f;
    glColor3f(0.88f, 0.18f, 0.18f);
    glBegin(GL_QUADS);
        glVertex2f(xLeyenda, yLeyenda);
        glVertex2f(xLeyenda + 2.2f, yLeyenda);
        glVertex2f(xLeyenda + 2.2f, yLeyenda + 2.2f);
        glVertex2f(xLeyenda, yLeyenda + 2.2f);
    glEnd();
    glColor3f(0.25f, 0.25f, 0.25f);
    dibujarTexto(xLeyenda + 3.0f, yLeyenda + 0.5f, ">= 90% Alta", GLUT_BITMAP_HELVETICA_10);

    /* 2. Forzar el vaciado de los comandos de OpenGL hacia la pantalla */
    glFlush();
}

/**
 * Función de inicialización de parámetros de OpenGL.
 */
void inicializar(void) {
    /* Color de fondo: blanco marfil limpio (RGB: 0.96, 0.96, 0.97) */
    glClearColor(0.96f, 0.96f, 0.97f, 1.0f);
}

/**
 * Callback de redimensionamiento de ventana (Reshape).
 * Configura la matriz de proyección ortográfica bidimensional (gluOrtho2D)
 * para que los valores de 0 a 100 encajen con holgura para títulos y etiquetas.
 */
void reshape(int ancho, int alto) {
    /* Evitar división por cero si la ventana se minimiza */
    if (alto == 0) alto = 1;

    glViewport(0, 0, ancho, alto);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /*
       Coordenadas del mundo virtual (gluOrtho2D):
       - X mínimo = -12.0: Deja margen a la izquierda para los porcentajes del eje Y.
       - X máximo = 76.0: Cubre las 7 barras y el margen derecho.
       - Y mínimo = -15.0: Margen inferior para los nombres de los días.
       - Y máximo = 125.0: Margen superior para el título y la leyenda del semáforo.
    */
    gluOrtho2D(-12.0, 76.0, -15.0, 125.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * Función principal del programa en C.
 */
int main(int argc, char** argv) {
    /* Inicializar la biblioteca GLUT */
    glutInit(&argc, argv);

    /* Modo de visualización: Búfer simple y modelo de color RGB */
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    /* Configurar tamaño inicial y posición de la ventana */
    glutInitWindowSize(ANCHO_VENTANA, ALTO_VENTANA);
    glutInitWindowPosition(150, 80);

    /* Crear la ventana con título descriptivo */
    glutCreateWindow("Caso 1 - Panel de Ocupacion Hotelera | Julian Vazquez (Mat. 2023-0715)");

    /* Inicializaciones de estado */
    inicializar();

    /* Registro de callbacks */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    /* Entrar en el bucle principal de eventos de GLUT */
    glutMainLoop();
    return 0;
}
