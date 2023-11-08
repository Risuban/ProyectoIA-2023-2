#include <iostream>
#include <vector>
#include <fstream>  // Necesario para leer archivos
#include <sstream>  // Necesario para dividir strings
#include <string>
#include <climits> 

using namespace std;

struct Turista {
    int tiempo_total_disponible;
    std::vector<int> valorizaciones_nodos;
    std::vector<std::vector<int>> valorizaciones_arcos;
    int ganancia; // Campo nuevo para almacenar la ganancia
};

std::vector<int> tiempos_nodos_globales;
std::vector<std::vector<int>> tiempos_arcos_globales;
std::vector<int> valorizaciones_nodos_globales;
std::vector<std::vector<int>> valorizaciones_arcos_globales;
int tiempo_maximo_global;

std::vector<Turista> turistas;




void leer_archivo_tipo_1(const std::string& nombre_archivo) {
    std::ifstream archivo(nombre_archivo);
    
    if (!archivo) {
        std::cerr << "No se puede abrir el archivo: " << nombre_archivo << std::endl;
        return;
    }
    
    int numero_de_nodos;
    archivo >> numero_de_nodos;

    tiempos_nodos_globales.resize(numero_de_nodos);
    for (int& tiempo : tiempos_nodos_globales) {
        archivo >> tiempo;
    }

    tiempos_arcos_globales.resize(numero_de_nodos, std::vector<int>(numero_de_nodos));
    for (int i = 0; i < numero_de_nodos; ++i) {
        for (int j = 0; j < numero_de_nodos; ++j) {
            archivo >> tiempos_arcos_globales[i][j];
        }
    }
    
    archivo.close();
}



void leer_archivo_tipo_2(const std::string& nombre_archivo) {
    std::ifstream archivo(nombre_archivo);

    if (!archivo) {
        std::cerr << "No se puede abrir el archivo: " << nombre_archivo << std::endl;
        return;
    }

    int numero_de_turistas;
    archivo >> numero_de_turistas;
    turistas.resize(numero_de_turistas);

    for (Turista& turista : turistas) {
        archivo >> turista.tiempo_total_disponible;

        turista.valorizaciones_nodos.resize(tiempos_nodos_globales.size());
        for (int& valorizacion : turista.valorizaciones_nodos) {
            archivo >> valorizacion;
        }

        turista.valorizaciones_arcos.resize(tiempos_nodos_globales.size(), std::vector<int>(tiempos_nodos_globales.size()));
        for (int i = 0; i < tiempos_nodos_globales.size(); ++i) {
            for (int j = 0; j < tiempos_nodos_globales.size(); ++j) {
                archivo >> turista.valorizaciones_arcos[i][j];
            }
        }
    }

    archivo.close();
}


int evaluar_ganancia(const std::vector<int>& ruta, const Turista& turista) {
    int ganancia_total = 0;
    int tiempo_usado = 0;
    
    for (size_t i = 0; i < ruta.size(); ++i) {
        if (i > 0) {
            int tiempo_traslado = tiempos_arcos_globales[ruta[i - 1]][ruta[i]];
            tiempo_usado += tiempo_traslado;
            ganancia_total += turista.valorizaciones_arcos[ruta[i - 1]][ruta[i]];

            // Imprimir tiempo y ganancia de traslado para depuración
            std::cout << "Traslado de " << ruta[i - 1] << " a " << ruta[i] << ": "
                      << "Tiempo usado: " << tiempo_traslado << ", Tiempo total: " << tiempo_usado << std::endl;
        }
        
        int tiempo_estancia = tiempos_nodos_globales[ruta[i]];
        tiempo_usado += tiempo_estancia;
        ganancia_total += turista.valorizaciones_nodos[ruta[i]];
        
        // Imprimir tiempo y ganancia de estancia para depuración
        std::cout << "Estancia en " << ruta[i] << ": "
                  << "Tiempo usado: " << tiempo_estancia << ", Tiempo total: " << tiempo_usado << std::endl;
        
        if (tiempo_usado > turista.tiempo_total_disponible) {
            std::cout << "Tiempo excedido en el nodo " << ruta[i] << std::endl;
            return INT_MIN;
        }
    }
    
    return ganancia_total;
}

int calcular_ganancia_total(const std::vector<int>& ruta) {
    int ganancia_sumada = 0;

    for (Turista& turista : turistas) {
        turista.ganancia = evaluar_ganancia(ruta, turista);
        if (turista.ganancia > 0) { // Si la ganancia es válida, se suma.
            ganancia_sumada += turista.ganancia;
        }
    }

    return ganancia_sumada;
}



int main() {


    leer_archivo_tipo_1("./Instances/tipo1/17_instancia.txt");
    leer_archivo_tipo_2("./Instances/tipo2/5us_17_instancia.txt");


    std::vector<int> ruta_ejemplo = {0, 1, 2, 0}; 
    tiempo_maximo_global = 6000;
    // Resto del código...

    int ganancia_total = calcular_ganancia_total(ruta_ejemplo);

        if (ganancia_total == INT_MIN) {
        std::cout << "La ruta excede el tiempo máximo permitido o es inválida." << std::endl;
    } else {
        std::cout << "La ganancia total para la ruta es: " << ganancia_total << std::endl;
    }

        for (const auto& turista : turistas) {
        std::cout << "Ganancia del turista: " << turista.ganancia << std::endl;
    }

    return 0;
}