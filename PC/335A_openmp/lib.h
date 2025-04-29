#pragma once
#include <omp.h>

/// <summary>
/// Вычисляет сумму ряда по условию
/// </summary>
/// <param name="n"> Сумма итерируется от 1 до n</param>
/// <returns>Сумма ряда</returns>
unsigned long long parallel_sum(unsigned int n) {
    unsigned long long sum = 0;
    
    // #pragma omp parallel - создает параллельную область, где несколько потоков выполняют один и тот же блок кода.
    // В данном случае по потокам распределяются итерации цикла for. Итерации делятся примерно поровну между потоками.
    // Число потоков либо равно числу физ ядер, либо установлено функцией omp_set_num_threads()
    // reduciton - каждый поток создает локальную копию sum, после завершения всех потоков локальные суммируются
    #pragma omp parallel for reduction(+:sum)
    for (int k = 1; k <= n; k++) {
        unsigned long long temp = 1;

        for (int i = k; i <= k * k; i++) {
            temp *= i;
        }

        sum += temp;
    }

    return sum;
}