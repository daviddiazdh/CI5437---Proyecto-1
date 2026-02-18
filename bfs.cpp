#include <iostream>
#include <vector>
#include <numeric>
#include <queue>
#include <algorithm>

using namespace std;


struct node{
    vector<int> state;
    double cost = 0;
    char last_movement = 'i';
};

int N = 8;
int k = 4;

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

bool are_equal_vector (vector<int> &a, vector<int> &b){

    int i = 0;

    while(i < N){
        if(a[i] != b[i]){
            return false;
        }
        i++;
    }

    return true;

}

bool is_in (vector<int> &a, vector<vector<int>> &list){
    bool found = false;
    for (int i = 0; i < list.size(); i++){
        if (are_equal_vector(a, list[i])){
            found = true;
        }
    }
    return found;
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

void breadth_first_search (vector<int> initial) {

    node iter_node = { initial, 0 };
    queue<node> frontier;
    vector<vector<int>> reached;
    vector<int> s;

    frontier.push(iter_node);
    reached.push_back(initial);

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
            if (!is_in(s, reached)){
                // cout << "El sucesor no está en reached" << endl;
                reached.push_back(s);
                frontier.push(child);
            }
            // cout << "-----------------------------" << endl;
        }
    }
    return;
}

int main(){

    // Definición de estado meta:
    vector<int> state (N);
    iota(state.begin(), state.end(), 1);
    breadth_first_search(state);

    return 0;
}