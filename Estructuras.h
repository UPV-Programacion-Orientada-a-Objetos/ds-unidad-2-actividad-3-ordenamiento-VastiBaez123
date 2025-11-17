#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>

class FuenteDatos {
public:
    virtual int obtenerSiguiente() = 0;
    virtual bool tieneMasDatos() = 0;
    virtual ~FuenteDatos() {} 
};

struct NodoLectura {
    int valorEnergia;
    NodoLectura* siguiente;
    NodoLectura* anterior;
};

class BufferCircular {
private:
    NodoLectura* cabeza;
    int capacidadMaxima;
    int tamanoActual;

    void ordenarInternamente() {
        if (tamanoActual <= 1) return;

        NodoLectura* actual = cabeza->siguiente;
        for (int i = 1; i < tamanoActual; ++i) {
            NodoLectura* temp = actual;
            while (temp != cabeza && temp->anterior->valorEnergia > temp->valorEnergia) {
                 int tempValor = temp->valorEnergia;
                 temp->valorEnergia = temp->anterior->valorEnergia;
                 temp->anterior->valorEnergia = tempValor;
                 temp = temp->anterior;
            }
            actual = actual->siguiente;
        }
    }

public:
    BufferCircular(int capacidad) : capacidadMaxima(capacidad), tamanoActual(0), cabeza(nullptr) {}

    ~BufferCircular() {
        if (!cabeza) return;

        NodoLectura* actual = cabeza;
        do {
            NodoLectura* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        } while (actual != cabeza);

        cabeza = nullptr;
    }

    void insertar(int valor) {
        NodoLectura* nuevoNodo = new NodoLectura{valor, nullptr, nullptr};
        
        if (cabeza == nullptr) {
            cabeza = nuevoNodo;
            cabeza->siguiente = cabeza;
            cabeza->anterior = cabeza;
        } else {
            NodoLectura* cola = cabeza->anterior;
            cola->siguiente = nuevoNodo;
            nuevoNodo->anterior = cola;
            nuevoNodo->siguiente = cabeza;
            cabeza->anterior = nuevoNodo;
        }
        tamanoActual++;
    }
    
    bool estaLleno() const {
        return tamanoActual >= capacidadMaxima;
    }

    void volcarAArchivo(const char* nombreArchivo) {
        ordenarInternamente(); 
        
        std::fstream archivoSalida(nombreArchivo, std::ios::out);
        if (!archivoSalida.is_open()) {
            std::cerr << "Error: No se pudo abrir " << nombreArchivo << " para escritura.\n";
            return;
        }

        std::cout << "Buffer ordenado: [";
        NodoLectura* actual = cabeza;
        for (int i = 0; i < tamanoActual; ++i) {
            archivoSalida << actual->valorEnergia << "\n";
            std::cout << actual->valorEnergia << (i == tamanoActual - 1 ? "" : ", ");
            actual = actual->siguiente;
        }
        std::cout << "]\n";
        
        archivoSalida.close();
        limpiarBuffer();
        std::cout << "Escribiendo " << nombreArchivo << "... OK.\n";
    }
    
    void limpiarBuffer() {
        if (!cabeza) return;
        
        NodoLectura* actual = cabeza;
        do {
            NodoLectura* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        } while (actual != cabeza);
        
        cabeza = nullptr;
        tamanoActual = 0;
        std::cout << "Buffer limpiado.\n";
    }
    
    int obtenerTamanoActual() const { return tamanoActual; }
};

#endif // ESTRUCTURAS_H