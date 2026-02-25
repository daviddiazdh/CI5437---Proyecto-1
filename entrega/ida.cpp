#include <iostream>
#include <vector>
#include <limits>
#include <numeric>
#include <functional>
#include <cctype>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include <cmath>
#include <fstream>
#include "./utils/conmons.h"
#include <random>

using namespace std;

unsigned long long generated_states = 0;

vector<long long> factorial(N + 1);
vector<vector<long long>> C(N+1, vector<long long>(N+1));
vector<vector<int>> path;
vector<long long> pdb_in_ram_1;
vector<long long> pdb_in_ram_2;

vector<bool> is_in_p1(N + 1, false);
vector<bool> is_in_p2(N + 1, false);

/**
 * @brief Representa un nodo del grafo virtual del problema.
 *
 * Contiene la información mínima necesaria para identificar
 * a un nodo como un estado del problema.
 */
struct node{
    vector<int> state;
    char last_movement = 'i';
    double cost = 0;
};


struct dfs_output{
    node solution;
    double f_limit;
    bool isNull;

    dfs_output(double f, bool iN){
        solution = {};
        f_limit = f;
        isNull = iN;
    }

    dfs_output(node s, double f){
        solution = s;
        f_limit = f;
        isNull = false;
    }

    dfs_output(node s, double f, bool iN){
        solution = s;
        f_limit = f;
        isNull = iN;
    }

};


node goal_node{ std::vector<int>(N) };


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

        // contar elementos menores a la derecha
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
long long find_index(vector<int> state, const vector<bool> is_in_p, int p_size){

    vector<int> perm(p_size); // Tamaño fijo
    vector<int> positions(p_size);
    int count = 0;

    for (int i = 0; i < N; i++) {
        if (is_in_p[state[i]]) {
            perm[count] = state[i];
            positions[count] = i;
            count++;
        }
    }

    long long lehmer_index = lehmer_rank(perm);
    long long combinadic_index = combinadic_rank(positions);

    return lehmer_index + combinadic_index * factorial[pattern_size_1];

}

/**
 * @brief Función de evaluación f(n) para IDA*.
 *
 * f(n) = g(n) + h(n), donde h(n) se obtiene desde
 * la Pattern Database cargada en memoria.
 *
 * @param n Nodo actual.
 * @param pdb_bin Archivo binario (no utilizado directamente si está en RAM).
 * @return Valor f correspondiente.
 */
double f(node& n, ifstream& pdb_bin){

    long long index_1 = find_index(n.state, is_in_p1, pattern_size_1);
    long long index_2 = find_index(n.state, is_in_p2, pattern_size_2);

    return n.cost + max(pdb_in_ram_1[index_1], pdb_in_ram_2[index_2]);
}

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
 * @brief Verifica si un nodo corresponde al estado meta.
 *
 * @param n Nodo a evaluar.
 * @return true si el estado es el objetivo, false en caso contrario.
 */
bool goal_test(node &n){

    for(int i = 0; i < N; i++){
        if(n.state[i] != goal_node.state[i]){
            return false;
        }
    }
    return true;

}

/**
 * @brief Realiza la búsqueda en profundidad limitada por contorno (IDA*).
 *
 * Explora recursivamente mientras f(n) ≤ f_limit.
 * Si supera el límite, devuelve el menor valor f encontrado.
 *
 * @param n Nodo actual.
 * @param f_limit Límite actual de la función f.
 * @param f Función de evaluación.
 * @param pdb_bin Archivo binario asociado a la PDB.
 * @return Estructura con solución o nuevo límite.
 */
dfs_output dfs_contour(node &n, double f_limit, double (*f)(node&, ifstream&), ifstream& pdb_bin){

    double node_cost = f(n, pdb_bin);

    if(node_cost > f_limit){ return dfs_output({}, node_cost, true); }

    if(goal_test(n)){ return dfs_output(n, f_limit); }

    double next_f = numeric_limits<double>::infinity();
    
    for(node s : succesors(n)){
        generated_states++;
        path.push_back(s.state);

        dfs_output new_dfs_output = dfs_contour(s, f_limit, f, pdb_bin);
        if(!new_dfs_output.isNull){ return new_dfs_output; }

        path.pop_back();

        next_f = min(next_f, new_dfs_output.f_limit);
    }

    return dfs_output({}, next_f, true);

}

/**
 * @brief Construye el estado inicial a partir de argumentos de línea de comandos.
 *
 * @param argv Arreglo de argumentos.
 * @return Vector que representa el estado inicial.
 * @throws std::invalid_argument Si algún argumento no es convertible a entero.
 */
vector<int> initial_state(char* argv[]){

    vector<int> initial(N,0);
    for(int i = 1; i <= N; i++){
        char* digit = argv[i];
        int num = stoi(digit);
        initial[i - 1] = num;
    }

    return initial;

}

/**
 * @brief Implementación del algoritmo IDA*.
 *
 * Incrementa iterativamente el límite f hasta encontrar solución
 * o determinar que no existe.
 *
 * @param initial_state Estado inicial del problema.
 * @param f Función de evaluación.
 * @param pdb_bin Archivo asociado a la PDB.
 * @return Nodo solución.
 * @throws std::runtime_error Si no se encuentra solución.
 */
node ida(vector<int> initial_state, double (*f)(node&, ifstream&), ifstream& pdb_bin){

    node root = {initial_state};

    double f_limit = f(root, pdb_bin);
    
    while (true){
        dfs_output dfs_solution = dfs_contour(root, f_limit, f, pdb_bin);
        f_limit = dfs_solution.f_limit;
        if(!dfs_solution.isNull){
            return dfs_solution.solution;
        }
        if(dfs_solution.f_limit == numeric_limits<double>::infinity()){
            throw runtime_error("No se consiguió solución");
        }
    }
}

/**
 * @brief Imprime un vector en un archivo de salida.
 *
 * @param n Vector a imprimir.
 * @param output_file Archivo destino.
 */
void print_vector(vector<int> &n, ofstream &output_file){

    output_file << "[";
    int i = 0;
    while(i < N){
        if(i == N - 1){
            output_file << n[i];
        } else {
            output_file << n[i] << ", ";
        }
        i++;
    }

    output_file << "]" << endl;

}

/**
 * @brief Carga la Pattern Database completa en memoria RAM.
 *
 * Lee desde el archivo binario todos los valores
 * y los almacena en el vector global pdb_in_ram.
 *
 * @param input_bin Archivo binario de entrada.
 * @param total_size Número total de entradas de la PDB.
 */
void load_pdb(ifstream& input_bin, long long total_size, vector<long long> &pdb_in_ram) {
    pdb_in_ram.resize(total_size);
    input_bin.read(reinterpret_cast<char*>(pdb_in_ram.data()), total_size * sizeof(long long));
    input_bin.close();
}

int main(int argc, char* argv[]){

    // Precalcula factoriales y combinaciones.
    init_factorials(N);
    init_combinations(N);
    // init_zobrist();

    for(int x : pattern_1) is_in_p1[x] = true;
    for(int x : pattern_2) is_in_p2[x] = true;

    // Inicializa el estado meta como [1, 2, ..., N].
    iota(goal_node.state.begin(), goal_node.state.end(), 1);

    // Verifica que la cantidad de argumentos sea correcta.
    if(argc != N + 2){
        cout << "Error: Debe enviar exactamente " << N + 1 << " argumentos." << endl;
        return 1;
    }

    // Obtiene el nombre del archivo donde se escribirá la solución.
    char* file_name = argv[N + 1];
    ofstream output_file(file_name);

    // Verifica que el archivo de salida pueda abrirse correctamente.
    if (!output_file.is_open()) {
        cerr << "No se pudo hallar el archivo" << endl;
        output_file.close();
        return 1;
    }

    vector<int> initial;
    node solution;

    // Construye el estado inicial a partir de los argumentos.
    try{
        initial = initial_state(argv);
    } catch (...){
        cerr << "Los primeros 12 números deben ser enteros." << endl;
        return 1;
    }

    // Abre el archivo binario que contiene la Pattern Database del primer patrón.
    ifstream input_bin_1("pdb_1.bin", ios::binary);

    if (!input_bin_1) {
        cout << "Error al abrir archivo\n";
        return 1;
    }

    // Abre el archivo binario que contiene la Pattern Database del segundo patrón.
    ifstream input_bin_2("pdb_2.bin", ios::binary);

    if (!input_bin_2) {
        cout << "Error al abrir archivo\n";
        return 1;
    }

    // Calcula el tamaño total de las PDB
    long long total_size_1 = C[N][pattern_size_1] * factorial[pattern_size_1];
    long long total_size_2 = C[N][pattern_size_2] * factorial[pattern_size_2];
    load_pdb(input_bin_1, total_size_1, pdb_in_ram_1);
    load_pdb(input_bin_2, total_size_2, pdb_in_ram_2);

    // Ejecuta el algoritmo IDA*.
    try{
        solution = ida(initial, f, input_bin_1);
    } catch (...){
        cerr << "No fue encontrada una solución." << endl;
        return 1;
    }

    // Imprime el estado inicial y el camino solución en el archivo.
    print_vector(initial, output_file);
    int i = 0;
    while(i < path.size()){
        if(i == path.size() - 1){
            print_vector(path[i], output_file);
        } else {
            print_vector(path[i], output_file);
        }
        i++;
    }

    cout << "Longitud del camino desde el estado inicial hasta la meta: " << path.size() + 1 << endl;
    cout << "Estados generados: " << generated_states << endl;

    output_file.close();
    input_bin_1.close();
    input_bin_2.close();
}

