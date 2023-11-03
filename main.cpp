#include <iostream>
#include <vector>
#include <fstream>  // Necesario para leer archivos
#include <sstream>  // Necesario para dividir strings
#include <string>

using namespace std;

struct Turista {
    int tiempo_max;
    vector<int> valorizaciones_nodos;
    vector<vector<int>> valorizaciones_arcos;
};

void leer_archivo_1(const string& nombre_archivo, vector<int>& tiempos_nodos, vector<vector<int>>& tiempos_arcos) {
    ifstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << nombre_archivo << endl;
        return;
    }

    int num_nodos;
    archivo >> num_nodos;

    tiempos_nodos.resize(num_nodos);
    tiempos_arcos.resize(num_nodos, vector<int>(num_nodos));

    for (int i = 0; i < num_nodos; ++i) {
        archivo >> tiempos_nodos[i];
    }

    for (int i = 0; i < num_nodos; ++i) {
        for (int j = 0; j < num_nodos; ++j) {
            archivo >> tiempos_arcos[i][j];
        }
    }

    archivo.close();
}


void leer_archivo_tipo_2(const string& nombre_archivo, vector<Turista>& turistas) {
    ifstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << nombre_archivo << endl;
        return;
    }

    int num_turistas;
    archivo >> num_turistas;

    turistas.resize(num_turistas);

    for (int t = 0; t < num_turistas; ++t) {
        archivo >> turistas[t].tiempo_max;

        int num_nodos;
        archivo >> num_nodos;

        turistas[t].valorizaciones_nodos.resize(num_nodos);
        turistas[t].valorizaciones_arcos.resize(num_nodos, vector<int>(num_nodos));

        for (int i = 0; i < num_nodos; ++i) {
            archivo >> turistas[t].valorizaciones_nodos[i];
        }

        for (int i = 0; i < num_nodos; ++i) {
            for (int j = 0; j < num_nodos; ++j) {
                archivo >> turistas[t].valorizaciones_arcos[i][j];
            }
        }
    }

    archivo.close();
}
