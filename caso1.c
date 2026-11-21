#include <GL/glut.h>
#include <stdio.h>

// Datos de ocupacion semanal del hotel (lunes a domingo)
float ocupacion[7] = {65.0f, 72.0f, 80.0f, 95.0f, 88.0f, 60.0f, 55.0f};
char *dias[7] = {"LUN", "MAR", "MIE", "JUE", "VIE", "SAB", "DOM"};

// Dibuja texto en las coordenadas dadas
void dibujarTexto(float x, float y, const char *texto) {
    glRasterPos2f(x, y);
    while (*texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *texto);
        texto++;
    }
}

// Dibuja el grafico de barras, ejes y etiquetas
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Lineas guia horizontales (cada 20%)
    glColor3f(0.85f, 0.85f, 0.85f);
    glLineWidth(1.0f);
    for (int nivel = 20; nivel <= 100; nivel += 20) {
        glBegin(GL_LINES);
            glVertex2f(0.0f, (float)nivel);
            glVertex2f(72.0f, (float)nivel);
        glEnd();

        char etiqueta[10];
        sprintf(etiqueta, "%d%%", nivel);
        glColor3f(0.4f, 0.4f, 0.4f);
        dibujarTexto(-7.0f, (float)nivel - 1.0f, etiqueta);
    }
    dibujarTexto(-5.0f, -1.0f, "0%");

    // Ejes X e Y
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        // Eje Y
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.0f, 105.0f);
        // Eje X
        glVertex2f(0.0f, 0.0f);
        glVertex2f(72.0f, 0.0f);
    glEnd();

    // Dibujo de las 7 barras
    float ancho = 6.0f;
    float espacio = 4.0f;
    float xInicio = 3.0f;

    for (int i = 0; i < 7; i++) {
        float x1 = xInicio + i * (ancho + espacio);
        float x2 = x1 + ancho;
        float y = ocupacion[i];

        // Semaforo de color segun el porcentaje
        if (y < 70.0f) {
            glColor3f(0.2f, 0.75f, 0.3f); // Verde (< 70%)
        } else if (y < 90.0f) {
            glColor3f(0.95f, 0.75f, 0.1f); // Ambar (70% - 89%)
        } else {
            glColor3f(0.9f, 0.2f, 0.2f);  // Rojo (>= 90%)
        }

        // Rectangulo de la barra
        glBegin(GL_QUADS);
            glVertex2f(x1, 0.0f);
            glVertex2f(x2, 0.0f);
            glVertex2f(x2, y);
            glVertex2f(x1, y);
        glEnd();

        // Borde de la barra
        glColor3f(0.2f, 0.2f, 0.2f);
        glLineWidth(1.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(x1, 0.0f);
            glVertex2f(x2, 0.0f);
            glVertex2f(x2, y);
            glVertex2f(x1, y);
        glEnd();

        // Porcentaje sobre la barra
        char valor[10];
        sprintf(valor, "%.0f%%", y);
        glColor3f(0.1f, 0.1f, 0.1f);
        dibujarTexto(x1 + 0.8f, y + 2.0f, valor);

        // Nombre del dia debajo del eje X
        glColor3f(0.3f, 0.3f, 0.3f);
        dibujarTexto(x1 + 1.0f, -5.0f, dias[i]);
    }

    // Titulo y subtitulo
    glColor3f(0.1f, 0.2f, 0.4f);
    dibujarTexto(12.0f, 114.0f, "PANEL DE OCUPACION SEMANAL DEL HOTEL");
    glColor3f(0.4f, 0.4f, 0.4f);
    dibujarTexto(18.0f, 108.0f, "Visualizacion de Datos - Caso 1");

    // Leyenda del semaforo en la esquina superior derecha
    float xLey = 54.0f;
    float yLey = 114.0f;

    // Verde
    glColor3f(0.2f, 0.75f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(xLey, yLey);
        glVertex2f(xLey + 2.0f, yLey);
        glVertex2f(xLey + 2.0f, yLey + 2.0f);
        glVertex2f(xLey, yLey + 2.0f);
    glEnd();
    glColor3f(0.3f, 0.3f, 0.3f);
    dibujarTexto(xLey + 3.0f, yLey + 0.5f, "< 70% Baja");

    // Ambar
    yLey -= 4.0f;
    glColor3f(0.95f, 0.75f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(xLey, yLey);
        glVertex2f(xLey + 2.0f, yLey);
        glVertex2f(xLey + 2.0f, yLey + 2.0f);
        glVertex2f(xLey, yLey + 2.0f);
    glEnd();
    glColor3f(0.3f, 0.3f, 0.3f);
    dibujarTexto(xLey + 3.0f, yLey + 0.5f, "70-89% Media");

    // Rojo
    yLey -= 4.0f;
    glColor3f(0.9f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(xLey, yLey);
        glVertex2f(xLey + 2.0f, yLey);
        glVertex2f(xLey + 2.0f, yLey + 2.0f);
        glVertex2f(xLey, yLey + 2.0f);
    glEnd();
    glColor3f(0.3f, 0.3f, 0.3f);
    dibujarTexto(xLey + 3.0f, yLey + 0.5f, ">= 90% Alta");

    glFlush();
}

// Configura la ventana y el sistema de coordenadas 2D
void reshape(int ancho, int alto) {
    if (alto == 0) alto = 1;
    glViewport(0, 0, ancho, alto);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Deja margenes para que los datos queden bien centrados
    gluOrtho2D(-12.0, 76.0, -15.0, 125.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(850, 600);
    glutInitWindowPosition(150, 100);
    glutCreateWindow("Caso 1 - Panel de Ocupacion Hotelera");

    // Fondo gris claro
    glClearColor(0.95f, 0.95f, 0.96f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
