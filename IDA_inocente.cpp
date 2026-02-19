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

using namespace std;

int N = 12;
int k = 4;
unsigned long long generated_states = 0;

vector<long long> factorial(N + 1);
vector<vector<long long>> C(N+1, vector<long long>(N+1));
vector<int> pattern = {1,3,5,7,9,11};
int pattern_size = pattern.size();

struct node{
    vector<int> state;
    char last_movement = 'i';
    double cost = 0;
};

vector<vector<int>> path;

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


// Lógica del patrón

void init_factorials(int n) {
    factorial[0] = 1;
    for (int i = 1; i <= n; i++)
        factorial[i] = factorial[i - 1] * i;
}

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


// Precalcular tabla de combinaciones
void init_combinations(int n) {
    for (int i = 0; i <= n; i++) {
        C[i][0] = C[i][i] = 1;
        for (int j = 1; j < i; j++)
            C[i][j] = C[i-1][j-1] + C[i-1][j];
    }
}

// Ranking combinadic
long long combinadic_rank(const vector<int>& comb) {
    long long rank = 0;
    int k = comb.size();

    for (int i = 0; i < k; i++) {
        rank += C[comb[i]][i+1];
    }

    return rank;
}

long long find_index(vector<int> state){

    vector<int> perm;
    vector<int> positions;

    for(int i = 0; i < state.size(); i++){
        for(int j = 0; j < pattern.size(); j++){
            if(state[i] == pattern[j]){
                perm.push_back(state[i]);
                positions.push_back(i);
            }
        }
    }

    // print_vector(perm);
    // print_vector(positions);

    long long lehmer_index = lehmer_rank(perm);
    long long combinadic_index = combinadic_rank(positions);

    // cout << "lehmer_index: " << lehmer_index << endl;
    // cout << "combinadic_index: " << combinadic_index << endl;

    return lehmer_index + combinadic_index * factorial[pattern_size];

}

// Fin de lógica del patrón

double heuristic(node& n){

    int breakpoints = 0;

    for(int i = 0; i < N - 1; i++){
        if(n.state[i] + 1 != n.state[i + 1]){
            breakpoints++;
        }
    }

    return ceil(breakpoints / 2.0);
}

double f(node& n, ifstream& pdb_bin){

    long long index = find_index(n.state);

    // cout << "INDEX: " << index << endl;

    long long pdb_cost;

    pdb_bin.seekg(index * sizeof(long long), ios::beg);
    pdb_bin.read(reinterpret_cast<char*>(&pdb_cost), sizeof(long long));

    // cout << "PDB_COST: " << pdb_cost << endl;

    return n.cost + pdb_cost;
}

void print_state(node &n){

    cout << "[";
    int i = 0;
    while(i < N){
        if(i == N - 1){
            cout << n.state[i];
        } else {
            cout << n.state[i] << ", ";
        }
        i++;
    }

    cout << "]" << endl;

}

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

bool goal_test(node &n){

    for(int i = 0; i < N; i++){
        if(n.state[i] != goal_node.state[i]){
            return false;
        }
    }
    return true;

}

dfs_output dfs_contour(node &n, double f_limit, double (*f)(node&, ifstream&), ifstream& pdb_bin){

    double next_f = numeric_limits<double>::infinity();

    double node_cost = f(n, pdb_bin);

    if(node_cost > f_limit){ return dfs_output({}, node_cost, true); }

    if(goal_test(n)){ return dfs_output(n, f_limit); }
    
    // cout << "Abriendo sucesores..." << endl;
    for(node s : succesors(n)){
        //cout << "Para el sucesor s = ";
        // print_state(s);
        // cout << "con costo " << s.cost << endl;
        generated_states++;
        path.push_back(s.state);

        dfs_output new_dfs_output = dfs_contour(s, f_limit, f, pdb_bin);
        if(!new_dfs_output.isNull){ return new_dfs_output; }

        path.pop_back();

        next_f = min(next_f, new_dfs_output.f_limit);
    }

    return dfs_output({}, next_f, true);

}

vector<int> initial_state(char* argv[]){

    vector<int> initial(N,0);
    for(int i = 1; i <= N; i++){
        char* digit = argv[i];
        int num = stoi(digit);
        initial[i - 1] = num;
    }

    return initial;

}

node ida(vector<int> initial_state, double (*f)(node&, ifstream&), ifstream& pdb_bin){

    node root = {initial_state};

    double f_limit = f(root, pdb_bin);
    // cout << "Primer valor de f_limit: " << f_limit << endl;
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


int main(int argc, char* argv[]){

    // Precalcular los factoriales y números combinatorios para dejarlos en un arreglo 
    // de modo que calcularlos luego sea O(1)
    init_factorials(N);
    init_combinations(N);

    // Definición de estado meta:
    iota(goal_node.state.begin(), goal_node.state.end(), 1);

    // Verificación de argumentos
    if(argc != N + 2){
        cout << "Error: Debe enviar exactamente " << N + 1 << " argumentos." << endl;
        return 1;
    }

    // Obtención de nombre de archivo de texto donde se registarará la solución (en caso de existir una)
    char* file_name = argv[N + 1];
    ofstream output_file(file_name);

    if (!output_file.is_open()) {
        cerr << "No se pudo hallar el archivo" << endl;
        output_file.close();
        return 1;
    }

    vector<int> initial;
    node solution;

    try{
        initial = initial_state(argv);
    } catch (...){
        cerr << "Los primeros 12 números deben ser enteros." << endl;
        return 1;
    }

    ifstream input_bin("pdb.bin", ios::binary);

    if (!input_bin) {
        cout << "Error al abrir archivo\n";
        return 1;
    }

    try{
        solution = ida(initial, f, input_bin);
    } catch (...){
        cerr << "No fue encontrada una solución." << endl;
        return 1;
    }

    
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
    input_bin.close();
}

