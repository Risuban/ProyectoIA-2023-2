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
#include <chrono> // Para cronometraje

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
            // Elegir un nodo de inicio aleatorio
            int nodo_inicio = rand() % tiempos_arcos.size();
            ruta.push_back(nodo_inicio);
        } else {
            int ultimo_nodo = ruta.back();
            std::vector<int> candidatos;

            for (int i = 0; i < tiempos_arcos.size(); ++i) {
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

struct EstadisticasTurista {
    int valorizacion;
    int tiempoUsado;
    int tiempoRestante;
};

EstadisticasTurista calcularEstadisticas(const std::vector<int>& ruta, const Turista& turista) {
    int tiempoUsado = 0;
    int valorizacion = 0;

    for (size_t i = 0; i < ruta.size() - 1; ++i) {
        tiempoUsado += tiempos_arcos_globales[ruta[i]][ruta[i + 1]];
        valorizacion += turista.valorizaciones_arcos[ruta[i]][ruta[i + 1]];
    }

    for (int nodo : ruta) {
        tiempoUsado += tiempos_nodos_globales[nodo];
        valorizacion += turista.valorizaciones_nodos[nodo];
    }

    int tiempoRestante = turista.tiempo_total_disponible - tiempoUsado;

    return {valorizacion, tiempoUsado, tiempoRestante};
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <archivo_tipo1> <archivo_tipo2> <numero_iteraciones_exploracion>" << std::endl;
        return 1;
    }

    std::string archivo_tipo1 = argv[1];
    std::string archivo_tipo2 = argv[2];
    int numero_iteraciones_exploracion = std::stoi(argv[3]);
    // Inicialización del generador de números aleatorios
    srand(static_cast<unsigned>(time(nullptr)));
    auto inicio = std::chrono::high_resolution_clock::now();

    leer_archivo_tipo_1("./Instances/tipo1/17_instancia.txt");
    leer_archivo_tipo_2("./Instances/tipo2/5us_17_instancia.txt");

    int tamano_tabu = 50;
    int iteraciones = 5000;
    int numero_iteraciones_exploracion = 10; // Número de veces que se repite el proceso de exploración

    // Inicializar contenedores para las mejores rutas y ganancias por turista
    std::vector<std::vector<int>> mejor_ruta_por_turista(turistas.size());
    std::vector<int> mejor_ganancia_por_turista(turistas.size(), INT_MIN);

    for (int iteracion = 0; iteracion < numero_iteraciones_exploracion; ++iteracion) {
        // Generar nuevas rutas aleatorias
        std::vector<std::vector<int>> rutas;
        int n = tiempos_nodos_globales.size();
        for (int longitud = 2; longitud < n; ++longitud) {
            rutas.push_back(generar_ruta_aleatoria(longitud, tiempos_arcos_globales));
        }

        // Aplicar búsqueda tabú para cada turista con las nuevas rutas
        for (size_t idx_turista = 0; idx_turista < turistas.size(); ++idx_turista) {
            const auto& turista = turistas[idx_turista];

            for (const auto& ruta : rutas) {
                std::vector<int> ruta_mejorada = aplicar_busqueda_tabu(ruta, turista, iteraciones, tamano_tabu);
                int ganancia_mejorada = evaluar_ganancia(ruta_mejorada, turista);

                if (ganancia_mejorada > mejor_ganancia_por_turista[idx_turista]) {
                    mejor_ruta_por_turista[idx_turista] = ruta_mejorada;
                    mejor_ganancia_por_turista[idx_turista] = ganancia_mejorada;
                }
            }
        }
    }

    // Medir el tiempo de ejecución
    auto fin = std::chrono::high_resolution_clock::now();
    auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();
    std::cout << "Tiempo de ejecucion: " << duracion << " milisegundos." << std::endl;

    // Imprimir los resultados para cada turista
    for (size_t idx_turista = 0; idx_turista < turistas.size(); ++idx_turista) {
        std::cout << "Turista " << idx_turista << ":" << std::endl;
        if (mejor_ganancia_por_turista[idx_turista] == INT_MIN) {
            std::cout << "No se pudo obtener una ruta apropiada." << std::endl;
        } else {
            std::cout << "Mejor ganancia: " << mejor_ganancia_por_turista[idx_turista] << std::endl;
            std::cout << "Mejor ruta: ";
            for (int nodo : mejor_ruta_por_turista[idx_turista]) {
                std::cout << nodo << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    return 0;
}