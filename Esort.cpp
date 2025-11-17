#include "Estructuras.h"
#include "FuenteSerial.h"
#include "FuenteArchivo.h"
#include <vector>

const int CAPACIDAD_BUFFER = 4;
std::vector<std::string> nombresChunks; 

void faseAdquisicion(FuenteDatos* fuente) {
    std::cout << "\nIniciando Fase 1: Adquisición de datos...\n";
    BufferCircular buffer(CAPACIDAD_BUFFER);
    int contadorChunks = 0;

    while (fuente->tieneMasDatos()) {
        int dato = fuente->obtenerSiguiente();
        
        if (dato == -1 && !fuente->tieneMasDatos()) {
            break;
        }

        std::cout << "Leyendo -> " << dato << "\n";
        buffer.insertar(dato);

        if (buffer.estaLleno()) {
            std::cout << "Buffer lleno. Ordenando internamente...\n";
            
            char nombreChunk[32];
            std::sprintf(nombreChunk, "trozo_%d.tmp", contadorChunks);
            
            buffer.volcarAArchivo(nombreChunk);
            nombresChunks.push_back(std::string(nombreChunk));
            
            contadorChunks++;
        }
    }

    if (buffer.obtenerTamanoActual() > 0) {
        std::cout << "Fin de lectura. Volcando sobrante (" << buffer.obtenerTamanoActual() << " elementos)...\n";
        char nombreChunk[32];
        std::sprintf(nombreChunk, "trozo_%d.tmp", contadorChunks);
        buffer.volcarAArchivo(nombreChunk);
        nombresChunks.push_back(std::string(nombreChunk));
    }

    std::cout << "Fase 1 completada. " << nombresChunks.size() << " trozos generados.\n";
}

void faseFusionExterna() {
    std::cout << "\nIniciando Fase 2: Fusión Externa (K-Way Merge)\n";
    size_t k = nombresChunks.size();

    if (k == 0) {
        std::cout << "No hay trozos para fusionar. Finalizando.\n";
        return;
    }

    FuenteDatos** fuentes = new FuenteDatos*[k];
    
    std::fstream archivoSalida("salida.ordenada.txt", std::ios::out);
    if (!archivoSalida.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de salida.\n";
        delete[] fuentes;
        return;
    }

    std::cout << "Abriendo " << k << " archivos fuente...\n";
    std::cout << "K=" << k << ". Fusión en progreso...\n";

    for (size_t i = 0; i < k; ++i) {
        fuentes[i] = new FuenteArchivo(nombresChunks[i].c_str()); 
    }

    int totalElementosEscritos = 0;
    
    while (true) {
        int indiceMinimo = -1;
        int valorMinimo = 2147483647;
        int fuentesActivas = 0;

        for (size_t i = 0; i < k; ++i) {
            FuenteArchivo* archivoActual = dynamic_cast<FuenteArchivo*>(fuentes[i]);
            
            if (archivoActual && archivoActual->tieneMasDatos()) {
                fuentesActivas++;
                int valorActual = archivoActual->obtenerValorActual();
                
                if (valorActual != -1 && valorActual < valorMinimo) {
                    valorMinimo = valorActual;
                    indiceMinimo = i;
                }
            }
        }
        
        if (fuentesActivas == 0) {
            break;
        }

        if (indiceMinimo != -1) {
            archivoSalida << valorMinimo << "\n";
            totalElementosEscritos++;

            fuentes[indiceMinimo]->obtenerSiguiente();
            std::cout << " - Min(trozo_" << indiceMinimo << ") -> " << valorMinimo << ". Escribiendo " << valorMinimo << ".\n";
        }
    }

    for (size_t i = 0; i < k; ++i) {
        delete fuentes[i]; 
    }
    delete[] fuentes; 

    archivoSalida.close();
    std::cout << "\nFusión completada. " << totalElementosEscritos << " elementos escritos. Archivo final: salida.ordenada.txt\n";
    std::cout << "Liberando memoria... Sistema apagado.\n";
}

int main() {
    std::cout << "Iniciando Sistema de Ordenamiento Externo E-Sort...\n";
    
    FuenteDatos* fuenteEntrada = new FuenteSerial("COM3", 15); 

    faseAdquisicion(fuenteEntrada);

    faseFusionExterna();

    delete fuenteEntrada;

    return 0;
}