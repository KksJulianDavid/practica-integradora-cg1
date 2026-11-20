/**
 * ============================================================================
 * UNIVERSIDAD CENTRAL DEL ESTE (UCE) - RECINTO PUNTA CANA
 * ASIGNATURA: COMPUTACION GRAFICA I (ISW-411)
 * PROFESOR:   ING. RICHARD A. JIMENEZ RIJO
 * ESTUDIANTE: JULIAN VAZQUEZ (MATRICULA: 2023-0715)
 * 
 * CASO 4: VISUALIZACION CIENTIFICA 3D - MODELO MOLECULAR (METANO CH4)
 * ============================================================================
 * Descripcion:
 * Visor interactivo en 3D de una molecula de Metano (CH4), compuesta por un
 * atomo central de Carbono (esfera mayor) y cuatro atomos satelite de Hidrogeno
 * (esferas menores) orientados en una geometria tetraedrica perfecta (109.5 deg).
 * Cada enlace covalente se visualiza mediante cilindros solidos 3D.
 * 
 * Conceptos clave aplicados:
 * - Proyeccion en perspectiva real mediante gluPerspective().
 * - Camara sintetica 3D posicionada con gluLookAt().
 * - Z-Buffer (Depth Buffer) para correcta oclusion y eliminacion de superficies ocultas:
 *     1) GLUT_DEPTH en glutInitDisplayMode().
 *     2) glEnable(GL_DEPTH_TEST) en inicializacion.
 *     3) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) en cada cuadro.
 * - Solidos esfericos parametricos mediante glutSolidSphere().
 * - Pila de transformaciones 3D (glPushMatrix / glPopMatrix) para atomos satelite.
 * - Modelo de iluminacion y sombreado Phong/Gouraud (GL_LIGHTING, GL_LIGHT0).
 * - Rotacion continua suave animada mediante glutTimerFunc().
 * ============================================================================
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Parametros geometricos de la molecula
#define RADIO_CARBONO    0.70f
#define RADIO_HIDROGENO  0.35f
#define RADIO_ENLACE     0.08f
#define LONGITUD_ENLACE  1.75f

// Angulos de rotacion interactiva de la escena 3D
static float rotacionY = 0.0f;
static float rotacionX = 20.0f;
static int animacionActiva = 1;

// Variables para control interactivo con el raton
static int mousePresionado = 0;
static int mouseXAnt = 0, mouseYAnt = 0;

// Posiciones normalizadas de los 4 vertices del tetraedro regular (CH4)
// En un tetraedro:
// V0 = (+1, +1, +1) / sqrt(3)
// V1 = (+1, -1, -1) / sqrt(3)
// V2 = (-1, +1, -1) / sqrt(3)
// V3 = (-1, -1, +1) / sqrt(3)
static const float coordTetraedro[4][3] = {
    {  0.57735f,  0.57735f,  0.57735f },
    {  0.57735f, -0.57735f, -0.57735f },
    { -0.57735f,  0.57735f, -0.57735f },
    { -0.57735f, -0.57735f,  0.57735f }
};

/**
 * Renderiza texto 2D superpuesto en pantalla utilizando una proyeccion ortogonal temporal.
 */
void dibujarTexto2D(float x, float y, const char *texto) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 850, 0, 650);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glRasterPos2f(x, y);
    for (int i = 0; texto[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, texto[i]);
    }
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Dibuja un cilindro 3D que conecta el origen (0,0,0) con el punto objetivo (tx, ty, tz).
 * Representa el enlace quimico covalente entre dos atomos.
 */
void dibujarEnlace(float tx, float ty, float tz, float radio, float longitud) {
    glPushMatrix();

    // Calcular vector de orientacion y angulos esfericos
    float distancia = sqrtf(tx * tx + ty * ty + tz * tz);
    if (distancia > 0.0001f) {
        float ax = -ty;
        float ay = tx;
        float az = 0.0f;
        float normaEje = sqrtf(ax * ax + ay * ay);
        float anguloRot = acosf(tz / distancia) * (180.0f / (float)M_PI);

        if (normaEje > 0.0001f) {
            glRotatef(anguloRot, ax / normaEje, ay / normaEje, az);
        } else if (tz < 0.0f) {
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        }
    }

    // Material gris titanio para los enlaces
    GLfloat matDifusaEnlace[] = { 0.60f, 0.65f, 0.70f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDifusaEnlace);

    GLUquadric *quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluCylinder(quad, radio, radio, longitud, 16, 4);
    gluDeleteQuadric(quad);

    glPopMatrix();
}

/**
 * Renderiza la escena molecular completa en el espacio tridimensional.
 */
void display(void) {
    // 1. Limpieza simultanea del bufer de color y del bufer de profundidad (Z-Buffer)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 2. Colocar la camara sintetica con gluLookAt mirando al origen desde un angulo elevado
    gluLookAt(0.0, 2.5, 6.2,   // Posicion de la camara (eyeX, eyeY, eyeZ)
              0.0, 0.0, 0.0,   // Punto al que mira la camara (centro de la molecula)
              0.0, 1.0, 0.0);  // Vector 'Up' (hacia arriba en Y)

    // 3. Aplicar rotaciones de usuario y animacion continua
    glRotatef(rotacionX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotacionY, 0.0f, 1.0f, 0.0f);

    // =========================================================================
    // DIBUJO DEL ATOMO CENTRAL: CARBONO (C)
    // =========================================================================
    glPushMatrix();
        // Material gris grafito oscuro con brillo especular metalico
        GLfloat matAmbDifCarbono[] = { 0.20f, 0.22f, 0.25f, 1.0f };
        GLfloat matEspecular[]     = { 0.90f, 0.90f, 0.90f, 1.0f };
        GLfloat brillo[]           = { 64.0f };

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDifCarbono);
        glMaterialfv(GL_FRONT, GL_SPECULAR, matEspecular);
        glMaterialfv(GL_FRONT, GL_SHININESS, brillo);

        // Esfera solida central con 32 paralelos y 32 meridianos
        glutSolidSphere(RADIO_CARBONO, 32, 32);
    glPopMatrix();

    // =========================================================================
    // DIBUJO DE LOS 4 ATOMOS SATELITE: HIDROGENO (H) Y ENLACES
    // =========================================================================
    for (int i = 0; i < 4; i++) {
        float hx = coordTetraedro[i][0] * LONGITUD_ENLACE;
        float hy = coordTetraedro[i][1] * LONGITUD_ENLACE;
        float hz = coordTetraedro[i][2] * LONGITUD_ENLACE;

        // Dibujar el enlace quimico desde el Carbono hasta este Hidrogeno
        dibujarEnlace(hx, hy, hz, RADIO_ENLACE, LONGITUD_ENLACE);

        // Posicionar cada satelite mediante glPushMatrix / glPopMatrix
        glPushMatrix();
            // Trasladar al vertice correspondiente del tetraedro
            glTranslatef(hx, hy, hz);

            // Material blanco puro con ligero matiz azul hielo
            GLfloat matAmbDifHidrogeno[] = { 0.88f, 0.94f, 1.00f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDifHidrogeno);
            glMaterialfv(GL_FRONT, GL_SPECULAR, matEspecular);
            glMaterialfv(GL_FRONT, GL_SHININESS, brillo);

            // Esfera de Hidrogeno
            glutSolidSphere(RADIO_HIDROGENO, 24, 24);
        glPopMatrix();
    }

    // =========================================================================
    // TEXTO SUPERPUESTO (HUD)
    // =========================================================================
    glColor3f(0.95f, 0.95f, 0.95f);
    dibujarTexto2D(20.0f, 620.0f, "UCE - COMPUTACION GRAFICA I  |  CASO 4: MODELO MOLECULAR 3D (METANO - CH4)");
    
    glColor3f(0.35f, 0.85f, 0.95f);
    dibujarTexto2D(20.0f, 600.0f, "Geometria: Tetraedrica (109.5 deg)  |  Z-Buffer: ACTIVADO  |  Sombreado: Phong/Gouraud");

    glColor3f(0.75f, 0.78f, 0.82f);
    dibujarTexto2D(20.0f, 580.0f, "Controles: [ESPACIO] Pausar rotacion  |  [Arrastre con Raton / Flechas] Rotar en 3D  |  [R] Reiniciar");

    glutSwapBuffers();
}

/**
 * Funcion timer para la rotacion suave continua de la molecula a ~60 FPS.
 */
void timerAnimacion(int valor) {
    (void)valor;
    if (animacionActiva) {
        rotacionY += 0.8f;
        if (rotacionY >= 360.0f) rotacionY -= 360.0f;
        glutPostRedisplay();
    }
    glutTimerFunc(16, timerAnimacion, 0);
}

/**
 * Control por teclado convencional.
 */
void teclado(unsigned char tecla, int x, int y) {
    (void)x; (void)y;
    switch (tecla) {
        case ' ': // Pausar o reanudar rotacion automatica
            animacionActiva = !animacionActiva;
            break;
        case 'r':
        case 'R':
            rotacionX = 20.0f;
            rotacionY = 0.0f;
            break;
        case 27: // Salir con tecla ESC
            exit(0);
            break;
    }
    glutPostRedisplay();
}

/**
 * Control por teclas de flechas para rotar manualmente la camara / modelo.
 */
void teclasEspeciales(int tecla, int x, int y) {
    (void)x; (void)y;
    switch (tecla) {
        case GLUT_KEY_LEFT:
            rotacionY -= 4.0f;
            break;
        case GLUT_KEY_RIGHT:
            rotacionY += 4.0f;
            break;
        case GLUT_KEY_UP:
            rotacionX -= 4.0f;
            break;
        case GLUT_KEY_DOWN:
            rotacionX += 4.0f;
            break;
    }
    glutPostRedisplay();
}

/**
 * Captura de clic del raton para rotacion orbital interactiva.
 */
void ratonBoton(int boton, int estado, int x, int y) {
    if (boton == GLUT_LEFT_BUTTON) {
        if (estado == GLUT_DOWN) {
            mousePresionado = 1;
            mouseXAnt = x;
            mouseYAnt = y;
        } else {
            mousePresionado = 0;
        }
    }
}

/**
 * Captura de movimiento con el raton mientras se mantiene pulsado el clic izquierdo.
 */
void ratonMovimiento(int x, int y) {
    if (mousePresionado) {
        int dx = x - mouseXAnt;
        int dy = y - mouseYAnt;

        rotacionY += (float)dx * 0.5f;
        rotacionX += (float)dy * 0.5f;

        mouseXAnt = x;
        mouseYAnt = y;

        glutPostRedisplay();
    }
}

/**
 * Configuracion de la proyeccion en PERSPECTIVA al ajustar la ventana.
 */
void reshape(int ancho, int alto) {
    if (alto == 0) alto = 1;
    glViewport(0, 0, ancho, alto);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Proyeccion en perspectiva con angulo de vision de 45 grados y ratio de aspecto
    float aspecto = (float)ancho / (float)alto;
    gluPerspective(45.0, aspecto, 1.0, 50.0);

    glMatrixMode(GL_MODELVIEW);
}

/**
 * Inicializacion de las caracteristicas graficas de OpenGL (Z-Buffer, Luces y Materiales).
 */
void inicializarOpenGL(void) {
    // Fondo azul marino espacial muy oscuro
    glClearColor(0.04f, 0.06f, 0.10f, 1.0f);

    // 1. ACTIVACION DEL Z-BUFFER (Depth Test)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // 2. ACTIVACION DE ILUMINACION Y FUENTE DE LUZ
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Posicion de la luz: elevada y lateral frontal (luz direccional/puntual)
    GLfloat posLuz0[]       = { 4.0f, 6.0f, 5.0f, 1.0f };
    GLfloat luzAmbiente[]   = { 0.25f, 0.25f, 0.28f, 1.0f };
    GLfloat luzDifusa[]     = { 0.90f, 0.90f, 0.95f, 1.0f };
    GLfloat luzEspecular[]  = { 1.00f, 1.00f, 1.00f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, posLuz0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);

    // Normalizacion automatica de normales tras transformaciones
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    // Declaracion obligatoria de GLUT_DEPTH para el bufer de profundidad
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(850, 650);
    glutInitWindowPosition(160, 80);
    glutCreateWindow("Caso 4: Modelo Molecular 3D (CH4) - Julian Vazquez 2023-0715");

    inicializarOpenGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(teclasEspeciales);
    glutMouseFunc(ratonBoton);
    glutMotionFunc(ratonMovimiento);
    glutTimerFunc(16, timerAnimacion, 0);

    glutMainLoop();
    return 0;
}
