#include <iostream>
#include <vector>
#include <fstream>  
#include <sstream>  
#include <algorithm> // para std::find
#include <string>
#include <climits> 
#include <cstdlib> // Para srand y rand
#include <ctime>   // Para time
#include <list>

using namespace std;

struct Turista {
    int tiempo_total_disponible;
    std::vector<int> valorizaciones_nodos;
    std::vector<std::vector<int>> valorizaciones_arcos;
    int ganancia; 
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

    for (size_t i = 0; i < ruta.size() - 1; ++i) {
        int desde = ruta[i];
        int hasta = ruta[i + 1];

        // Comprobar si hay conexión entre 'desde' y 'hasta'
        if (tiempos_arcos_globales[desde][hasta] == -1) {
            return INT_MIN; // Ruta inválida debido a falta de conexión
        }

        // Sumar tiempos y ganancias
        tiempo_usado += tiempos_nodos_globales[desde] + tiempos_arcos_globales[desde][hasta];
        ganancia_total += turista.valorizaciones_nodos[desde] + turista.valorizaciones_arcos[desde][hasta];

        // Comprobar si se excede el tiempo disponible
        if (tiempo_usado > turista.tiempo_total_disponible) {
            return INT_MIN; // Ruta inválida por exceder el tiempo
        }
    }

    // Agregar tiempo y ganancia del último nodo
    tiempo_usado += tiempos_nodos_globales[ruta.back()];
    ganancia_total += turista.valorizaciones_nodos[ruta.back()];

    if (tiempo_usado > turista.tiempo_total_disponible) {
        return INT_MIN; // Ruta inválida por exceder el tiempo
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


bool esTabu(const std::pair<int, int>& par, const std::vector<std::pair<int, int>>& lista_tabu) {
    for (const auto& item : lista_tabu) {
        if ((item.first == par.first && item.second == par.second) || 
            (item.first == par.second && item.second == par.first)) {
            return true;
        }
    }
    return false;
}


std::pair<std::vector<int>, std::pair<int, int>> generar_mejor_vecino(const std::vector<int>& ruta_actual, const Turista& turista, const std::vector<std::pair<int, int>>& lista_tabu) {
    int mejor_ganancia = evaluar_ganancia(ruta_actual, turista);
    std::vector<int> mejor_vecino = ruta_actual;
    std::pair<int, int> movimiento = std::make_pair(-1, -1); // Movimiento nulo inicial

    for (size_t i = 1; i < ruta_actual.size() - 2; ++i) {
        for (size_t j = i + 1; j < ruta_actual.size() - 1; ++j) {
            std::vector<int> vecino = ruta_actual;
            std::swap(vecino[i], vecino[j]);

            if (!esTabu(std::make_pair(vecino[i], vecino[j]), lista_tabu)) {
                int ganancia_vecino = evaluar_ganancia(vecino, turista);

                if (ganancia_vecino > mejor_ganancia) {
                    mejor_ganancia = ganancia_vecino;
                    mejor_vecino = vecino;
                    movimiento = std::make_pair(i, j); // Movimiento realizado
                    return std::make_pair(mejor_vecino, movimiento); // Retorna si encuentra una mejora
                }
            }
        }
    }

    return std::make_pair(mejor_vecino, movimiento); // Retorna la ruta actual y un movimiento nulo si no encuentra mejora
}






std::vector<int> generar_ruta_aleatoria(int longitud, const std::vector<std::vector<int>>& tiempos_arcos) {
    std::vector<int> ruta;
    int intentos = 0;
    const int max_intentos = 100; // Un límite arbitrario para evitar bucles infinitos

    while (ruta.size() < longitud && intentos < max_intentos) {
        if (ruta.empty()) {
            ruta.push_back(0); // Comenzar siempre en el nodo 0
        } else {
            int ultimo_nodo = ruta.back();
            std::vector<int> candidatos;
            
            for (int i = 1; i < tiempos_arcos.size(); ++i) {
                if (tiempos_arcos[ultimo_nodo][i] != -1 && std::find(ruta.begin(), ruta.end(), i) == ruta.end()) {
                    candidatos.push_back(i);
                }
            }

            if (candidatos.empty()) {
                ruta.clear(); // Reiniciar la ruta si no hay candidatos válidos
                intentos++;
                continue;
            }

            int elegido = candidatos[rand() % candidatos.size()];
            ruta.push_back(elegido);
        }
    }

    if (ruta.size() < longitud) {
        // Manejar el caso en que no se pudo generar una ruta válida
    }

    return ruta;
}



std::vector<int> aplicar_busqueda_tabu(std::vector<int> ruta_actual, const Turista& turista, int iteraciones, int tamano_tabu) {
    std::vector<int> mejor_ruta = ruta_actual;
    int mejor_ganancia = calcular_ganancia_total(ruta_actual);
    std::vector<std::pair<int, int>> lista_tabu;

    for (int iter = 0; iter < iteraciones; ++iter) {
        std::pair<std::vector<int>, std::pair<int, int>> resultado = generar_mejor_vecino(ruta_actual, turista, lista_tabu);
        std::vector<int> vecino = resultado.first;
        std::pair<int, int> movimiento = resultado.second;
        
        int ganancia_vecino = calcular_ganancia_total(vecino);
        if (ganancia_vecino > mejor_ganancia) {
            mejor_ruta = vecino;
            mejor_ganancia = ganancia_vecino;

            if (movimiento.first != -1 && movimiento.second != -1) {
                lista_tabu.push_back(movimiento);
                if (lista_tabu.size() > tamano_tabu) {
                    lista_tabu.erase(lista_tabu.begin());
                }
            }
        }

        ruta_actual = vecino;
    }

    return mejor_ruta;
}



int main() {
    // Inicialización del generador de números aleatorios
    srand(static_cast<unsigned>(time(nullptr)));


    leer_archivo_tipo_1("./Instances/tipo1/17_instancia.txt");
    leer_archivo_tipo_2("./Instances/tipo2/5us_17_instancia.txt");

    // Asignar un tiempo máximo global
    tiempo_maximo_global = 6000;
    int tamano_tabu = 50;
    int iteraciones = 5000;

    // Generar rutas aleatorias y calcular sus ganancias
    std::vector<std::vector<int>> rutas;
    std::vector<int> ganancias;

    int n = tiempos_nodos_globales.size(); 
    for (int longitud = 2; longitud < n; ++longitud) {
        std::vector<int> ruta = generar_ruta_aleatoria(longitud, tiempos_arcos_globales);
        rutas.push_back(ruta);

        int ganancia = calcular_ganancia_total(ruta); // Calcula la ganancia total para la ruta
        ganancias.push_back(ganancia);
    }

    // Aplicar búsqueda tabú a cada ruta
    for (int i = 0; i < rutas.size(); ++i) {
        for (const auto& turista : turistas) {
            std::vector<int> ruta_mejorada = aplicar_busqueda_tabu(rutas[i], turista, iteraciones, tamano_tabu);
            rutas[i] = ruta_mejorada; // Actualizar la ruta con la versión mejorada

            int ganancia_mejorada = calcular_ganancia_total(ruta_mejorada); // Calcular la ganancia para la ruta mejorada
            ganancias[i] = ganancia_mejorada; // Actualizar la ganancia con el nuevo valor
        }
    }

    // Imprimir los resultados
    for (int i = 0; i < rutas.size(); ++i) {
        std::cout << "Ganancia para la ruta " << i << ": " << ganancias[i] << std::endl;
    }
    return 0;
}