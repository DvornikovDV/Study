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
/// ������� ��� ��������������� ���������� � ���� ��������������� ������.
/// </summary>
/// <typeparam name="T">����� ��������� ��� (int, float)</typeparam>
/// <param name="arr">������, ���������� ���������� ��� �������.</param>
/// <param name="left">������ ������ ������� ����������.</param>
/// <param name="mid">������ ����� ������� ����������.</param>
/// <param name="right">������ ����� ������� ����������.</param>
/// <param name="temp">��������� ������ ��� �������� ������������� �����������.</param>
template <typename T>
void merge(std::vector<T>& arr, size_t left, size_t mid, size_t right, std::vector<T>& temp) {
    size_t i = left, j = mid + 1, k = left;

    // ���������� �������� ����������� � �������� ������� �� ��������� ������
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++]; // �������� ������� �� ������� ����������
        }
        else {
            temp[k++] = arr[j++]; // �������� ������� �� ������� ����������
        }
    }

    // �������� ���������� �������� ������� ����������
    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    // �������� ���������� �������� ������� ����������
    while (j <= right) {
        temp[k++] = arr[j++];
    }

    // �������� ��������������� ��������� ������� � �������� ������
    std::memcpy(&arr[left], &temp[left], (right - left + 1) * sizeof(T));
}

/// <summary>
/// ��������� ����������� ���������� �������� ��� ��������� ��������� �������.
/// </summary>
/// <typeparam name="T">����� ��������� ��� (int, float)</typeparam>
/// <param name="arr">������ ��� ����������.</param>
/// <param name="left">������ ������ ���������.</param>
/// <param name="right">������ ����� ���������.</param>
/// <param name="temp">��������� ������ ��� �������.</param>
template <typename T>
void mergeSort(std::vector<T>& arr, size_t left, size_t right, std::vector<T>& temp) {
    if (left < right) {
        // ��������� �������� ���������
        size_t mid = left + (right - left) / 2;
        // ���������� ��������� ����� ��������
        mergeSort(arr, left, mid, temp);
        // ���������� ��������� ������ ��������
        mergeSort(arr, mid + 1, right, temp);
        // ������� ��������������� ��������
        merge(arr, left, mid, right, temp);
    }
}

/// <summary>
/// ��������� ������������ ���������� �������� ����� �������.
/// </summary>
/// <typeparam name="T">����� ��������� ��� (int, float)</typeparam>
/// <param name="arr">������ ��� ����������.</param>
template <typename T>
void singleThreadMergeSort(std::vector<T>& arr) {
    if (arr.empty()) return; // ���������� ������ ������
    // ������ ��������� ������ ��� �������
    std::vector<T> temp(arr.size());
    // ��������� ����������� ����������
    mergeSort(arr, 0, arr.size() - 1, temp);
}

/// <summary>
/// ��������� ����� ������� � ��������� ������.
/// </summary>
/// <typeparam name="T">����� ��������� ��� (int, float)</typeparam>
/// <param name="arr">�������� ������ ��� ����������.</param>
/// <param name="left">������ ������ ���������.</param>
/// <param name="right">������ ����� ���������.</param>
/// <param name="result">������ ��� ������ ����������.</param>
template <typename T>
void threadTask(std::vector<T>& arr, size_t left, size_t right, std::vector<T>& result) {
    // ������ ��������� ����� ��� ����� ����� �������
    std::vector<T> localBuffer(right - left + 1);
    // �������� ������ �� ��������� �������
    std::memcpy(localBuffer.data(), arr.data() + left, (right - left + 1) * sizeof(T));
    // ������ ��������� ������ ��� �������
    std::vector<T> temp(localBuffer.size());
    // ��������� ��������� �����
    mergeSort(localBuffer, 0, localBuffer.size() - 1, temp);
    // �������� ��������������� ��������� �������
    std::memcpy(result.data() + left, localBuffer.data(), localBuffer.size() * sizeof(T));
}

/// <summary>
/// ��������� ������������� ���������� �������� � ���������������� �������� ������.
/// </summary>
/// <typeparam name="T">����� ��������� ��� (int, float)</typeparam>
/// <param name="arr">������ ��� ����������.</param>
/// <param name="numThreads">���������� �������.</param>
template <typename T>
void parallelMergeSort(std::vector<T>& arr, size_t numThreads) {
    size_t n = arr.size();
    if (n == 0) return; // ���������� ������ ������

    if (numThreads <= 1) {
        // ���������� ������������ ���������� ��� ������ ������
        singleThreadMergeSort(arr);
        return;
    }

    // ������������ ���������� �������
    numThreads = std::min({ numThreads, n, size_t(16) });
    // ������ ������ ��� �������� �������
    std::vector<std::thread> threads;
    // ��������� ������ ����� ��� ������� ������, � ����������� � ������� �������
    size_t chunkSize = (n + numThreads - 1) / numThreads;

    // �������� ����� ����������
    auto startSort = std::chrono::high_resolution_clock::now();
    // ��������� ������ ��� ���������� ������ �������
    for (size_t i = 0; i < numThreads; ++i) {
        size_t left = i * chunkSize;
        // ���������� �� - ����� ������ + ���� \ ��������� ������ �������
        size_t right = std::min(left + chunkSize - 1, n - 1);
        if (left < n) {
            // ������ ������ �������� � ������ �������, ��������� ��������� �� �������(������������� ������������� �� �����) � ��������� ��� ���. ����� � ������ 1 �������� 
            threads.emplace_back(threadTask<T>, std::ref(arr), left, right, std::ref(arr));
        }
    }

    // ������� ���������� ���� �������
    for (auto& t : threads) {
        t.join();
    }
    auto endSort = std::chrono::high_resolution_clock::now();

    // �������� ����� �������
    auto startMerge = std::chrono::high_resolution_clock::now();
    // ������ ��������� ������ ��� �������
    std::vector<T> temp(n);
    size_t currentSize = chunkSize;
    // ��������������� ������� ��������������� �����
    while (currentSize < n) {
        for (size_t i = 0; i < n; i += currentSize * 2) {
            size_t left = i;
            size_t mid = std::min(i + currentSize - 1, n - 1);
            size_t right = std::min(i + 2 * currentSize - 1, n - 1);
            if (mid < right) {
                // ������� ��� ����� �������
                merge(arr, left, mid, right, temp);
            }
        }
        currentSize *= 2; // ��������� ������ ��������� ������
    }
    auto endMerge = std::chrono::high_resolution_clock::now();

    // ������� ����� ���������� � �������
    std::cout << "Sorting time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(endSort - startSort).count()
        << " ms\n";
    std::cout << "Merging time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(endMerge - startMerge).count()
        << " ms\n";
}

/// <summary>
/// ���������� ������ ��������� ����� � ��������� [-100, 100].
/// </summary>
/// <typeparam name="T">����� ��������� ��� (int, float)</typeparam>
/// <param name="size">������ ������������� �������.</param>
/// <returns>������ ��������� ����� ���� T.</returns>
template <typename T>
std::vector<T> generateRandomArray(size_t size) {
    // �������������� ��������� ��������� �����
    std::random_device rd;
    std::mt19937 gen(rd());
    // ������ ������ ��������� �������
    std::vector<T> arr(size);

    if constexpr (std::is_integral_v<T>) {
        // ���������� ����� ����� � ��������� [-100, 100]
        std::uniform_int_distribution<> dis(-100, 100);
        for (size_t i = 0; i < size; ++i) {
            arr[i] = dis(gen); // ��������� ������ ���������� ������ �������
        }
    }
    else if constexpr (std::is_floating_point_v<T>) {
        // ���������� ������������ ����� � ��������� [-100, 100]
        std::uniform_real_distribution<> dis(-100.0, 100.0);
        for (size_t i = 0; i < size; ++i) {
            arr[i] = dis(gen); // ��������� ������ ���������� ������������� �������
        }
    }

    return arr;
}

/// <summary>
/// ���������� ������ � ���� � ������� MessagePack.
/// </summary>
/// <typeparam name="T">����� ��������� ��� (int, float)</typeparam>
/// <param name="arr">������ ��� ������.</param>
/// <param name="filename">��� ����� ��� ������.</param>
/// <returns>true, ���� ������ �������, ����� false.</returns>
template <typename T>
bool writeArrayMsgpack(const std::vector<T>& arr, const std::string& filename) {
    try {
        // ��������� ���� ��� ������ � �������� ������
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << " for writing.\n";
            return false;
        }

        // ������ ����� ��� ����������� ������
        constexpr size_t bufferSize = 1048576;
        std::vector<char> buffer(bufferSize);
        ofs.rdbuf()->pubsetbuf(buffer.data(), bufferSize);

        // �������� ����� ������
        auto start = std::chrono::high_resolution_clock::now();
        // ������ ����� MessagePack
        msgpack::sbuffer sbuf;
        msgpack::packer<msgpack::sbuffer> pk(&sbuf);
        // ���������� map � ����� ������ "array"
        pk.pack_map(1);
        pk.pack(std::string("array"));
        // ���������� ������
        pk.pack_array(arr.size());
        for (const auto& value : arr) {
            pk.pack(value); // ����������� ������ �������
        }
        // ���������� ������ � ����
        ofs.write(sbuf.data(), sbuf.size());
        // ��������� ����
        ofs.close();

        // ������� ����� ������
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
/// ������ ������ �� ����� � ������� MessagePack.
/// </summary>
/// <typeparam name="T">����� ��������� ��� (int, float)</typeparam>
/// <param name="arr">������ ��� �������� ����������� ������.</param>
/// <param name="filename">��� ����� ��� ������.</param>
/// <returns>true, ���� ������ �������, ����� false.</returns>
template <typename T>
bool readArrayMsgpack(std::vector<T>& arr, const std::string& filename) {
    try {
        // ��������� ���� ��� ������ � �������� ������
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << " for reading.\n";
            return false;
        }
        // ������ ����� ��� ����������� ������
        constexpr size_t bufferSize = 1048576;
        std::vector<char> buffer(bufferSize);
        ifs.rdbuf()->pubsetbuf(buffer.data(), bufferSize);
        // �������� ����� ������
        auto start = std::chrono::high_resolution_clock::now();
        // ���������� ������ �����
        ifs.seekg(0, std::ios::end);
        size_t fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        // ������ ����� ��� ������ �����
        std::vector<char> fileBuffer(fileSize);
        // ������ ������ �� �����
        ifs.read(fileBuffer.data(), fileSize);
        // ��������� ����
        ifs.close();
        // ������������� ������ MessagePack
        msgpack::object_handle oh = msgpack::unpack(fileBuffer.data(), fileSize);
        msgpack::object obj = oh.get();
        // ��������� ������: map � ����� ������
        if (obj.type != msgpack::type::MAP || obj.via.map.size != 1) {
            std::cerr << "Error: Invalid Msgpack format. Expected map with one key.\n";
            return false;
        }
        auto& kv = obj.via.map.ptr[0];
        // ��������� ���� "array" � ��� �������� (������)
        if (kv.key.as<std::string>() != "array" || kv.val.type != msgpack::type::ARRAY) {
            std::cerr << "Error: Expected key 'array' with array value.\n";
            return false;
        }
        auto& array = kv.val.via.array;
        // ������� � ����������� ����� ��� �������
        arr.clear();
        arr.reserve(array.size);
        // ������ �������� �������
        for (size_t i = 0; i < array.size; ++i) {
            if constexpr (std::is_integral_v<T>) {
                // ���������, ��� ������� � ����� �����
                if (array.ptr[i].type != msgpack::type::POSITIVE_INTEGER &&
                    array.ptr[i].type != msgpack::type::NEGATIVE_INTEGER) {
                    std::cerr << "Error: Msgpack array contains non-integer value at index " << i << ".\n";
                    return false;
                }
                arr.push_back(array.ptr[i].as<T>()); // ������������ � ��� T
            }
            else if constexpr (std::is_floating_point_v<T>) {
                // ���������, ��� ������� � float ��� ����� �����
                if (array.ptr[i].type != msgpack::type::FLOAT32 &&
                    array.ptr[i].type != msgpack::type::POSITIVE_INTEGER &&
                    array.ptr[i].type != msgpack::type::NEGATIVE_INTEGER) {
                    std::cerr << "Error: Msgpack array contains non-float value at index " << i << ".\n";
                    return false;
                }
                arr.push_back(array.ptr[i].as<T>()); // ������������ � ��� T
            }
        }
        // ������� ����� ������
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Msgpack read time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << " ms\n";
        return true;
    }
    // ""����������� � ���������"'
    catch (const std::exception& e) {
        std::cerr << "Error reading Msgpack from " << filename << ": " << e.what() << "\n";
        return false;
    }
}

/// <summary>
/// ��������� ������������������ ������������� ���������� ��� �������� ������� �������.
/// </summary>
/// <param name="numThreads">���������� �������.</param>
void testSortPerformance(size_t numThreads) {
    // ���������� ������� �������� ��� ������������
    const std::vector<size_t> sizes = { 30000000, 50000000, 60000000 };
    for (size_t size : sizes) {
        // ���������� ��������� ������
        std::vector<int> arr = generateRandomArray<int>(size);
        std::cout << "Array size: " << size << ", Threads: " << numThreads << "\n";
        // �������� ����� ����� ����������
        auto totalStart = std::chrono::high_resolution_clock::now();
        // ��������� ����������
        parallelMergeSort(arr, numThreads);
        auto totalEnd = std::chrono::high_resolution_clock::now();
        // ������� ����� �����
        std::cout << "Total time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count()
            << " ms\n";
        std::cout << "------------------------\n";
    }
}

/// <summary>
/// ���������, ������������ �� ������ �� ����������.
/// </summary>
/// <typeparam name="T">����� ��������� ��� (int, float)</typeparam>
/// <param name="arr">������ ��� ��������.</param>
/// <returns>true, ���� ������ ������������, ����� false.</returns>
template <typename T>
bool isSorted(const std::vector<T>& arr) {
    // ���������, ��� ������ ������� �� ������ �����������
    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] < arr[i - 1]) {
            return false;
        }
    }
    return true;
}