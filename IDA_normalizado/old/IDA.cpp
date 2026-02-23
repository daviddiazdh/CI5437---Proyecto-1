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
#include <map>
#include <fstream>

using namespace std;

int N = 12;
int k = 4;
unordered_map<int, int> movements_in_window;
vector<int> path;
unsigned long long path_length = 0;
unsigned long long generated_states = 0;
int pos_one;

struct node{
    vector<int> state;
    double cost = 0;
    int last_rotation = -1;
    int pos_one = -1;
    int depth = 0;
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

double heuristic(node& n){

    int breakpoints = 0;

    for(int i = 0; i < N - 1; i++){
        if(n.state[i] + 1 != n.state[i + 1]){
            breakpoints++;
        }
    }

    return ceil(breakpoints / 2.0);
}

double f(node& n){
    return n.cost + heuristic(n);
}

void print_state(node &n, ofstream& output_file){

    output_file << "[";
    int i = 0;
    while(i < N){
        if(i == N - 1){
            output_file << n.state[i];
        } else {
            output_file << n.state[i] << ", ";
        }
        i++;
    }

    output_file << "]" << endl;

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

node normalize(node &n){
    node normalized_node = n;
    // Encontrar el índice donde se encuentra el valor 1
    int one_pos = 0;
    while(n.state[one_pos] != 1){
        one_pos++;
    }

    int iter = 0;
    int iter_mod = one_pos;
    while(iter < N){
        normalized_node.state[iter] = n.state[iter_mod];
        iter++;
        iter_mod = iter_mod + 1;
        if(iter_mod >= N){
            iter_mod -= N;
        }
    }

    return normalized_node;

}

int banned_rotation(int last_rotation){

    int special_value = N - k + 1;

    if (last_rotation == 0){
        return special_value;
    } else if (last_rotation == special_value){
        return 0;
    } else if (last_rotation > special_value){
        int special_half = (k - 2) / 2;
        int special_dif = (N - 1) - last_rotation;
        if (special_dif < special_half){
            return (N - 1) - (k - 2) + 1 + special_dif;
        } else {
            return special_value + 1 + special_dif;
        }
    }

    return last_rotation;
    
}

int movements_to_zero(int position){
    
    int right_movement = N - position;
    if(position < right_movement){
       return -1 * position;
    } else{
        return right_movement;
    }

}

void calculate_real_cost(node &n, int rotation){

    int pos_change = n.pos_one + rotation;
    if(pos_change >= N){
        pos_change -= N;
    }

    int movement = movements_to_zero(pos_change);
    // cout << "Cantidad de movimientos para mover a la ventana: " << movement << endl;
    int new_pos_one = n.pos_one + movement;

    if(new_pos_one >= N){
        new_pos_one -= N;
    } else if (new_pos_one < 0){
        new_pos_one += N;
    }

    n.cost += abs(movement) + 1;
    int special_value = N - k + 1;
    // cout << "Costo: " << n.cost << endl;

    if(rotation == 0 || (rotation >= special_value)){
        new_pos_one = new_pos_one + movements_in_window[rotation];
        if(new_pos_one >= N){
            new_pos_one -= N;
        } else if (new_pos_one < 0){
            new_pos_one += N;
        }
        n.pos_one = new_pos_one;
        // cout << "Nueva posición del uno: " << new_pos_one << endl;
    } else {
        n.pos_one = new_pos_one;
        // cout << "Nueva posición del uno: " << new_pos_one << endl;
    }

    // cout << "------------------------------------------" << endl;

    return;

}

vector<node> succesors(const node& n){
    vector<node> succesors;

    int prohibited_rotation = banned_rotation(n.last_rotation);

    for(int i = 0; i < N; i++){

        // cout << "Rotación: " << i << endl;
        if(i == prohibited_rotation){
            // cout << "Se prohibió la rotación: " << prohibited_rotation << ", dado que la rotación anterior fue " << n.last_rotation << endl;
            continue;
        }
        node next = apply_rotation(n, i);
        next.pos_one = n.pos_one;
        // cout << "Pasando de padre a hijo la posición de uno: " << n.pos_one << endl;
        if(next.state[0] != 1){
            next = normalize(next);
        }
        calculate_real_cost(next, i);
        // next.cost = n.cost + 1;
        next.last_rotation = i;
        next.depth = n.depth + 1;
        succesors.push_back(next);
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

dfs_output dfs_contour(node &n, double f_limit, double (*f)(node&)){

    double next_f = numeric_limits<double>::infinity();

    double node_cost = f(n);

    if(node_cost > f_limit){ return dfs_output({}, node_cost, true); }

    if(goal_test(n)){ return dfs_output(n, f_limit); }
    
    // cout << "Abriendo sucesores..." << endl;
    for(node s : succesors(n)){
        // cout << "Para el sucesor s = ";
        // print_state(s);
        // cout << "con costo " << s.cost << " y profundidad " << s.depth <<  endl;
        generated_states++;
        path.push_back(s.last_rotation);

        dfs_output new_dfs_output = dfs_contour(s, f_limit, f);
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


int detect_pos_one(node root){

    int iter = 0;
    for(int i : root.state){
        if(i == 1){
            return iter;
        }
        iter++;
    }

    return -1;

}

node ida(vector<int> initial_state, double (*f)(node&)){

    node root = {initial_state};
    pos_one = detect_pos_one(root);
    root.pos_one = pos_one;

    // cout << "Primer valor de pos_one: " << root.pos_one << endl;

    root = normalize(root);
    double f_limit = f(root);
    root.last_rotation = -1;
    // cout << "Primer valor de f_limit: " << f_limit << endl;
    
    while (true){
        dfs_output dfs_solution = dfs_contour(root, f_limit, f);
        f_limit = dfs_solution.f_limit;
        if(!dfs_solution.isNull){
            return dfs_solution.solution;
        }
        if(dfs_solution.f_limit == numeric_limits<double>::infinity()){
            throw runtime_error("No se consiguió solución");
        }
    }

}

void print_vector(vector<int> &n){

    cout << "[";
    int i = 0;
    while(i < N){
        if(i == N - 1){
            cout << n[i];
        } else {
            cout << n[i] << ", ";
        }
        i++;
    }

    cout << "]" << endl;

}

void reconstruct_solution(vector<int> partial_vector, ofstream& output_file){

    node partial_solution = {partial_vector};
    print_state(partial_solution, output_file); 

    for(int rot : path){

        int pos_change = pos_one + rot;
        if(pos_change >= N){
            pos_change -= N;
        }

        int movement = movements_to_zero(pos_change);

        int iter_movement = movement;
        while(iter_movement != 0){
            if(iter_movement > 0){
                rotate(partial_solution.state.rbegin(), partial_solution.state.rbegin() + 1, partial_solution.state.rend());
                iter_movement -= 1;
            } else {
                rotate(partial_solution.state.begin(), partial_solution.state.begin() + 1, partial_solution.state.end());
                iter_movement += 1;
            }
            path_length++;
            print_state(partial_solution, output_file);
        }

        int new_pos_one = pos_one + movement;

        if(new_pos_one >= N){
            new_pos_one -= N;
        } else if (new_pos_one < 0){
            new_pos_one += N;
        }

        int special_value = N - k + 1;

        if(rot == 0 || (rot >= special_value)){
            new_pos_one = new_pos_one + movements_in_window[rot];
            if(new_pos_one >= N){
                new_pos_one -= N;
            } else if (new_pos_one < 0){
                new_pos_one += N;
            }
            pos_one = new_pos_one;
        } else {
            pos_one = new_pos_one;
        }

        partial_solution = apply_rotation(partial_solution, 0);
        path_length++;
        print_state(partial_solution, output_file);

    }

    if(partial_solution.state[0] != 1){

        int movement = movements_to_zero(pos_one);

        int iter_movement = movement;
        while(iter_movement != 0){
            if(iter_movement > 0){
                rotate(partial_solution.state.rbegin(), partial_solution.state.rbegin() + 1, partial_solution.state.rend());
                iter_movement -= 1;
            } else {
                rotate(partial_solution.state.begin(), partial_solution.state.begin() + 1, partial_solution.state.end());
                iter_movement += 1;
            }
            path_length++;
            print_state(partial_solution, output_file);
        }

    }

}


int main(int argc, char* argv[]){

    // Inicialización de un mapa que ayuda a calcular los movimientos que afectan al número 1 en la ventana
    int iter = 1;
    int descend = N - 1;
    int descend_by_two = k - 1; 
    movements_in_window[0] = descend_by_two;
    descend_by_two -= 2;
    while(iter < k){
        movements_in_window[descend] = descend_by_two;
        descend--;
        descend_by_two -= 2;
        iter++;
    }

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
        output_file.close();
        return 1;
    }

    try{
        solution = ida(initial, f);
    } catch (...){
        cerr << "No fue encontrada una solución." << endl;
        output_file.close();
        return 1;
    }

    reconstruct_solution(initial, output_file);
    cout << "Longitud del camino desde el estado inicial hasta la meta: " << path_length + 1 << endl;
    cout << "Estados generados: " << generated_states << endl;

    output_file.close();
}

