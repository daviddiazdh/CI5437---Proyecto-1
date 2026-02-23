#include <iostream>
#include <vector>
#include <numeric>
#include <queue>
#include <algorithm>
#include <fstream>

using namespace std;


struct node{
    vector<int> state;
    long long cost = 0;
    char last_movement = 'i';
};

int N = 12;
int k = 4;

vector<long long> factorial(N + 1);
vector<vector<long long>> C(N+1, vector<long long>(N+1));
vector<int> pattern = {1,3,5,7,9,11};
int pattern_size = pattern.size();


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


void print_vector(vector<int> &n){

    cout << "[";
    int i = 0;
    while(i < n.size()){
        if(i == n.size() - 1){
            cout << n[i];
        } else {
            cout << n[i] << ", ";
        }
        i++;
    }

    cout << "]" << endl;

}



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


void breadth_first_search (vector<int> initial, ofstream &output_bin) {

    node iter_node = { initial, 0 };
    queue<node> frontier;
    // vector<vector<int>> reached;
    long long total_size = C[N][pattern_size] * factorial[pattern_size];
    vector<bool> visited(total_size, false);
    vector<int> s;

    frontier.push(iter_node);
    visited[0] = true;
    // reached.push_back(initial);

    while (!frontier.empty()){
        iter_node = frontier.front();
        frontier.pop();

        // cout << "Verificamos los sucesores de" << endl;
        // print_vector(iter_node.state);

        for (node child : succesors(iter_node)){
            // cout << "Sucesor: ";
            // print_vector(child.state);
            s = child.state;
            //Verificamos que s no esté en reached
            long long index = find_index(s);

            if (!visited[index]){

                visited[index] = true;

                output_bin.seekp(index * sizeof(long long), ios::beg);
                output_bin.write(reinterpret_cast<char*>(&child.cost), sizeof(long long));

                frontier.push(child);
            }
            // cout << "-----------------------------" << endl;
        }
    }
    return;
}

int main(){

    init_factorials(N);
    init_combinations(N);

    // Definición de estado meta:
    vector<int> initial_state (N);

    const char* filename = "pdb.bin";

    long long total_size = C[N][pattern_size] * factorial[pattern_size];

    ofstream output_bin(filename, ios::binary | ios::in | ios::out);

    if (!output_bin) {
        cout << "Error al crear archivo\n";
        return 1;
    }
    
    long long zero = 0;

    for (long long i = 0; i < total_size; i++)
        output_bin.write(reinterpret_cast<char*>(&zero),
                        sizeof(long long));

    ifstream input_bin(filename, ios::binary);

    if (!input_bin) {
        cout << "Error al abrir archivo\n";
        return 1;
    }

    for(int i = 0; i < N; i++){
        for(int j = 0; j < pattern_size; j++){
            if(pattern[j] == i + 1){
                initial_state[i] = i + 1;
                break;
            } else {
                initial_state[i] = 0;
            }
        }
    }

    print_vector(initial_state);

    breadth_first_search(initial_state, output_bin);

    output_bin.close();

    // long long index = find_index({1, 4, 5, 2, 6, 3, 7, 8});
    // long long cost;

    // input_bin.seekg(index * sizeof(long long), ios::beg);

    // input_bin.read(reinterpret_cast<char*>(&cost), sizeof(long long));

    // cout << "Costo: " << cost << endl;

    input_bin.close();

    return 0;
}