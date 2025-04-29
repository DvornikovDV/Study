#pragma once
#include <omp.h>

/// <summary>
/// ��������� ����� ���� �� �������
/// </summary>
/// <param name="n"> ����� ����������� �� 1 �� n</param>
/// <returns>����� ����</returns>
unsigned long long parallel_sum(unsigned int n) {
    unsigned long long sum = 0;
    
    // #pragma omp parallel - ������� ������������ �������, ��� ��������� ������� ��������� ���� � ��� �� ���� ����.
    // � ������ ������ �� ������� �������������� �������� ����� for. �������� ������� �������� ������� ����� ��������.
    // ����� ������� ���� ����� ����� ��� ����, ���� ����������� �������� omp_set_num_threads()
    // reduciton - ������ ����� ������� ��������� ����� sum, ����� ���������� ���� ������� ��������� �����������
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