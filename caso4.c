#include <GL/glut.h>
#include <stdio.h>

// Angulo de rotacion para la animacion
float rotacionY = 0.0f;

// Posiciones de los 4 atomos satelite (geometria tetraedrica)
float satelites[4][3] = {
    {  1.0f,  1.0f,  1.0f },
    {  1.0f, -1.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f },
    { -1.0f, -1.0f,  1.0f }
};

// Dibuja la molecula 3D con Z-Buffer e iluminacion
void display(void) {
    // 1. Limpiar el buffer de color y el buffer de profundidad (Z-Buffer)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 2. Colocar la camara mirando al centro desde arriba y en angulo
    gluLookAt(0.0, 2.5, 5.5,   // Posicion de la camara
              0.0, 0.0, 0.0,   // Punto al que mira
              0.0, 1.0, 0.0);  // Vector hacia arriba

    // Rotacion de toda la molecula
    glRotatef(rotacionY, 0.0f, 1.0f, 0.0f);
    glRotatef(15.0f, 1.0f, 0.0f, 0.0f);

    // =========================================================
    // Atomo central (Carbono)
    // =========================================================
    glPushMatrix();
        GLfloat colorCarbono[] = { 0.25f, 0.25f, 0.28f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colorCarbono);
        glutSolidSphere(0.7, 24, 24); // Esfera central
    glPopMatrix();

    // =========================================================
    // Atomos satelite (Hidrogeno) y enlaces
    // =========================================================
    for (int i = 0; i < 4; i++) {
        float x = satelites[i][0] * 1.5f;
        float y = satelites[i][1] * 1.5f;
        float z = satelites[i][2] * 1.5f;

        // Enlace quimico (linea 3D que conecta el centro con el satelite)
        glDisable(GL_LIGHTING); // Desactiva luz temporalmente para ver la linea nitida
        glColor3f(0.7f, 0.7f, 0.75f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(x, y, z);
        glEnd();
        glEnable(GL_LIGHTING);

        // Cada satelite usa glPushMatrix/glPopMatrix para no afectar a los demas
        glPushMatrix();
            glTranslatef(x, y, z);
            GLfloat colorHidrogeno[] = { 0.9f, 0.95f, 1.0f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colorHidrogeno);
            glutSolidSphere(0.35, 20, 20); // Esfera satelite
        glPopMatrix();
    }

    glutSwapBuffers();
}

// Timer para rotar la molecula continuamente a ~60 FPS
void timer(int v) {
    (void)v;
    rotacionY += 0.8f;
    if (rotacionY >= 360.0f) {
        rotacionY -= 360.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// Configura la proyeccion en perspectiva
void reshape(int ancho, int alto) {
    if (alto == 0) alto = 1;
    glViewport(0, 0, ancho, alto);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Proyeccion en perspectiva con profundidad real (FOV = 45 grados)
    gluPerspective(45.0, (float)ancho / (float)alto, 1.0, 50.0);

    glMatrixMode(GL_MODELVIEW);
}

// Configuracion de luces y Z-Buffer
void inicializar(void) {
    glClearColor(0.05f, 0.07f, 0.12f, 1.0f);

    // Activar Z-Buffer para profundidad y ocultar caras traseras
    glEnable(GL_DEPTH_TEST);

    // Activar iluminacion
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Posicion y color de la luz
    GLfloat posLuz[] = { 3.0f, 5.0f, 4.0f, 1.0f };
    GLfloat luzBlanca[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, posLuz);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzBlanca);

    // Brillo para que las esferas se vean con volumen
    GLfloat especular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    // Modo con doble buffer, color RGB y buffer de profundidad (Z-Buffer)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(150, 100);
    glutCreateWindow("Caso 4 - Modelo Molecular 3D");

    inicializar();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
