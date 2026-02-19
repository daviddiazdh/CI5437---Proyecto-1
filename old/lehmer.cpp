#include <iostream>
#include <vector>

using namespace std;

static const int N = 12;
long long factorial[N + 1];
long long C[N+1][N+1];
vector<int> pattern = {1,2,3,4};
int pattern_size = pattern.size();

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

    print_vector(perm);
    print_vector(positions);

    long long lehmer_index = lehmer_rank(perm);
    long long combinadic_index = combinadic_rank(positions);

    cout << "lehmer_index: " << lehmer_index << endl;
    cout << "combinadic_index: " << combinadic_index << endl;

    return lehmer_index + combinadic_index * factorial[pattern_size];

}

int main(){

    init_factorials(N);
    init_combinations(N);

    vector<int> state = {1, 3, 0, 2, 4, 0, 0, 0};

    long long index = find_index(state);

    cout << "Index: " << index << endl; 


    return 0;
}