#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

// Dimensiones de los segmentos del brazo
#define LARGO_BRAZO     65.0f
#define GROSOR_BRAZO    14.0f
#define LARGO_ANTEBRAZO 50.0f
#define GROSOR_ANTEBRAZO 10.0f

// Angulos de las articulaciones
float anguloHombro = 45.0f;
float anguloCodo = 35.0f;

// Dibuja un segmento rectangular partiendo del origen hacia el eje X
void dibujarSegmento(float largo, float grosor, float r, float g, float b) {
    float medioGrosor = grosor / 2.0f;
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, -medioGrosor);
        glVertex2f(largo, -medioGrosor);
        glVertex2f(largo, medioGrosor);
        glVertex2f(0.0f, medioGrosor);
    glEnd();
}

// Dibuja un circulo para la articulacion (pivote)
void dibujarArticulacion(float radio) {
    glColor3f(0.3f, 0.35f, 0.4f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i <= 20; i++) {
            float ang = (float)i * (2.0f * 3.14159f / 20.0f);
            glVertex2f(cosf(ang) * radio, sinf(ang) * radio);
        }
    glEnd();

    // Centro del pivote
    glColor3f(0.8f, 0.85f, 0.9f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i <= 16; i++) {
            float ang = (float)i * (2.0f * 3.14159f / 16.0f);
            glVertex2f(cosf(ang) * (radio * 0.4f), sinf(ang) * (radio * 0.4f));
        }
    glEnd();
}

// Dibuja texto en pantalla
void dibujarTexto(float x, float y, const char *texto) {
    glRasterPos2f(x, y);
    while (*texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *texto);
        texto++;
    }
}

// Dibuja la escena completa usando la jerarquia de matrices
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Suelo
    glColor3f(0.2f, 0.22f, 0.25f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(220.0f, 0.0f);
        glVertex2f(220.0f, 15.0f);
        glVertex2f(0.0f, 15.0f);
    glEnd();

    // Base fija del robot
    glColor3f(0.35f, 0.4f, 0.45f);
    glBegin(GL_QUADS);
        glVertex2f(35.0f, 15.0f);
        glVertex2f(75.0f, 15.0f);
        glVertex2f(65.0f, 40.0f);
        glVertex2f(45.0f, 40.0f);
    glEnd();

    // Textos informativos y angulos actuales
    glColor3f(0.9f, 0.9f, 0.9f);
    dibujarTexto(15.0f, 155.0f, "Control Cinematico de Brazo Robotico 2D - Caso 3");
    
    char estado[80];
    sprintf(estado, "Hombro: %.0f deg  |  Codo: %.0f deg", anguloHombro, anguloCodo);
    glColor3f(0.3f, 0.8f, 0.9f);
    dibujarTexto(15.0f, 147.0f, estado);

    glColor3f(0.7f, 0.7f, 0.7f);
    dibujarTexto(15.0f, 139.0f, "Teclas: [A / D] Mover hombro   |   [W / S] Mover codo");

    // =========================================================
    // Jerarquia con glPushMatrix y glPopMatrix
    // =========================================================
    glPushMatrix();

        // Trasladar a la base del hombro
        glTranslatef(55.0f, 40.0f, 0.0f);

        // Rotar el primer segmento (brazo)
        glRotatef(anguloHombro, 0.0f, 0.0f, 1.0f);

        // Dibujar el primer segmento y su articulacion
        dibujarSegmento(LARGO_BRAZO, GROSOR_BRAZO, 0.85f, 0.35f, 0.15f); // Naranja
        dibujarArticulacion(7.0f);

        // Sub-bloque para el segundo segmento (antebrazo)
        glPushMatrix();

            // Trasladar a la punta del primer segmento
            glTranslatef(LARGO_BRAZO, 0.0f, 0.0f);

            // Rotar el segundo segmento respecto al primero
            glRotatef(anguloCodo, 0.0f, 0.0f, 1.0f);

            // Dibujar el segundo segmento y su articulacion
            dibujarSegmento(LARGO_ANTEBRAZO, GROSOR_ANTEBRAZO, 0.2f, 0.55f, 0.85f); // Azul
            dibujarArticulacion(6.0f);

            // Pinza en la punta del antebrazo
            glPushMatrix();
                glTranslatef(LARGO_ANTEBRAZO, 0.0f, 0.0f);
                dibujarArticulacion(4.0f);

                // Dedo superior
                glColor3f(0.95f, 0.75f, 0.1f);
                glBegin(GL_LINES);
                    glVertex2f(0.0f, 0.0f);
                    glVertex2f(12.0f, 8.0f);
                    glVertex2f(12.0f, 8.0f);
                    glVertex2f(16.0f, 4.0f);
                // Dedo inferior
                    glVertex2f(0.0f, 0.0f);
                    glVertex2f(12.0f, -8.0f);
                    glVertex2f(12.0f, -8.0f);
                    glVertex2f(16.0f, -4.0f);
                glEnd();
            glPopMatrix();

        glPopMatrix(); // Fin del antebrazo

    glPopMatrix(); // Fin del brazo

    glutSwapBuffers();
}

// Control interactivo con las teclas
void teclado(unsigned char tecla, int x, int y) {
    (void)x; (void)y;
    float paso = 4.0f;

    switch (tecla) {
        // Control del hombro
        case 'a':
        case 'A':
            if (anguloHombro < 150.0f) anguloHombro += paso;
            break;
        case 'd':
        case 'D':
            if (anguloHombro > -15.0f) anguloHombro -= paso;
            break;

        // Control del codo
        case 'w':
        case 'W':
            if (anguloCodo < 140.0f) anguloCodo += paso;
            break;
        case 's':
        case 'S':
            if (anguloCodo > -140.0f) anguloCodo -= paso;
            break;
    }

    glutPostRedisplay();
}

// Configura la ventana y el sistema de coordenadas
void reshape(int ancho, int alto) {
    if (alto == 0) alto = 1;
    glViewport(0, 0, ancho, alto);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 220.0, 0.0, 170.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(150, 100);
    glutCreateWindow("Caso 3 - Brazo Robotico Articulado");

    glClearColor(0.08f, 0.09f, 0.12f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);

    glutMainLoop();
    return 0;
}
