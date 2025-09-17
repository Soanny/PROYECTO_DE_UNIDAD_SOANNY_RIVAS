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

void dibujarPixel(int x, int y)
{
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

    auto dibujarPuntosCirculo = [&](int x, int y)
    {
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

void dibujarElipsePuntoMedio(Punto centro, int rx, int ry) {
    if (rx <= 0 || ry <= 0) return;

    int rx2 = rx * rx;
    int ry2 = ry * ry;
    int dosRx2 = 2 * rx2;
    int dosRy2 = 2 * ry2;

    int x = 0;
    int y = ry;
    int px = 0;
    int py = dosRx2 * y;

    int d1 = ry2 - (rx2 * ry) + (rx2 / 4);

    auto dibujarPuntosElipse = [&](int x, int y)
    {
        dibujarPixel(centro.x + x, centro.y + y);
        dibujarPixel(centro.x - x, centro.y + y);
        dibujarPixel(centro.x + x, centro.y - y);
        dibujarPixel(centro.x - x, centro.y - y);
    };

    while (px < py)
    {
        dibujarPuntosElipse(x, y);
        x++;
        px += dosRy2;
        if (d1 < 0)
        {
            d1 += ry2 + px;
        }
        else
        {
            y--;
            py -= dosRx2;
            d1 += ry2 + px - py;
        }
    }

    int d2 = ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;

    while (y >= 0)
    {
        dibujarPuntosElipse(x, y);
        y--;
        py -= dosRx2;
        if (d2 > 0)
        {
            d2 += rx2 - py;
        }
        else
        {
            x++;
            px += dosRy2;
            d2 += rx2 - py + px;
        }
    }
}

void dibujarFigura(const vector<Punto>& puntos, ModoDibujo modo) {
    if (puntos.size() < 2) return;

    switch (modo)
    {
        case LINEA_DIRECTA:
            dibujarLineaDirecta(puntos[0], puntos[1]);
            break;
        case LINEA_DDA:
            dibujarLineaDDA(puntos[0], puntos[1]);
            break;
        case CIRCULO_PUNTO_MEDIO:
            if (puntos.size() == 2)
            {
                int radio = static_cast<int>(sqrt(pow(puntos[1].x - puntos[0].x, 2) + pow(puntos[1].y - puntos[0].y, 2)));
                dibujarCirculoPuntoMedio(puntos[0], radio);
            }
            break;
        case ELIPSE_PUNTO_MEDIO:
            if (puntos.size() == 2) {
                int rx = abs(puntos[1].x - puntos[0].x);
                int ry = abs(puntos[1].y - puntos[0].y);
                dibujarElipsePuntoMedio(puntos[0], rx, ry);
            }
            break;
        default:
            break;
    }
}

void dibujarCuadricula()
{
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINES);

    for (int x = -anchoVentana/2; x <= anchoVentana/2; x += espaciadoCuadricula)
    {
        glVertex2i(x, -altoVentana/2);
        glVertex2i(x, altoVentana/2);
    }

    for (int y = -altoVentana/2; y <= altoVentana/2; y += espaciadoCuadricula)
    {
        glVertex2i(-anchoVentana/2, y);
        glVertex2i(anchoVentana/2, y);
    }

    glEnd();
}

void dibujarEjes()
{
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);

    glVertex2i(-anchoVentana/2, 0);
    glVertex2i(anchoVentana/2, 0);

    glVertex2i(0, -altoVentana/2);
    glVertex2i(0, altoVentana/2);

    glEnd();
}

void mostrar()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (cuadriculaVisible) dibujarCuadricula();
    if (ejesVisibles) dibujarEjes();

    for (size_t i = 0; i < figuras.size(); i++)
    {
        glColor3f(coloresFiguras[i].r, coloresFiguras[i].g, coloresFiguras[i].b);
        dibujarFigura(figuras[i], modosFiguras[i]);
    }

    if (!puntosActuales.empty())
    {
        glColor3f(colorActual.r, colorActual.g, colorActual.b);
        dibujarFigura(puntosActuales, modoActual);
    }
    glutSwapBuffers();
}
