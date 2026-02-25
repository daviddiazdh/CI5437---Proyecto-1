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
    string output_file = "output/res_0.txt";
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
    vector<vector<int>> instancias = generar_instancias(1000, 12);
    
    // vector<vector<int>> instancias = {{7, 2, 1, 6, 5, 11, 4, 3, 10, 9, 12, 8},
    //                                     {10, 7, 6, 2, 3, 12, 5, 8, 1, 11, 4, 9},
    //                                     {10, 9, 11, 7, 4, 12, 1, 5, 8, 6, 3, 2},
    //                                     {8, 12, 7, 2, 5, 1, 4, 10, 3, 6, 11, 9},
    //                                     {4, 6, 11, 3, 10, 12, 9, 8, 1, 5, 7, 2},
    //                                     {3, 5, 4, 6, 12, 8, 11, 7, 1, 9, 10, 2},
    //                                     {6, 7, 8, 11, 3, 4, 9, 1, 12, 10, 5, 2},
    //                                     {9, 1, 4, 12, 6, 8, 5, 2, 10, 3, 7, 11},
    //                                     {11, 4, 9, 7, 10, 3, 8, 12, 1, 5, 2, 6},
    //                                     {2, 9, 3, 8, 5, 12, 11, 4, 10, 1, 6, 7},
    //                                     {6, 11, 2, 10, 5, 3, 8, 4, 7, 1, 9, 12},
    //                                     {7, 11, 12, 2, 1, 10, 5, 8, 6, 4, 3, 9},
    //                                     {2, 5, 1, 6, 11, 4, 9, 10, 3, 12, 8, 7},
    //                                     {4, 2, 7, 11, 10, 1, 6, 9, 12, 3, 8, 5},
    //                                     {8, 3, 4, 10, 6, 9, 1, 7, 12, 2, 5, 11},
    //                                     {2, 8, 4, 7, 12, 3, 6, 5, 9, 10, 1, 11},
    //                                     {3, 1, 12, 10, 9, 11, 7, 2, 5, 8, 6, 4},
    //                                     {12, 11, 9, 6, 5, 3, 10, 7, 2, 8, 4, 1},
    //                                     {4, 3, 1, 7, 11, 8, 6, 9, 12, 5, 10, 2},
    //                                     {1, 6, 5, 12, 3, 9, 10, 2, 4, 8, 7, 11}};


    for (int i = 0; i < instancias.size(); ++i) {
        correr_benchmark(instancias[i], i, reporte);
    }

    reporte.close();
    cout << "\nBenchmark finalizado. Resultados guardados en 'benchmark_results.csv'" << endl;

    return 0;
}