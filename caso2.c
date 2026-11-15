/**
 * ============================================================================
 * UNIVERSIDAD CENTRAL DEL ESTE (UCE) - RECINTO PUNTA CANA
 * ASIGNATURA: COMPUTACION GRAFICA I (ISW-411)
 * PROFESOR:   ING. RICHARD A. JIMENEZ RIJO
 * ESTUDIANTE: JULIAN VAZQUEZ (MATRICULA: 2023-0715)
 * 
 * CASO 2: SIMULACION CIENTIFICA - PARTICULAS DE UN GAS EN UN CONTENEDOR 2D
 * ============================================================================
 * Descripcion:
 * Animacion grafica de 20 particulas de gas confinadas dentro de un recipiente
 * hermetico rectangular. Cada particula se desplaza con velocidad uniforme y
 * experimenta rebotes elasticos perfectos al colisionar con las 4 paredes.
 * 
 * Conceptos clave aplicados:
 * - Ciclo de animacion a ~60 FPS mediante glutTimerFunc.
 * - Primitiva GL_TRIANGLE_FAN para renderizar circulos suavizados.
 * - Deteccion de colisiones contra bordes tomando en cuenta el radio (r).
 * - Mapeo de color dinamico segun la rapidez (energia cinetica termica).
 * - Sistema ortografico 2D configurado con gluOrtho2D.
 * ============================================================================
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NUM_PARTICULAS 20
#define SEGMENTOS_CIRCULO 28

// Limites geometricos del contenedor en el mundo virtual
#define CONTENEDOR_X_MIN 20.0f
#define CONTENEDOR_X_MAX 180.0f
#define CONTENEDOR_Y_MIN 20.0f
#define CONTENEDOR_Y_MAX 140.0f

// Estructura que almacena el estado fisico de cada particula
typedef struct {
    float x;        // Posicion en X
    float y;        // Posicion en Y
    float vx;       // Componente horizontal de velocidad
    float vy;       // Componente vertical de velocidad
    float radio;    // Radio de la particula
    float r, g, b;  // Color RGB (asociado a la rapidez)
} Particula;

// Arreglo global de particulas y variables de control de simulacion
static Particula particulas[NUM_PARTICULAS];
static int simulacionPausada = 0;
static float factorVelocidad = 1.0f;
static unsigned long contadorRebotes = 0;

/**
 * Dibuja una cadena de texto en pantalla en la coordenada (x, y).
 */
void dibujarTexto(float x, float y, const char *texto) {
    glRasterPos2f(x, y);
    for (int i = 0; texto[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, texto[i]);
    }
}

/**
 * Actualiza el color de la particula segun su rapidez escalar (v = sqrt(vx^2 + vy^2)).
 * Simula el concepto fisico de temperatura / energia cinetica:
 * - Rapidez baja  (< 0.7): Azul / Cian (Particulas frias)
 * - Rapidez media (0.7 - 1.2): Amarillo / Ambar (Particulas templadas)
 * - Rapidez alta  (> 1.2): Rojo / Fucsia (Particulas calientes)
 */
void actualizarColorPorRapidez(Particula *p) {
    float rapidez = sqrtf(p->vx * p->vx + p->vy * p->vy);
    
    if (rapidez < 0.75f) {
        // Azul celeste / frio
        p->r = 0.20f; p->g = 0.70f; p->b = 1.00f;
    } else if (rapidez < 1.15f) {
        // Amarillo / templado
        p->r = 0.98f; p->g = 0.82f; p->b = 0.15f;
    } else {
        // Rojo carmesi / caliente
        p->r = 0.95f; p->g = 0.22f; p->b = 0.25f;
    }
}

/**
 * Genera un numero flotante aleatorio en el rango [min, max].
 */
float randomFloat(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

/**
 * Inicializa las posiciones, velocidades y radios de las particulas de gas.
 */
void inicializarParticulas(void) {
    srand((unsigned int)time(NULL));

    for (int i = 0; i < NUM_PARTICULAS; i++) {
        particulas[i].radio = 3.0f;

        // Ubicacion inicial asegurando que queden completamente dentro del contenedor
        particulas[i].x = randomFloat(CONTENEDOR_X_MIN + particulas[i].radio + 2.0f,
                                      CONTENEDOR_X_MAX - particulas[i].radio - 2.0f);
        particulas[i].y = randomFloat(CONTENEDOR_Y_MIN + particulas[i].radio + 2.0f,
                                      CONTENEDOR_Y_MAX - particulas[i].radio - 2.0f);

        // Velocidades aleatorias en ambas direcciones (-1.2 a +1.2)
        float vx = randomFloat(-1.1f, 1.1f);
        float vy = randomFloat(-1.1f, 1.1f);
        
        // Evitar particulas estaticas
        if (fabsf(vx) < 0.25f) vx = (vx < 0) ? -0.5f : 0.5f;
        if (fabsf(vy) < 0.25f) vy = (vy < 0) ? -0.5f : 0.5f;

        particulas[i].vx = vx;
        particulas[i].vy = vy;

        actualizarColorPorRapidez(&particulas[i]);
    }
    contadorRebotes = 0;
}

/**
 * Dibuja un circulo solido utilizando la primitiva GL_TRIANGLE_FAN.
 * El primer vertice es el centro; los siguientes forman los triangulos que
 * completan los 360 grados de la circunferencia.
 */
void dibujarParticulaCirculo(float cx, float cy, float radio, int segmentos) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy); // Vertice central
    for (int i = 0; i <= segmentos; i++) {
        float angulo = (float)i * (2.0f * (float)M_PI / (float)segmentos);
        float px = cx + cosf(angulo) * radio;
        float py = cy + sinf(angulo) * radio;
        glVertex2f(px, py);
    }
    glEnd();
}

/**
 * Dibuja el contenedor cerrado y los paneles informativos del laboratorio.
 */
void dibujarContenedorYPaneles(void) {
    // 1. Fondo interior del contenedor (recipiente de gas)
    glColor3f(0.07f, 0.10f, 0.16f);
    glBegin(GL_QUADS);
    glVertex2f(CONTENEDOR_X_MIN, CONTENEDOR_Y_MIN);
    glVertex2f(CONTENEDOR_X_MAX, CONTENEDOR_Y_MIN);
    glVertex2f(CONTENEDOR_X_MAX, CONTENEDOR_Y_MAX);
    glVertex2f(CONTENEDOR_X_MIN, CONTENEDOR_Y_MAX);
    glEnd();

    // 2. Paredes solidas del contenedor hermetico (borde cian neon)
    glLineWidth(3.0f);
    glColor3f(0.20f, 0.65f, 0.90f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(CONTENEDOR_X_MIN, CONTENEDOR_Y_MIN);
    glVertex2f(CONTENEDOR_X_MAX, CONTENEDOR_Y_MIN);
    glVertex2f(CONTENEDOR_X_MAX, CONTENEDOR_Y_MAX);
    glVertex2f(CONTENEDOR_X_MIN, CONTENEDOR_Y_MAX);
    glEnd();
    glLineWidth(1.0f);

    // 3. Encabezado institucional
    glColor3f(0.95f, 0.95f, 0.95f);
    dibujarTexto(20.0f, 152.0f, "UCE - COMPUTACION GRAFICA I  |  CASO 2: SIMULACION DE GAS IDEAL 2D");

    // 4. Panel de telemetria en tiempo real
    char infoBuffer[128];
    sprintf(infoBuffer, "Particulas: %d  |  Rebotes: %lu  |  Velocidad: %.1fx  |  Estado: %s",
            NUM_PARTICULAS, contadorRebotes, factorVelocidad,
            simulacionPausada ? "[PAUSADO]" : "[EN EJECUCION]");
    glColor3f(0.80f, 0.85f, 0.90f);
    dibujarTexto(20.0f, 143.0f, infoBuffer);

    // 5. Leyenda termica inferior
    glColor3f(0.20f, 0.70f, 1.00f);
    dibujarParticulaCirculo(25.0f, 10.0f, 2.5f, 16);
    glColor3f(0.75f, 0.75f, 0.75f);
    dibujarTexto(30.0f, 9.0f, "Baja rapidez (Fria)");

    glColor3f(0.98f, 0.82f, 0.15f);
    dibujarParticulaCirculo(80.0f, 10.0f, 2.5f, 16);
    glColor3f(0.75f, 0.75f, 0.75f);
    dibujarTexto(85.0f, 9.0f, "Media rapidez");

    glColor3f(0.95f, 0.22f, 0.25f);
    dibujarParticulaCirculo(130.0f, 10.0f, 2.5f, 16);
    glColor3f(0.75f, 0.75f, 0.75f);
    dibujarTexto(135.0f, 9.0f, "Alta rapidez (Caliente)");
}

/**
 * Funcion display: renderiza la escena en cada fotograma.
 */
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Dibujar el contenedor y textos
    dibujarContenedorYPaneles();

    // Dibujar cada una de las particulas como circulos usando GL_TRIANGLE_FAN
    for (int i = 0; i < NUM_PARTICULAS; i++) {
        glColor3f(particulas[i].r, particulas[i].g, particulas[i].b);
        dibujarParticulaCirculo(particulas[i].x, particulas[i].y,
                                particulas[i].radio, SEGMENTOS_CIRCULO);

        // Nucleo brillante decorativo para sensacion de esfera/atomo
        glColor3f(1.0f, 1.0f, 1.0f);
        dibujarParticulaCirculo(particulas[i].x - particulas[i].radio * 0.25f,
                                particulas[i].y + particulas[i].radio * 0.25f,
                                particulas[i].radio * 0.35f, 12);
    }

    glutSwapBuffers();
}

/**
 * Funcion timer: actualiza las posiciones fisicas y gestiona los rebotes a ~60 FPS.
 */
void actualizar(int valor) {
    (void)valor;

    if (!simulacionPausada) {
        for (int i = 0; i < NUM_PARTICULAS; i++) {
            // 1. Integracion de posicion: x = x + vx * dt
            particulas[i].x += particulas[i].vx * factorVelocidad;
            particulas[i].y += particulas[i].vy * factorVelocidad;

            // 2. Deteccion de colision con las paredes considerando el RADIO (r):
            // Pared izquierda:
            if (particulas[i].x - particulas[i].radio <= CONTENEDOR_X_MIN) {
                particulas[i].x = CONTENEDOR_X_MIN + particulas[i].radio;
                particulas[i].vx = -particulas[i].vx; // Inversion elastica
                contadorRebotes++;
            }
            // Pared derecha:
            if (particulas[i].x + particulas[i].radio >= CONTENEDOR_X_MAX) {
                particulas[i].x = CONTENEDOR_X_MAX - particulas[i].radio;
                particulas[i].vx = -particulas[i].vx;
                contadorRebotes++;
            }
            // Pared inferior:
            if (particulas[i].y - particulas[i].radio <= CONTENEDOR_Y_MIN) {
                particulas[i].y = CONTENEDOR_Y_MIN + particulas[i].radio;
                particulas[i].vy = -particulas[i].vy;
                contadorRebotes++;
            }
            // Pared superior:
            if (particulas[i].y + particulas[i].radio >= CONTENEDOR_Y_MAX) {
                particulas[i].y = CONTENEDOR_Y_MAX - particulas[i].radio;
                particulas[i].vy = -particulas[i].vy;
                contadorRebotes++;
            }

            actualizarColorPorRapidez(&particulas[i]);
        }
    }

    // Solicitar redibujado de la escena
    glutPostRedisplay();

    // Programar siguiente cuadro a 16 milisegundos (~60 FPS)
    glutTimerFunc(16, actualizar, 0);
}

/**
 * Control interactivo por teclado.
 */
void teclado(unsigned char tecla, int x, int y) {
    (void)x; (void)y;
    switch (tecla) {
        case ' ': // Pausar o reanudar
            simulacionPausada = !simulacionPausada;
            break;
        case '+': // Incrementar rapidez
        case '=':
            if (factorVelocidad < 3.0f) factorVelocidad += 0.2f;
            break;
        case '-': // Disminuir rapidez
        case '_':
            if (factorVelocidad > 0.4f) factorVelocidad -= 0.2f;
            break;
        case 'r': // Reiniciar posiciones
        case 'R':
            inicializarParticulas();
            break;
        case 27: // Tecla ESC para salir
            exit(0);
            break;
    }
}

/**
 * Ajuste del viewport y sistema de coordenadas al redimensionar la ventana.
 */
void reshape(int ancho, int alto) {
    if (alto == 0) alto = 1;
    glViewport(0, 0, ancho, alto);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Mundo virtual de 0 a 200 en X y 0 a 160 en Y
    gluOrtho2D(0.0, 200.0, 0.0, 160.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * Configuracion inicial del entorno OpenGL.
 */
void inicializarOpenGL(void) {
    // Fondo exterior gris oscuro
    glClearColor(0.03f, 0.04f, 0.06f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(850, 650);
    glutInitWindowPosition(120, 80);
    glutCreateWindow("Caso 2: Simulador de Particulas en Contenedor - Julian Vazquez 2023-0715");

    inicializarOpenGL();
    inicializarParticulas();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutTimerFunc(16, actualizar, 0);

    glutMainLoop();
    return 0;
}
