#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

int N = 12;
int k = 4;

vector<long long> factorial(N + 1);
vector<vector<long long>> C(N+1, vector<long long>(N+1));
vector<int> pattern = {1,3,5,7,9,11};
int pattern_size = pattern.size();

void init_factorials(int n) {
    factorial[0] = 1;
    for (int i = 1; i <= n; i++)
        factorial[i] = factorial[i - 1] * i;
}

void init_combinations(int n) {
    for (int i = 0; i <= n; i++) {
        C[i][0] = C[i][i] = 1;
        for (int j = 1; j < i; j++)
            C[i][j] = C[i-1][j-1] + C[i-1][j];
    }
}

int main(){
    init_factorials(N);
    init_combinations(N);

    long long total_size = C[N][pattern_size] * factorial[pattern_size];

    const char* filename = "pdb.bin";

    ifstream input_bin(filename, ios::binary | ios::in | ios::out);

    if (!input_bin) {
        cout << "Error al crear archivo\n";
        return 1;
    }
    long long value;
    for (long long i = 0; i < total_size; i++){
        input_bin.read(reinterpret_cast<char*>(&value),
                        sizeof(long long));
        cout << i << ": " <<  value << endl;
    }
    
    input_bin.close();

}