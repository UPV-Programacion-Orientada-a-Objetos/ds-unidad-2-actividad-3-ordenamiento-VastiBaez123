#ifndef FUENTESERIAL_H
#define FUENTESERIAL_H

#include "Estructuras.h" 

class FuenteSerial : public FuenteDatos {
private:
    FILE* puertoSimulado;
    int contadorLecturas;
    int maxLecturas;

public:
    FuenteSerial(const char* nombrePuerto, int limite) : contadorLecturas(0), maxLecturas(limite) {
        
        puertoSimulado = fopen("datos_simulados.txt", "r"); 
        if (!puertoSimulado) {
            puertoSimulado = fopen("datos_simulados.txt", "w+");
            if (puertoSimulado) {
                fprintf(puertoSimulado, "105\n5\n210\n99\n1\n500\n20\n15\n100\n3\n300\n7\n50\n25\n10\n1\n500\n20\n15\n100\n3\n300\n7\n50\n25\n10\n");
                fseek(puertoSimulado, 0, SEEK_SET);
            }
        }

        std::cout << "Conectando a puerto " << nombrePuerto << " (Simulado)... Conectado.\n";
    }

    ~FuenteSerial() override {
        if (puertoSimulado) {
            fclose(puertoSimulado);
        }
        std::cout << "Puerto serial cerrado.\n";
    }

    int obtenerSiguiente() override {
        if (contadorLecturas >= maxLecturas || !puertoSimulado) return -1;
        
        char buffer[16];
        if (fgets(buffer, 16, puertoSimulado) != NULL) {
            contadorLecturas++;
            return std::atoi(buffer);
        }
        return -1;
    }

    bool tieneMasDatos() override {
        return contadorLecturas < maxLecturas && !feof(puertoSimulado);
    }
};

#endif // FUENTESERIAL_H