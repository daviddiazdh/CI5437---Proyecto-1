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

    // Aquí puedes meter todas las permutaciones que quieras probar
    vector<vector<int>> instancias = generar_instancias(20, 12);

    cout << "{";
    for (int i = 0; i < instancias.size(); ++i) {
        cout << "{";
        for(int j = 0; j < 12; j++){
            if(j == 11){
                cout << instancias[i][j];
            } else{
                cout << instancias[i][j] << ", ";
            }
        }
        if(i == 19){
            cout << "}";
        } else {
            cout << "}," << endl;
        }
    }
    cout << "}" << endl;

    return 0;
}