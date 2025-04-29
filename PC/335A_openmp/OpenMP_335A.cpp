#include <iostream>
#include "lib.h"

/*

https://ivtipm.github.io/Programming/Glava10/index10.htm#z335

*/

int main() {
    long long n;
    std::cout << "Enter the value of n: ";
    std::cin >> n;

    // Проверка корректности ввода
    if (n < 1) {
        std::cout << "n must be at least 1\n";
        return 1;
    }

    // Вычисление суммы
    unsigned long long result = parallel_sum(n);
    std::cout << "The sum is: " << result << "\n";

    return 0;
}