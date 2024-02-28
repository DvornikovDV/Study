// Дворников Даниил ИВТ-22

#include "arr_alg_headers.h"

int main()
{
    test();

    srand(time(NULL));
    
    
    size_t N = 1'000'000;
    int* a = new int[N];
    int value{};

    fill_arr_rand(a, N, 1'000'000, 0);

    const auto start_time{ steady_clock::now() };

    int iter{100};
    for (int j{}; j < iter; j++) {
        value = rand() % 1'000'000;
        incremental_search(a, value, N);
        //cout <<  << "\t";
    }
    const auto end_time{ steady_clock::now() };
    const duration<double> elapsed_s{ end_time - start_time };

    cout << elapsed_s / iter << '\n';
    
    delete[] a;
}