#ifndef FUENTEARCHIVO_H
#define FUENTEARCHIVO_H

#include "Estructuras.h"

class FuenteArchivo : public FuenteDatos {
private:
    std::fstream archivoEntrada;
    int ultimoValor;
    bool datosDisponibles;

public:
    FuenteArchivo(const char* nombre) : ultimoValor(-1), datosDisponibles(true) {
        archivoEntrada.open(nombre, std::ios::in);
        if (!archivoEntrada.is_open()) {
            std::cerr << "Error: No se pudo abrir el chunk de entrada: " << nombre << "\n";
            datosDisponibles = false;
        }
        if (datosDisponibles) {
            obtenerSiguiente(); 
        }
    }

    ~FuenteArchivo() override {
        if (archivoEntrada.is_open()) {
            archivoEntrada.close();
        }
    }

    int obtenerSiguiente() override {
        if (!datosDisponibles) return -1;
        
        int valorRetorno = ultimoValor;
        
        if (archivoEntrada >> ultimoValor) {
            datosDisponibles = true;
        } else {
            datosDisponibles = false;
        }

        return valorRetorno;
    }

    bool tieneMasDatos() override {
        return datosDisponibles;
    }
    
    int obtenerValorActual() const {
        return ultimoValor;
    }
};

#endif // FUENTEARCHIVO_H