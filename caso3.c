/**
 * ============================================================================
 * UNIVERSIDAD CENTRAL DEL ESTE (UCE) - RECINTO PUNTA CANA
 * ASIGNATURA: COMPUTACION GRAFICA I (ISW-411)
 * PROFESOR:   ING. RICHARD A. JIMENEZ RIJO
 * ESTUDIANTE: JULIAN VAZQUEZ (MATRICULA: 2023-0715)
 * 
 * CASO 3: DISENO E INGENIERIA - CONTROL CINEMATICO DE UN BRAZO ROBOTICO 2D
 * ============================================================================
 * Descripcion:
 * Simulador interactivo de un brazo robotico industrial articulado con dos
 * eslabones principales (brazo y antebrazo) mas efector final (pinza mecanica).
 * Se implementa la transformacion jerarquica estricta mediante la pila de 
 * matrices de OpenGL (glPushMatrix / glPopMatrix), logrando que los eslabones
 * sucesivos hereden de forma natural la traslacion y rotacion de sus antecesores.
 * 
 * Conceptos clave aplicados:
 * - Pila de matrices de modelado: glPushMatrix() y glPopMatrix().
 * - Transformaciones relativas: glRotatef() seguido de glTranslatef().
 * - Diseno modular con rectangulos (GL_QUADS) y pivotes circulares (GL_TRIANGLE_FAN).
 * - Control cinematico interactivo en tiempo real mediante teclado.
 * ============================================================================
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Dimensiones de los componentes mecanicos
#define LONGITUD_BRAZO     65.0f
#define GROSOR_BRAZO       14.0f

#define LONGITUD_ANTEBRAZO 50.0f
#define GROSOR_ANTEBRAZO   11.0f

#define LONGITUD_PINZA     18.0f
#define GROSOR_DEDO         4.0f

#define RADIO_ARTICULACION  8.0f

// Angulos de las articulaciones en grados sexagesimales
static float anguloHombro = 45.0f;  // Rango: -15 a 150 grados
static float anguloCodo   = 35.0f;  // Rango: -140 a 140 grados
static float anguloPinza  = 20.0f;  // Apertura de pinza: 5 a 45 grados

// Coordenadas de la base de montaje en el mundo virtual
static const float BASE_X = 50.0f;
static const float BASE_Y = 30.0f;

/**
 * Renderiza una cadena de texto en las coordenadas (x, y) del mundo virtual.
 */
void dibujarTexto(float x, float y, const char *texto) {
    glRasterPos2f(x, y);
    for (int i = 0; texto[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, texto[i]);
    }
}

/**
 * Dibuja un disco circular para representar los pivotes/remaches de las articulaciones.
 */
void dibujarPivote(float radio, float r, float g, float b) {
    // Disco exterior
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= 24; i++) {
        float theta = (float)i * (2.0f * (float)M_PI / 24.0f);
        glVertex2f(cosf(theta) * radio, sinf(theta) * radio);
    }
    glEnd();

    // Remache interior plateado
    glColor3f(0.85f, 0.88f, 0.92f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= 16; i++) {
        float theta = (float)i * (2.0f * (float)M_PI / 16.0f);
        glVertex2f(cosf(theta) * (radio * 0.45f), sinf(theta) * (radio * 0.45f));
    }
    glEnd();
}

/**
 * Dibuja el suelo del taller y la base mecanica fija sobre la que descansa el robot.
 */
void dibujarBaseFija(void) {
    // 1. Suelo del entorno industrial (linea horizontal y patron rayado)
    glColor3f(0.20f, 0.22f, 0.26f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(220.0f, 0.0f);
    glVertex2f(220.0f, 15.0f);
    glVertex2f(0.0f, 15.0f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.10f); // Raya de seguridad industrial
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 15.0f);
    glVertex2f(220.0f, 15.0f);
    glEnd();
    glLineWidth(1.0f);

    // 2. Pedestal trapezoidal fijo de acero
    glColor3f(0.28f, 0.32f, 0.38f);
    glBegin(GL_QUADS);
    glVertex2f(BASE_X - 25.0f, 15.0f);
    glVertex2f(BASE_X + 25.0f, 15.0f);
    glVertex2f(BASE_X + 16.0f, BASE_Y);
    glVertex2f(BASE_X - 16.0f, BASE_Y);
    glEnd();

    // 3. Torre de montaje fija
    glColor3f(0.38f, 0.42f, 0.48f);
    glBegin(GL_QUADS);
    glVertex2f(BASE_X - 12.0f, BASE_Y);
    glVertex2f(BASE_X + 12.0f, BASE_Y);
    glVertex2f(BASE_X + 12.0f, BASE_Y + 12.0f);
    glVertex2f(BASE_X - 12.0f, BASE_Y + 12.0f);
    glEnd();
}

/**
 * Dibuja un segmento mecanico rectangular centrado verticalmente a lo largo del eje X positivo.
 */
void dibujarSegmentoRectangular(float longitud, float grosor, float r, float g, float b) {
    float medioGrosor = grosor / 2.0f;

    // Cuerpo principal del eslabon
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, -medioGrosor);
    glVertex2f(longitud, -medioGrosor);
    glVertex2f(longitud, medioGrosor);
    glVertex2f(0.0f, medioGrosor);
    glEnd();

    // Borde biselado superior (iluminacion ficticia)
    glColor3f(r + 0.15f, g + 0.15f, b + 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, medioGrosor - 2.0f);
    glVertex2f(longitud, medioGrosor - 2.0f);
    glVertex2f(longitud, medioGrosor);
    glVertex2f(0.0f, medioGrosor);
    glEnd();

    // Linea decorativa central
    glColor3f(r - 0.15f, g - 0.15f, b - 0.15f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(10.0f, 0.0f);
    glVertex2f(longitud - 10.0f, 0.0f);
    glEnd();
    glLineWidth(1.0f);
}

/**
 * Dibuja la pinza o efector final mecanico (Reto opcional).
 */
void dibujarPinzaMecanica(float apertura) {
    // Bloque central de montaje de la pinza
    glColor3f(0.25f, 0.28f, 0.32f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, -6.0f);
    glVertex2f(6.0f, -6.0f);
    glVertex2f(6.0f, 6.0f);
    glVertex2f(0.0f, 6.0f);
    glEnd();

    // Dedo superior (se abre hacia arriba segun apertura)
    glPushMatrix();
    glTranslatef(6.0f, 4.0f, 0.0f);
    glRotatef(apertura, 0.0f, 0.0f, 1.0f);
    glColor3f(0.95f, 0.72f, 0.12f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, -GROSOR_DEDO / 2.0f);
    glVertex2f(LONGITUD_PINZA, -GROSOR_DEDO / 2.0f);
    glVertex2f(LONGITUD_PINZA, GROSOR_DEDO / 2.0f);
    glVertex2f(0.0f, GROSOR_DEDO / 2.0f);
    // Punta prensora
    glVertex2f(LONGITUD_PINZA - 4.0f, -GROSOR_DEDO / 2.0f);
    glVertex2f(LONGITUD_PINZA, -GROSOR_DEDO / 2.0f);
    glVertex2f(LONGITUD_PINZA, -GROSOR_DEDO / 2.0f - 4.0f);
    glVertex2f(LONGITUD_PINZA - 4.0f, -GROSOR_DEDO / 2.0f - 4.0f);
    glEnd();
    glPopMatrix();

    // Dedo inferior (se abre simetricamente hacia abajo)
    glPushMatrix();
    glTranslatef(6.0f, -4.0f, 0.0f);
    glRotatef(-apertura, 0.0f, 0.0f, 1.0f);
    glColor3f(0.95f, 0.72f, 0.12f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, -GROSOR_DEDO / 2.0f);
    glVertex2f(LONGITUD_PINZA, -GROSOR_DEDO / 2.0f);
    glVertex2f(LONGITUD_PINZA, GROSOR_DEDO / 2.0f);
    glVertex2f(0.0f, GROSOR_DEDO / 2.0f);
    // Punta prensora
    glVertex2f(LONGITUD_PINZA - 4.0f, GROSOR_DEDO / 2.0f);
    glVertex2f(LONGITUD_PINZA, GROSOR_DEDO / 2.0f);
    glVertex2f(LONGITUD_PINZA, GROSOR_DEDO / 2.0f + 4.0f);
    glVertex2f(LONGITUD_PINZA - 4.0f, GROSOR_DEDO / 2.0f + 4.0f);
    glEnd();
    glPopMatrix();
}

/**
 * Renderiza el HUD superior con datos de telemetria angular y teclas de operacion.
 */
void dibujarHUD(void) {
    glColor3f(0.95f, 0.95f, 0.95f);
    dibujarTexto(15.0f, 160.0f, "UCE - COMPUTACION GRAFICA I  |  CASO 3: BRAZO ROBOTICO ARTICULADO");

    char buffer[128];
    sprintf(buffer, "Hombro: %.1f deg  |  Codo: %.1f deg  |  Pinza: %.1f deg",
            anguloHombro, anguloCodo, anguloPinza);
    glColor3f(0.30f, 0.85f, 0.95f);
    dibujarTexto(15.0f, 151.0f, buffer);

    glColor3f(0.75f, 0.78f, 0.82f);
    dibujarTexto(15.0f, 142.0f, "Controles: [A/D] Hombro  |  [W/S] Codo  |  [O/P] Pinza  |  [R] Reiniciar");
}

/**
 * Funcion principal de dibujo: construye la jerarquia cinemática completa.
 */
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Dibujar suelo, base fija y panel de informacion
    dibujarBaseFija();
    dibujarHUD();

    // =========================================================================
    // INICIO DE LA JERARQUIA MATRICIAL (glPushMatrix / glPopMatrix)
    // =========================================================================
    glPushMatrix();

        // 1. Posicionar el origen cinemático en el punto de anclaje del hombro
        glTranslatef(BASE_X, BASE_Y + 12.0f, 0.0f);

        // 2. Rotar el conjunto completo segun el angulo del hombro
        glRotatef(anguloHombro, 0.0f, 0.0f, 1.0f);

        // 3. Dibujar el primer eslabon (Brazo) a lo largo del eje X local
        dibujarSegmentoRectangular(LONGITUD_BRAZO, GROSOR_BRAZO, 0.88f, 0.32f, 0.16f); // Naranja industrial
        dibujarPivote(RADIO_ARTICULACION, 0.22f, 0.25f, 0.30f); // Pivote del hombro

        // ---------------------------------------------------------------------
        // SEGUNDO ESLABON (Antebrazo): articulado en la PUNTA del brazo
        // ---------------------------------------------------------------------
        glPushMatrix();

            // Trasladar exactamente a la punta del primer eslabon
            glTranslatef(LONGITUD_BRAZO, 0.0f, 0.0f);

            // Rotar de forma relativa segun el angulo del codo
            glRotatef(anguloCodo, 0.0f, 0.0f, 1.0f);

            // Dibujar el segundo eslabon (Antebrazo)
            dibujarSegmentoRectangular(LONGITUD_ANTEBRAZO, GROSOR_ANTEBRAZO, 0.22f, 0.55f, 0.85f); // Azul cian
            dibujarPivote(RADIO_ARTICULACION * 0.85f, 0.22f, 0.25f, 0.30f); // Pivote del codo

            // -----------------------------------------------------------------
            // TERCER ELEMENTO: Pinza mecanica (Reto opcional)
            // -----------------------------------------------------------------
            glPushMatrix();

                // Trasladar a la punta del antebrazo
                glTranslatef(LONGITUD_ANTEBRAZO, 0.0f, 0.0f);

                // Dibujar la pinza articulada
                dibujarPinzaMecanica(anguloPinza);
                dibujarPivote(RADIO_ARTICULACION * 0.60f, 0.15f, 0.18f, 0.22f); // Pivote de muneca

            glPopMatrix(); // Fin de la pinza

        glPopMatrix(); // Fin del antebrazo

    glPopMatrix(); // Fin de la jerarquia del brazo
    // =========================================================================

    glutSwapBuffers();
}

/**
 * Control interactivo de las articulaciones mediante el teclado.
 */
void teclado(unsigned char tecla, int x, int y) {
    (void)x; (void)y;
    const float PASO = 3.0f;

    switch (tecla) {
        // Control de articulacion del hombro (Brazo)
        case 'a':
        case 'A':
            if (anguloHombro < 150.0f) anguloHombro += PASO;
            break;
        case 'd':
        case 'D':
            if (anguloHombro > -15.0f) anguloHombro -= PASO;
            break;

        // Control de articulacion del codo (Antebrazo)
        case 'w':
        case 'W':
            if (anguloCodo < 135.0f) anguloCodo += PASO;
            break;
        case 's':
        case 'S':
            if (anguloCodo > -135.0f) anguloCodo -= PASO;
            break;

        // Control de apertura y cierre de la pinza
        case 'o':
        case 'O':
            if (anguloPinza < 42.0f) anguloPinza += 2.0f;
            break;
        case 'p':
        case 'P':
            if (anguloPinza > 5.0f) anguloPinza -= 2.0f;
            break;

        // Reset de angulos
        case 'r':
        case 'R':
            anguloHombro = 45.0f;
            anguloCodo = 35.0f;
            anguloPinza = 20.0f;
            break;

        // Salir
        case 27:
            exit(0);
            break;
    }

    glutPostRedisplay();
}

/**
 * Soporte para teclas especiales de flechas como alternativa ergonomica.
 */
void teclasEspeciales(int tecla, int x, int y) {
    (void)x; (void)y;
    const float PASO = 3.0f;

    switch (tecla) {
        case GLUT_KEY_LEFT:
            if (anguloHombro < 150.0f) anguloHombro += PASO;
            break;
        case GLUT_KEY_RIGHT:
            if (anguloHombro > -15.0f) anguloHombro -= PASO;
            break;
        case GLUT_KEY_UP:
            if (anguloCodo < 135.0f) anguloCodo += PASO;
            break;
        case GLUT_KEY_DOWN:
            if (anguloCodo > -135.0f) anguloCodo -= PASO;
            break;
    }
    glutPostRedisplay();
}

/**
 * Manejo de redimensionado de la ventana y espacio ortografico 2D.
 */
void reshape(int ancho, int alto) {
    if (alto == 0) alto = 1;
    glViewport(0, 0, ancho, alto);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Mundo virtual de 0 a 220 en X y 0 a 170 en Y
    gluOrtho2D(0.0, 220.0, 0.0, 170.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * Inicializacion de parametros de OpenGL.
 */
void inicializarOpenGL(void) {
    glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(850, 650);
    glutInitWindowPosition(140, 80);
    glutCreateWindow("Caso 3: Brazo Robotico Articulado - Julian Vazquez 2023-0715");

    inicializarOpenGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(teclasEspeciales);

    glutMainLoop();
    return 0;
}
