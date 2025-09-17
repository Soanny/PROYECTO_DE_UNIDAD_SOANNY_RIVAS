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
