#include <iostream>
#include <vector>
#include <numeric>
#include <queue>
#include <algorithm>
#include <fstream>
#include "conmons.h"

using namespace std;

/**
 * @brief Representa un nodo del grafo virtual del problema.
 *
 * Contiene la información mínima necesaria para identificar
 * a un nodo como un estado del problema.
 */
struct node{
    vector<int> state;
    long long cost = 0;
    char last_movement = 'i';
};

vector<long long> factorial(N + 1);
vector<vector<long long>> C(N+1, vector<long long>(N+1));


/**
 * @brief Aplica una rotación de tamaño k sobre el estado.
 *
 * Realiza la inversión de los k elementos consecutivos
 * comenzando en la posición i (considerando estructura circular).
 *
 * @param n Nodo base.
 * @param i Posición inicial de la rotación.
 * @return Nuevo nodo con la rotación aplicada.
 */
node apply_rotation(node n, int i){

    int i_inv = i + k - 1;
    if(i_inv < 0){
        i_inv += N;
    } else if(i_inv >= N){
        i_inv -= N;
    }
    int iter = 0;
    while(iter < k / 2){
        
        int temp = n.state[i];
        n.state[i] = n.state[i_inv];
        n.state[i_inv] = temp;

        // Actualización de variables que permiten el swap
        i = i + 1;
        if(i >= N){i -= N;}
        i_inv = i_inv - 1;
        if(i_inv < 0){i_inv += N;}
        iter++;
    }

    return n;
}

/**
 * @brief Genera los sucesores válidos de un nodo.
 *
 * Genera hasta tres movimientos:
 *  - Rotación izquierda
 *  - Rotación derecha
 *  - Swap (inversión de k elementos)
 *
 * Evita deshacer el último movimiento aplicado.
 *
 * @param n Nodo actual.
 * @return Vector con los nodos sucesores.
 */
vector<node> succesors(const node& n){
    vector<node> succesors;
    node successor_node = n; 
    char last_movement = n.last_movement;


    if(last_movement != 'l'){
        rotate(successor_node.state.rbegin(), successor_node.state.rbegin() + 1, successor_node.state.rend());
        successor_node.cost += 1;
        successor_node.last_movement = 'r';
        succesors.push_back(successor_node);
        successor_node = n;
    }
    
    if(last_movement != 'r'){
        rotate(successor_node.state.begin(), successor_node.state.begin() + 1, successor_node.state.end());
        successor_node.cost += 1;
        successor_node.last_movement = 'l';
        succesors.push_back(successor_node);
        successor_node = n;
    }

    if(last_movement != 's'){
        successor_node = apply_rotation(n, 0);
        successor_node.cost += 1;
        successor_node.last_movement = 's';
        succesors.push_back(successor_node);
    }

    return succesors;

}


/**
 * @brief Inicializa el arreglo global de factoriales hasta n.
 *
 * Precalcula los valores factorial[i] = i! para 0 ≤ i ≤ n,
 * permitiendo consultas en O(1) posteriormente.
 * 
 * Complejidad: O(n)
 *
 * @param n Valor máximo para el cual se calculará el factorial.
 */
void init_factorials(int n) {
    factorial[0] = 1;
    for (int i = 1; i <= n; i++)
        factorial[i] = factorial[i - 1] * i;
}

/**
 * @brief Calcula el ranking de Lehmer de una permutación.
 *
 * Determina la posición lexicográfica única de la permutación
 * dentro del conjunto de todas las permutaciones posibles
 * de los mismos elementos.
 *
 * Complejidad: O(n²)
 *
 * @param perm Vector que representa una permutación.
 * @return Índice único (rank) de la permutación.
 */
long long lehmer_rank(const vector<int>& perm) {
    int n = perm.size();
    long long rank = 0;

    for (int i = 0; i < n; i++) {
        int smaller = 0;

        for (int j = i + 1; j < n; j++) {
            if (perm[j] < perm[i])
                smaller++;
        }

        rank += smaller * factorial[n - 1 - i];
    }

    return rank;
}

/**
 * @brief Inicializa la tabla de coeficientes binomiales.
 *
 * Construye la tabla C[i][j] = "i choose j" utilizando
 * programación dinámica para permitir acceso en O(1).
 * 
 * Complejidad: O(n^2)
 *
 * @param n Tamaño máximo para la tabla combinatoria.
 */
void init_combinations(int n) {
    for (int i = 0; i <= n; i++) {
        C[i][0] = C[i][i] = 1;
        for (int j = 1; j < i; j++)
            C[i][j] = C[i-1][j-1] + C[i-1][j];
    }
}

/**
 * @brief Calcula el ranking combinadic de una combinación.
 *
 * Determina el índice lexicográfico único de una combinación
 * dentro del conjunto de combinaciones posibles.
 * 
 * Complejidad: O(|comb|)
 *
 * @param comb Vector con las posiciones seleccionadas.
 * @return Índice combinatorio correspondiente.
 */
long long combinadic_rank(const vector<int>& comb) {
    long long rank = 0;
    int k = comb.size();

    for (int i = 0; i < k; i++) {
        rank += C[comb[i]][i+1];
    }

    return rank;
}


/**
 * @brief Calcula el índice único de un estado en la Pattern Database.
 *
 * Extrae los elementos pertenecientes al patrón, calcula:
 *  - el ranking de su permutación (Lehmer)
 *  - el ranking de sus posiciones (combinadic)
 * y combina ambos valores para obtener un índice único.
 *
 * @param state Estado completo del problema.
 * @return Índice correspondiente dentro de la PDB.
 */
long long find_index(vector<int> state){

    vector<int> perm;
    vector<int> positions;

    for(int i = 0; i < state.size(); i++){
        for(int j = 0; j < pattern_1.size(); j++){
            if(state[i] == pattern_1[j]){
                perm.push_back(state[i]);
                positions.push_back(i);
            }
        }
    }

    long long lehmer_index = lehmer_rank(perm);
    long long combinadic_index = combinadic_rank(positions);


    return lehmer_index + combinadic_index * factorial[pattern_size_1];

}

/**
 * @brief Construye una Pattern Database (PDB) mediante búsqueda en anchura (BFS).
 *
 * Realiza una exploración exhaustiva del espacio de estados inducido por el patrón,
 * comenzando desde el estado inicial (que corresponde al índice 0 en la PDB).
 * 
 * Para cada estado alcanzado por primera vez (propiedad garantizada por BFS),
 * se calcula su índice mediante la función de ranking del patrón y se escribe
 * en el archivo binario el costo óptimo (distancia mínima en número de movimientos)
 * desde el estado inicial hasta dicho estado.
 *
 * La función:
 * - Usa una cola para implementar BFS.
 * - Mantiene un vector `visited` indexado por el ranking combinadic + Lehmer,
 *   evitando reexplorar estados ya descubiertos.
 * - Escribe directamente en el archivo binario en la posición correspondiente
 *   al índice del estado (index * sizeof(long long)).
 *
 * @param initial Estado inicial desde el cual se genera la PDB.
 * @param output_bin Flujo binario abierto en modo escritura donde se almacenará
 *        la base de datos de patrones. Debe estar correctamente abierto antes
 *        de llamar a esta función.
 */
void breadth_first_search (vector<int> initial, ofstream &output_bin) {

    node iter_node = { initial, 0 };
    queue<node> frontier;
    long long total_size = C[N][pattern_size_1] * factorial[pattern_size_1];
    vector<bool> visited(total_size, false);
    vector<int> s;

    frontier.push(iter_node);
    visited[0] = true;

    while (!frontier.empty()){
        iter_node = frontier.front();
        frontier.pop();

        for (node child : succesors(iter_node)){

            s = child.state;
            
            long long index = find_index(s);

            if (!visited[index]){

                visited[index] = true;

                output_bin.seekp(index * sizeof(long long), ios::beg);
                output_bin.write(reinterpret_cast<char*>(&child.cost), sizeof(long long));

                frontier.push(child);
            }
            
        }
    }
    return;
}

int main(){

    init_factorials(N);
    init_combinations(N);

    // Estado inicial del patrón
    vector<int> initial_state (N);

    const char* filename = "pdb_1.bin";

    // Total de estados posibles del patrón
    long long total_size = C[N][pattern_size_1] * factorial[pattern_size_1];

    // Crea archivo binario para la PDB
    ofstream output_bin(filename, ios::binary);

    if (!output_bin) {
        cout << "Error al crear archivo\n";
        return 1;
    }
    
    long long zero = 0;

    // Inicializa el archivo con ceros
    for (long long i = 0; i < total_size; i++)
        output_bin.write(reinterpret_cast<char*>(&zero),
                        sizeof(long long));

    ifstream input_bin(filename, ios::binary);

    if (!input_bin) {
        cout << "Error al abrir archivo\n";
        return 1;
    }

    // Construye el estado base del patrón
    for(int i = 0; i < N; i++){
        for(int j = 0; j < pattern_size_1; j++){
            if(pattern_1[j] == i + 1){
                initial_state[i] = i + 1;
                break;
            } else {
                initial_state[i] = 0;
            }
        }
    }

    // Genera la PDB con BFS
    breadth_first_search(initial_state, output_bin);

    // Cierra los archivos
    output_bin.close();
    input_bin.close();

    return 0;
}