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
    string cmd = "./exe/IDA ";
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
    // vector<vector<int>> instancias = {
    //     {4, 1, 3, 7, 10, 2, 12, 5, 8, 11, 6, 9},
    //     {12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}, // El peor caso
    //     {1, 3, 5, 7, 9, 11, 2, 4, 6, 8, 10, 12}
    // };

    vector<vector<int>> instancias = generar_instancias(20, 12);

    for (int i = 0; i < instancias.size(); ++i) {
        correr_benchmark(instancias[i], i, reporte);
    }

    reporte.close();
    cout << "\nBenchmark finalizado. Resultados guardados en 'benchmark_results.csv'" << endl;

    return 0;
}