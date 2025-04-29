#pragma once
#include <vector>
#include <thread>
#include <random>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <cstring>
#include <fstream>
#include <msgpack.hpp>

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
/// Сортирует часть массива в отдельном потоке.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="arr">Исходный вектор для сортировки.</param>
/// <param name="left">Индекс начала диапазона.</param>
/// <param name="right">Индекс конца диапазона.</param>
/// <param name="result">Вектор для записи результата.</param>
template <typename T>
void threadTask(std::vector<T>& arr, size_t left, size_t right, std::vector<T>& result) {
    // Создаёт локальный буфер для копии части массива
    std::vector<T> localBuffer(right - left + 1);
    // Копирует данные из исходного массива
    std::memcpy(localBuffer.data(), arr.data() + left, (right - left + 1) * sizeof(T));
    // Создаёт временный вектор для слияния
    std::vector<T> temp(localBuffer.size());
    // Сортирует локальный буфер
    mergeSort(localBuffer, 0, localBuffer.size() - 1, temp);
    // Копирует отсортированный результат обратно
    std::memcpy(result.data() + left, localBuffer.data(), localBuffer.size() * sizeof(T));
}

/// <summary>
/// Выполняет многопоточную сортировку слиянием с последовательным слиянием частей.
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

    // Ограничивает количество потоков
    numThreads = std::min({ numThreads, n, size_t(16) });
    // Создаёт вектор для хранения потоков
    std::vector<std::thread> threads;
    // Вычисляет размер части для каждого потока, с округлением в большую сторону
    size_t chunkSize = (n + numThreads - 1) / numThreads;

    // Замеряет время сортировки
    auto startSort = std::chrono::high_resolution_clock::now();
    // Запускает потоки для сортировки частей массива
    for (size_t i = 0; i < numThreads; ++i) {
        size_t left = i * chunkSize;
        // Наименьшее из - левый индекс + чанк \ последний индекс массива
        size_t right = std::min(left + chunkSize - 1, n - 1);
        if (left < n) {
            // Создаёт объект напрямую в памяти вектора, принимает указатель на функцию(автоматически преобразуется от имени) и параметры для нее. Пишет и читает 1 массивом 
            threads.emplace_back(threadTask<T>, std::ref(arr), left, right, std::ref(arr));
        }
    }

    // Ожидает завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }
    auto endSort = std::chrono::high_resolution_clock::now();

    // Замеряет время слияния
    auto startMerge = std::chrono::high_resolution_clock::now();
    // Создаёт временный вектор для слияния
    std::vector<T> temp(n);
    size_t currentSize = chunkSize;
    // Последовательно сливает отсортированные части
    while (currentSize < n) {
        for (size_t i = 0; i < n; i += currentSize * 2) {
            size_t left = i;
            size_t mid = std::min(i + currentSize - 1, n - 1);
            size_t right = std::min(i + 2 * currentSize - 1, n - 1);
            if (mid < right) {
                // Сливает две части массива
                merge(arr, left, mid, right, temp);
            }
        }
        currentSize *= 2; // Удваивает размер сливаемых частей
    }
    auto endMerge = std::chrono::high_resolution_clock::now();

    // Выводит время сортировки и слияния
    std::cout << "Sorting time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(endSort - startSort).count()
        << " ms\n";
    std::cout << "Merging time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(endMerge - startMerge).count()
        << " ms\n";
}

/// <summary>
/// Генерирует массив случайных чисел в диапазоне [-100, 100].
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="size">Размер генерируемого массива.</param>
/// <returns>Вектор случайных чисел типа T.</returns>
template <typename T>
std::vector<T> generateRandomArray(size_t size) {
    // Инициализирует генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    // Создаёт вектор заданного размера
    std::vector<T> arr(size);

    if constexpr (std::is_integral_v<T>) {
        // Генерирует целые числа в диапазоне [-100, 100]
        std::uniform_int_distribution<> dis(-100, 100);
        for (size_t i = 0; i < size; ++i) {
            arr[i] = dis(gen); // Заполняет массив случайными целыми числами
        }
    }
    else if constexpr (std::is_floating_point_v<T>) {
        // Генерирует вещественные числа в диапазоне [-100, 100]
        std::uniform_real_distribution<> dis(-100.0, 100.0);
        for (size_t i = 0; i < size; ++i) {
            arr[i] = dis(gen); // Заполняет массив случайными вещественными числами
        }
    }

    return arr;
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
    // ""Оптимизация и упрощение"'
    catch (const std::exception& e) {
        std::cerr << "Error reading Msgpack from " << filename << ": " << e.what() << "\n";
        return false;
    }
}

/// <summary>
/// Тестирует производительность многопоточной сортировки для массивов разного размера.
/// </summary>
/// <param name="numThreads">Количество потоков.</param>
void testSortPerformance(size_t numThreads) {
    // Определяет размеры массивов для тестирования
    const std::vector<size_t> sizes = { 30000000, 50000000, 60000000 };
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
/// Проверяет, отсортирован ли массив по неубыванию.
/// </summary>
/// <typeparam name="T">Любой численный тип (int, float)</typeparam>
/// <param name="arr">Вектор для проверки.</param>
/// <returns>true, если массив отсортирован, иначе false.</returns>
template <typename T>
bool isSorted(const std::vector<T>& arr) {
    // Проверяет, что каждый элемент не меньше предыдущего
    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] < arr[i - 1]) {
            return false;
        }
    }
    return true;
}