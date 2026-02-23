#include <iostream>
#include <vector>
#include <string>
#include <chrono>   // Para medir el tiempo con precisión
#include <cstdlib>  // Para system()
#include <fstream>  // Para escribir los reportes
#include <algorithm>
#include <random>
#include <numeric>

using namespace std;
using namespace std::chrono;

void correr_benchmark(const vector<int>& perm, int id, ofstream& reporte) {
    // 1. Construir el comando
    string cmd = "./exe/IDA_inocente ";
    string params = "";
    for (int n : perm) params += to_string(n) + " ";
    string output_file = "output/res_" + to_string(id) + ".txt";
    string full_cmd = cmd + params + output_file;

    cout << "------------------------------------------" << endl;
    cout << "Probando instancia " << id << "..." << endl;

    // 2. Medir el tiempo
    auto inicio = high_resolution_clock::now();
    
    int exit_code = system(full_cmd.c_str());
    
    auto fin = high_resolution_clock::now();
    
    // 3. Calcular duración
    duration<double> tiempo = fin - inicio;

    if (exit_code == 0) {
        cout << "Terminado en: " << tiempo.count() << " segundos." << endl;
        // Guardar en el archivo de reporte (ID, Tiempo, Permutación)
        reporte << id << "," << tiempo.count() << "," << params << endl;
    } else {
        cerr << "Error: El IDA falló en la instancia " << id << endl;
    }
}

vector<vector<int>> generar_instancias(int cantidad, int N) {
    vector<vector<int>> instancias;
    
    // 1. Crear un estado base [1, 2, ..., 12]
    vector<int> base(N);
    iota(base.begin(), base.end(), 1);

    // 2. Preparar el generador de números aleatorios (Mersenne Twister)
    random_device rd;  // Semilla basada en hardware
    mt19937 g(rd());   // Motor de números aleatorios

    for (int i = 0; i < cantidad; ++i) {
        vector<int> nueva_perm = base;
        
        // 3. Mezclar aleatoriamente
        shuffle(nueva_perm.begin(), nueva_perm.end(), g);
        
        instancias.push_back(nueva_perm);
    }

    return instancias;
}

int main() {
    // Asegúrate de que la carpeta existe
    system("mkdir -p output");

    ofstream reporte("./output/benchmark_results.csv");
    reporte << "ID,Tiempo_Segundos,Permutacion" << endl;

    // Aquí puedes meter todas las permutaciones que quieras probar
    vector<vector<int>> instancias = generar_instancias(20, 12);
    
    // {
    //     {3, 10, 6, 7, 5, 12, 2, 1, 9, 4, 11, 8},
    //     {10, 6, 7, 2, 11, 5, 3, 9, 8, 4, 1, 12},
    //     {9, 11, 5, 4, 1, 8, 12, 6, 2, 3, 10, 7},
    //     {2, 3, 8, 11, 4, 6, 12, 10, 5, 7, 1, 9},
    //     {2, 3, 6, 11, 7, 4, 12, 8, 10, 5, 9, 1},
    //     {7, 3, 6, 4, 8, 1, 9, 2, 10, 11, 5, 12},
    //     {1, 2, 11, 4, 10, 6, 9, 12, 8, 3, 7, 5},
    //     {1, 3, 10, 8, 11, 7, 12, 4, 2, 5, 9, 6},
    //     {11, 8, 3, 2, 6, 5, 12, 4, 7, 1, 10, 9},
    //     {4, 6, 2, 10, 8, 9, 5, 12, 11, 3, 1, 7},
    //     {1, 6, 3, 2, 4, 12, 10, 7, 9, 5, 8, 11},
    //     {1, 9, 8, 12, 10, 3, 4, 7, 5, 2, 11, 6},
    //     {11, 8, 4, 5, 10, 3, 12, 1, 6, 2, 9, 7},
    //     {3, 4, 8, 6, 5, 2, 12, 10, 11, 1, 7, 9},
    //     {12, 9, 1, 2, 4, 6, 3, 11, 5, 7, 10, 8},
    //     {3, 10, 1, 5, 8, 9, 12, 7, 6, 2, 11, 4},
    //     {2, 4, 9, 10, 11, 5, 8, 1, 7, 12, 6, 3},
    //     {11, 7, 10, 6, 3, 4, 1, 8, 5, 9, 2, 12},
    //     {4, 12, 6, 1, 2, 8, 10, 5, 11, 7, 9, 3},
    //     {6, 2, 11, 7, 3, 8, 1, 9, 5, 10, 12, 4}
    // };

    for (int i = 0; i < instancias.size(); ++i) {
        correr_benchmark(instancias[i], i, reporte);
    }

    reporte.close();
    cout << "\nBenchmark finalizado. Resultados guardados en 'benchmark_results.csv'" << endl;

    return 0;
}