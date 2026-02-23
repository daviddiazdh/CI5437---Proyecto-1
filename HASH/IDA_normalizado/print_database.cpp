#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

int N = 12;
int k = 4;

vector<long long> factorial(N + 1);
vector<vector<long long>> C(N+1, vector<long long>(N+1));
vector<int> pattern = {1,2,3,4,5,6};
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

    long long total_size = C[N - 1][pattern_size - 1] * factorial[pattern_size - 1];

    cout << "total_size: " << total_size; 

    const char* filename = "pdb.bin";

    ifstream input_bin(filename, ios::binary | ios::in | ios::out);

    if (!input_bin) {
        cout << "Error al crear archivo\n";
        return 1;
    }
    long long value;
    long long max_value = 0;
    for (long long i = 0; i < total_size; i++){
        input_bin.read(reinterpret_cast<char*>(&value),
                        sizeof(long long));
        if(value > max_value){
            max_value = value;
        }
        cout << i << ": " <<  value << endl;
    }

    cout << "Costo mayor: " << max_value << endl; 
    
    input_bin.close();

}