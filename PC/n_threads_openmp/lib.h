#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <cstring>
#include <fstream>
#include <msgpack.hpp>
#include <omp.h>

/// <summary>
/// Сливает два отсортированных подмассива в один отсортированный массив.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="arr">Вектор, содержащий подмассивы для слияния.</param>
/// <param name="left">Индекс начала первого подмассива.</param>
/// <param name="mid">Индекс конца первого подмассива.</param>
/// <param name="right">Индекс конца второго подмассива.</param>
/// <param name="temp">Временный вектор для хранения промежуточных результатов.</param>
template <typename T>
void merge(std::vector<T>& arr, size_t left, size_t mid, size_t right, std::vector<T>& temp) {
    size_t i = left, j = mid + 1, k = left;

    // Сравнивает элементы подмассивов и помещает меньший во временный вектор
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++]; // Копирует элемент из первого подмассива
        }
        else {
            temp[k++] = arr[j++]; // Копирует элемент из второго подмассива
        }
    }

    // Копирует оставшиеся элементы первого подмассива
    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    // Копирует оставшиеся элементы второго подмассива
    while (j <= right) {
        temp[k++] = arr[j++];
    }

    // Копирует отсортированный результат обратно в исходный массив
    std::memcpy(&arr[left], &temp[left], (right - left + 1) * sizeof(T));
}

/// <summary>
/// Выполняет рекурсивную сортировку слиянием для заданного диапазона массива.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="arr">Вектор для сортировки.</param>
/// <param name="left">Индекс начала диапазона.</param>
/// <param name="right">Индекс конца диапазона.</param>
/// <param name="temp">Временный вектор для слияния.</param>
template <typename T>
void mergeSort(std::vector<T>& arr, size_t left, size_t right, std::vector<T>& temp) {
    if (left < right) {
        // Вычисляет середину диапазона
        size_t mid = left + (right - left) / 2;
        // Рекурсивно сортирует левую половину
        mergeSort(arr, left, mid, temp);
        // Рекурсивно сортирует правую половину
        mergeSort(arr, mid + 1, right, temp);
        // Сливает отсортированные половины
        merge(arr, left, mid, right, temp);
    }
}

/// <summary>
/// Выполняет однопоточную сортировку слиянием всего массива.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="arr">Вектор для сортировки.</param>
template <typename T>
void singleThreadMergeSort(std::vector<T>& arr) {
    if (arr.empty()) return; // Пропускает пустой массив
    // Создаёт временный вектор для слияния
    std::vector<T> temp(arr.size());
    // Запускает рекурсивную сортировку
    mergeSort(arr, 0, arr.size() - 1, temp);
}

/// <summary>
/// Выполняет многопоточную сортировку слиянием с использованием OpenMP.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="arr">Вектор для сортировки.</param>
/// <param name="numThreads">Количество потоков.</param>
/// <summary>
/// Выполняет многопоточную сортировку слиянием с использованием OpenMP.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="arr">Вектор для сортировки.</param>
/// <param name="numThreads">Количество потоков.</param>
template <typename T>
void parallelMergeSort(std::vector<T>& arr, size_t numThreads) {
    size_t n = arr.size();
    if (n == 0) return; // Пропускает пустой массив

    if (numThreads <= 1) {
        // Использует однопоточную сортировку для одного потока
        singleThreadMergeSort(arr);
        return;
    }

    // Устанавливает количество потоков для OpenMP
    omp_set_num_threads(static_cast<int>(numThreads));
    // Вычисляет размер части для каждого потока
    size_t chunkSize = (n + numThreads - 1) / numThreads;
    std::cout << "Number of threads: " << omp_get_max_threads() << "\n";

    // Предварительно выделяет буферы
    std::vector<std::vector<T>> localBuffers(numThreads);
    std::vector<std::vector<T>> tempBuffers(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        size_t left = i * chunkSize;
        size_t right = std::min(left + chunkSize - 1, n - 1);
        if (left < n) {
            localBuffers[i].resize(right - left + 1);
            tempBuffers[i].resize(right - left + 1);
        }
    }
    std::vector<T> temp(n);

    // Создаёт пул потоков и распределяет итерации цикла между ними для параллельной сортировки частей массива.
    #pragma omp parallel
    {
        size_t threadId = omp_get_thread_num();
        size_t left = threadId * chunkSize;
        size_t right = std::min(left + chunkSize - 1, n - 1);
        if (left < n && threadId < numThreads) {
            // Копирует данные в локальный буфер
            std::memcpy(localBuffers[threadId].data(), arr.data() + left, (right - left + 1) * sizeof(T));
            // Сортирует локальный буфер
            mergeSort(localBuffers[threadId], 0, localBuffers[threadId].size() - 1, tempBuffers[threadId]);
            // Копирует результат обратно
            std::memcpy(arr.data() + left, localBuffers[threadId].data(), localBuffers[threadId].size() * sizeof(T));
        }

        // Синхронизация перед слиянием
        #pragma omp barrier

        // Параллельное слияние
        #pragma omp single nowait
        {
            size_t currentSize = chunkSize;
            while (currentSize < n) {
                for (size_t i = 0; i < n; i += currentSize * 2) {
                    size_t left = i;
                    size_t mid = std::min(i + currentSize - 1, n - 1);
                    size_t right = std::min(i + 2 * currentSize - 1, n - 1);
                    if (mid < right) {
                        #pragma omp task
                        merge(arr, left, mid, right, temp);
                    }
                }
                #pragma omp taskwait
                currentSize *= 2; // Удваивает размер сливаемых частей
            }
        }
    }
}

/// <summary>
/// Генерирует массив случайных чисел в диапазоне.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="size">Размер генерируемого массива.</param>
/// <returns>Вектор случайных чисел типа T.</returns>
template <typename T>
std::vector<T> generateRandomArray(size_t size) {
    // Создаёт вектор заданного размера
    std::vector<T> arr(size);

    // Однопоточная генерация для малых массивов
    std::random_device rd;
    std::mt19937 gen(rd());
    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<> dis(-100, 100);
        for (size_t i = 0; i < size; ++i) {
            arr[i] = dis(gen);
        }
    }
    else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<> dis(-100.0, 100.0);
        for (size_t i = 0; i < size; ++i) {
            arr[i] = dis(gen);
        }
    }

    return arr;
}

/// <summary>
/// Тестирует производительность многопоточной сортировки для массивов разного размера.
/// </summary>
/// <param name="numThreads">Количество потоков.</param>
void testSortPerformance(size_t numThreads) {
    // Определяет размеры массивов для тестирования
    const std::vector<size_t> sizes = { 5000000, 10000000, 20000000, 30000000, 40000000, 50000000, 60000000, 80000000 };
    for (size_t size : sizes) {
        // Генерирует случайный массив
        std::vector<int> arr = generateRandomArray<int>(size);
        std::cout << "Array size: " << size << ", Threads: " << numThreads << "\n";
        // Замеряет общее время сортировки
        auto totalStart = std::chrono::high_resolution_clock::now();
        // Выполняет сортировку
        parallelMergeSort(arr, numThreads);
        auto totalEnd = std::chrono::high_resolution_clock::now();
        // Выводит общее время
        std::cout << "Total time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count()
            << " ms\n";
        std::cout << "------------------------\n";
    }
}



/// <summary>
/// Записывает массив в файл в формате MessagePack.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="arr">Вектор для записи.</param>
/// <param name="filename">Имя файла для записи.</param>
/// <returns>true, если запись успешна, иначе false.</returns>
template <typename T>
bool writeArrayMsgpack(const std::vector<T>& arr, const std::string& filename) {
    try {
        // Открывает файл для записи в бинарном режиме
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << " for writing.\n";
            return false;
        }

        // Создаёт буфер для оптимизации записи
        constexpr size_t bufferSize = 1048576;
        std::vector<char> buffer(bufferSize);
        ofs.rdbuf()->pubsetbuf(buffer.data(), bufferSize);

        // Замеряет время записи
        auto start = std::chrono::high_resolution_clock::now();
        // Создаёт буфер MessagePack
        msgpack::sbuffer sbuf;
        msgpack::packer<msgpack::sbuffer> pk(&sbuf);
        // Записывает map с одним ключом "array"
        pk.pack_map(1);
        pk.pack(std::string("array"));
        // Записывает массив
        pk.pack_array(arr.size());
        for (const auto& value : arr) {
            pk.pack(value); // Сериализует каждый элемент
        }
        // Записывает данные в файл
        ofs.write(sbuf.data(), sbuf.size());
        // Закрывает файл
        ofs.close();

        // Выводит время записи
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Msgpack write time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << " ms\n";
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing Msgpack to " << filename << ": " << e.what() << "\n";
        return false;
    }
}

/// <summary>
/// Читает массив из файла в формате MessagePack.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="arr">Вектор для хранения прочитанных данных.</param>
/// <param name="filename">Имя файла для чтения.</param>
/// <returns>true, если чтение успешно, иначе false.</returns>
template <typename T>
bool readArrayMsgpack(std::vector<T>& arr, const std::string& filename) {
    try {
        // Открывает файл для чтения в бинарном режиме
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << " for reading.\n";
            return false;
        }
        // Создаёт буфер для оптимизации чтения
        constexpr size_t bufferSize = 1048576;
        std::vector<char> buffer(bufferSize);
        ifs.rdbuf()->pubsetbuf(buffer.data(), bufferSize);
        // Замеряет время чтения
        auto start = std::chrono::high_resolution_clock::now();
        // Определяет размер файла
        ifs.seekg(0, std::ios::end);
        size_t fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        // Создаёт буфер для чтения файла
        std::vector<char> fileBuffer(fileSize);
        // Читает данные из файла
        ifs.read(fileBuffer.data(), fileSize);
        // Закрывает файл
        ifs.close();
        // Распаковывает данные MessagePack
        msgpack::object_handle oh = msgpack::unpack(fileBuffer.data(), fileSize);
        msgpack::object obj = oh.get();
        // Проверяет формат: map с одним ключом
        if (obj.type != msgpack::type::MAP || obj.via.map.size != 1) {
            std::cerr << "Error: Invalid Msgpack format. Expected map with one key.\n";
            return false;
        }
        auto& kv = obj.via.map.ptr[0];
        // Проверяет ключ "array" и тип значения (массив)
        if (kv.key.as<std::string>() != "array" || kv.val.type != msgpack::type::ARRAY) {
            std::cerr << "Error: Expected key 'array' with array value.\n";
            return false;
        }
        auto& array = kv.val.via.array;
        // Очищает и резервирует место для массива
        arr.clear();
        arr.reserve(array.size);
        // Читает элементы массива
        for (size_t i = 0; i < array.size; ++i) {
            if constexpr (std::is_integral_v<T>) {
                // Проверяет, что элемент — целое число
                if (array.ptr[i].type != msgpack::type::POSITIVE_INTEGER &&
                    array.ptr[i].type != msgpack::type::NEGATIVE_INTEGER) {
                    std::cerr << "Error: Msgpack array contains non-integer value at index " << i << ".\n";
                    return false;
                }
                arr.push_back(array.ptr[i].as<T>()); // Конвертирует в тип T
            }
            else if constexpr (std::is_floating_point_v<T>) {
                // Проверяет, что элемент — float или целое число
                if (array.ptr[i].type != msgpack::type::FLOAT32 &&
                    array.ptr[i].type != msgpack::type::POSITIVE_INTEGER &&
                    array.ptr[i].type != msgpack::type::NEGATIVE_INTEGER) {
                    std::cerr << "Error: Msgpack array contains non-float value at index " << i << ".\n";
                    return false;
                }
                arr.push_back(array.ptr[i].as<T>()); // Конвертирует в тип T
            }
        }
        // Выводит время чтения
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Msgpack read time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << " ms\n";
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading Msgpack from " << filename << ": " << e.what() << "\n";
        return false;
    }
}

/// <summary>
/// Проверяет, отсортирован ли массив по неубыванию.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="arr">Вектор для проверки.</param>
/// <returns>true, если массив отсортирован, иначе false.</returns>
template <typename T>
bool isSorted(const std::vector<T>& arr) {
    bool result = true;
    // Однопоточная проверка для малых массивов
    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] < arr[i - 1]) {
            return false;
        }
    }
    return result;
}