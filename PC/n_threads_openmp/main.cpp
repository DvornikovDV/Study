#include <iostream>
#include <vector>
#include <string>
#include "lib.h"

int main() {
    size_t numThreads;

    // Запрашиваем количество потоков
    std::cout << "Enter the number of threads (max 16): ";
    if (!(std::cin >> numThreads) || numThreads < 0 || numThreads > 16) {
        std::cerr << "Error: Invalid number of threads. Must be between 1 and 16.\n";
        return 1;
    }

    // Запускаем тестирование производительности
    testSortPerformance(numThreads);

    std::cout << "All tests completed successfully.\n";
    return 0;
}

/*
    size_t numThreads;
    std::string inputFile, outputFile;

    std::cout << "Enter the number of threads (max 16): ";
    if (!(std::cin >> numThreads) || numThreads < 1 || numThreads > 16) {
        std::cerr << "Error: Invalid number of threads. Must be between 1 and 16.\n";
        return 1;
    }

    std::cout << "Enter input CBOR file name (or press Enter to generate random array): ";
    std::cin.ignore(); // Очищает буфер
    std::getline(std::cin, inputFile);


    std::cout << "Enter output CBOR file name: ";
    std::getline(std::cin, outputFile);
    if (outputFile.empty()) {
        std::cerr << "Error: Output file name cannot be empty.\n";
        return 1;
    }
    outputFile.append(".cbor");

    std::vector<float> arr;
    if (inputFile.empty()) {
        // случайный инт массив для демонстрации
        size_t size;
        std::cout << "Enter the number of elements: ";
        if (!(std::cin >> size) || size < 1) {
            std::cerr << "Error: Invalid number of elements. Must be positive.\n";
            return 1;
        }
        arr = generateRandomArray<float>(size);
    }
    else {
        // Массив численного типа из файла
        inputFile.append(".cbor");
        if (!readArrayMsgpack(arr, inputFile)) {
            std::cerr << "Error: Failed to read array from " << inputFile << ".\n";
            return 1;
        }
    }

    auto startSort = std::chrono::high_resolution_clock::now();
    parallelMergeSort(arr, numThreads);
    auto endSort = std::chrono::high_resolution_clock::now();

    if (!writeArrayMsgpack(arr, outputFile)) {
        std::cerr << "Error: Failed to write array to " << outputFile << ".\n";
        return 1;
    }
    std::cout << "File write succesfully.\n";

    std::cout << "Total time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(endSort - startSort).count()
        << " ms\n";
    std::cout << "Sorting completed successfully.\n";

    return 0;
    
    
    
    
*/