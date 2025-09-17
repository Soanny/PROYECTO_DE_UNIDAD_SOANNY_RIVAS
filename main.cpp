#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <algorithm>
using namespace std;

struct Punto
{
    int x, y;
    Punto(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Color
{
    float r, g, b;
    Color(float r = 0.0f, float g = 0.0f, float b = 0.0f) : r(r), g(g), b(b) {}
};

enum ModoDibujo
{
    NINGUNO, LINEA_DIRECTA, LINEA_DDA, CIRCULO_PUNTO_MEDIO, ELIPSE_PUNTO_MEDIO
};

int anchoVentana = 800;
int altoVentana = 600;
bool cuadriculaVisible = false;
bool ejesVisibles = true;
bool mostrarCoordenadas = false;
int espaciadoCuadricula = 20;
int grosorLinea = 1;
Color colorActual(0.0f, 0.0f, 0.0f);
ModoDibujo modoActual = NINGUNO;
vector<Punto> puntosActuales;
vector<vector<Punto>> figuras;
vector<Color> coloresFiguras;
vector<int> grosoresFiguras;
vector<ModoDibujo> modosFiguras;

void inicializar();
void mostrar();
void redimensionar(int w, int h);
void raton(int boton, int estado, int x, int y);
void teclado(unsigned char tecla, int x, int y);
void teclasEspeciales(int tecla, int x, int y);
void crearMenu();
void manejarMenu(int valor);
void dibujarCuadricula();
void dibujarEjes();
void dibujarPixel(int x, int y);
void dibujarLineaDirecta(Punto p1, Punto p2);
void dibujarLineaDDA(Punto p1, Punto p2);
void dibujarCirculoPuntoMedio(Punto centro, int radio);
void dibujarElipsePuntoMedio(Punto centro, int rx, int ry);
void dibujarFigura(const vector<Punto>& puntos, ModoDibujo modo);
void limpiarLienzo();
void exportarImagen();

void dibujarPixel(int x, int y) {
    glPointSize(grosorLinea);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void dibujarLineaDirecta(Punto p1, Punto p2)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;

    if (dx == 0)
    {
        int inicioY = min(p1.y, p2.y);
        int finY = max(p1.y, p2.y);
        for (int y = inicioY; y <= finY; y++)
        {
            dibujarPixel(p1.x, y);
        }
        return;
    }

    float m = (float)dy / dx;
    float b = p1.y - m * p1.x;

    if (abs(m) <= 1.0f)
    {
        int inicioX = min(p1.x, p2.x);
        int finX = max(p1.x, p2.x);
        for (int x = inicioX; x <= finX; x++)
        {
            int y = round(m * x + b);
            dibujarPixel(x, y);
        }
    }
    else
    {
        int inicioY = min(p1.y, p2.y);
        int finY = max(p1.y, p2.y);
        for (int y = inicioY; y <= finY; y++)
        {
            int x = round((y - b) / m);
            dibujarPixel(x, y);
        }
    }
}

void dibujarLineaDDA(Punto p1, Punto p2)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    int pasos = max(abs(dx), abs(dy));

    if (pasos == 0)
    {
        dibujarPixel(p1.x, p1.y);
        return;
    }

    float incrementoX = (float)dx / pasos;
    float incrementoY = (float)dy / pasos;

    float x = p1.x;
    float y = p1.y;

    for (int i = 0; i <= pasos; i++)
    {
        dibujarPixel(round(x), round(y));
        x += incrementoX;
        y += incrementoY;
    }

void dibujarCirculoPuntoMedio(Punto centro, int radio)
{
    int x = 0;
    int y = radio;
    int d = 1 - radio;

    auto dibujarPuntosCirculo = [&](int x, int y) {
        dibujarPixel(centro.x + x, centro.y + y);
        dibujarPixel(centro.x - x, centro.y + y);
        dibujarPixel(centro.x + x, centro.y - y);
        dibujarPixel(centro.x - x, centro.y - y);
        dibujarPixel(centro.x + y, centro.y + x);
        dibujarPixel(centro.x - y, centro.y + x);
        dibujarPixel(centro.x + y, centro.y - x);
        dibujarPixel(centro.x - y, centro.y - x);
    };

    dibujarPuntosCirculo(x, y);

    while (y > x)
    {
        x++;
        if (d < 0)
        {
            d += 2 * x + 1;
        }
        else
        {
            y--;
            d += 2 * (x - y) + 1;
        }
        dibujarPuntosCirculo(x, y);
    }
}
