#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUM_PARTICULAS 20

// Limites de la caja contenedora
#define MIN_X 20.0f
#define MAX_X 180.0f
#define MIN_Y 20.0f
#define MAX_Y 140.0f

// Estructura para cada particula de gas
typedef struct {
    float x, y;     // Posicion
    float vx, vy;   // Velocidad
    float radio;    // Radio
    float r, g, b;  // Color
} Particula;

Particula particulas[NUM_PARTICULAS];

// Genera un numero flotante aleatorio entre min y max
float numAleatorio(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

// Asigna color a la particula segun su rapidez (azul=lenta, roja=rapida)
void actualizarColor(Particula *p) {
    float rapidez = sqrtf(p->vx * p->vx + p->vy * p->vy);
    if (rapidez < 0.75f) {
        p->r = 0.2f; p->g = 0.7f; p->b = 1.0f; // Azul/Cian
    } else if (rapidez < 1.15f) {
        p->r = 0.95f; p->g = 0.8f; p->b = 0.2f; // Amarillo
    } else {
        p->r = 0.95f; p->g = 0.2f; p->b = 0.2f; // Rojo
    }
}

// Inicializa las particulas con datos aleatorios
void inicializar(void) {
    srand((unsigned int)time(NULL));

    for (int i = 0; i < NUM_PARTICULAS; i++) {
        particulas[i].radio = 3.0f;
        // Posicion dentro de los limites
        particulas[i].x = numAleatorio(MIN_X + 5.0f, MAX_X - 5.0f);
        particulas[i].y = numAleatorio(MIN_Y + 5.0f, MAX_Y - 5.0f);

        // Velocidad aleatoria (evitando que sea 0)
        particulas[i].vx = numAleatorio(-1.0f, 1.0f);
        particulas[i].vy = numAleatorio(-1.0f, 1.0f);
        if (fabsf(particulas[i].vx) < 0.2f) particulas[i].vx = 0.5f;
        if (fabsf(particulas[i].vy) < 0.2f) particulas[i].vy = 0.5f;

        actualizarColor(&particulas[i]);
    }
}

// Dibuja un circulo usando abanico de triangulos
void dibujarCirculo(float cx, float cy, float radio) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy); // Centro
        for (int i = 0; i <= 20; i++) {
            float angulo = (float)i * (2.0f * 3.14159f / 20.0f);
            glVertex2f(cx + cosf(angulo) * radio, cy + sinf(angulo) * radio);
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

// Dibuja la escena: contenedor y particulas
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Fondo del contenedor
    glColor3f(0.08f, 0.1f, 0.15f);
    glBegin(GL_QUADS);
        glVertex2f(MIN_X, MIN_Y);
        glVertex2f(MAX_X, MIN_Y);
        glVertex2f(MAX_X, MAX_Y);
        glVertex2f(MIN_X, MAX_Y);
    glEnd();

    // Paredes del contenedor
    glColor3f(0.3f, 0.7f, 0.9f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(MIN_X, MIN_Y);
        glVertex2f(MAX_X, MIN_Y);
        glVertex2f(MAX_X, MAX_Y);
        glVertex2f(MIN_X, MAX_Y);
    glEnd();

    // Dibujar cada particula
    for (int i = 0; i < NUM_PARTICULAS; i++) {
        glColor3f(particulas[i].r, particulas[i].g, particulas[i].b);
        dibujarCirculo(particulas[i].x, particulas[i].y, particulas[i].radio);
    }

    // Titulo e informacion
    glColor3f(0.9f, 0.9f, 0.9f);
    dibujarTexto(MIN_X, 150.0f, "Simulacion de Gas en Contenedor 2D - Caso 2");
    glColor3f(0.6f, 0.7f, 0.8f);
    dibujarTexto(MIN_X, 143.0f, "Particulas: 20  |  Animacion continua con rebotes");

    glutSwapBuffers();
}

// Actualiza las posiciones y calcula los rebotes con las paredes
void timer(int v) {
    (void)v;

    for (int i = 0; i < NUM_PARTICULAS; i++) {
        // Mueve la particula
        particulas[i].x += particulas[i].vx;
        particulas[i].y += particulas[i].vy;

        // Rebote en pared izquierda
        if (particulas[i].x - particulas[i].radio <= MIN_X) {
            particulas[i].x = MIN_X + particulas[i].radio;
            particulas[i].vx = -particulas[i].vx;
        }
        // Rebote en pared derecha
        if (particulas[i].x + particulas[i].radio >= MAX_X) {
            particulas[i].x = MAX_X - particulas[i].radio;
            particulas[i].vx = -particulas[i].vx;
        }
        // Rebote en pared inferior
        if (particulas[i].y - particulas[i].radio <= MIN_Y) {
            particulas[i].y = MIN_Y + particulas[i].radio;
            particulas[i].vy = -particulas[i].vy;
        }
        // Rebote en pared superior
        if (particulas[i].y + particulas[i].radio >= MAX_Y) {
            particulas[i].y = MAX_Y - particulas[i].radio;
            particulas[i].vy = -particulas[i].vy;
        }

        actualizarColor(&particulas[i]);
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // Vuelve a llamar la funcion cada 16ms (~60 FPS)
}

// Configura el sistema de coordenadas de la ventana
void reshape(int ancho, int alto) {
    if (alto == 0) alto = 1;
    glViewport(0, 0, ancho, alto);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 200.0, 0.0, 160.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(150, 100);
    glutCreateWindow("Caso 2 - Particulas de Gas en Contenedor");

    glClearColor(0.04f, 0.05f, 0.08f, 1.0f);

    inicializar();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
