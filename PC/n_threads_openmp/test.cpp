#include "pch.h"

// Тест генерации случайного массива
TEST(GenerateRandomArrayTest, IntRangeAndSize) {
    size_t size = 1000;
    auto arr = generateRandomArray<int>(size);
    EXPECT_EQ(arr.size(), size) << "Array size does not match requested size";
    for (const auto& val : arr) {
        EXPECT_GE(val, -100) << "Value below minimum range (-100)";
        EXPECT_LE(val, 100) << "Value above maximum range (100)";
    }
}

TEST(GenerateRandomArrayTest, FloatRangeAndSize) {
    size_t size = 1000;
    auto arr = generateRandomArray<float>(size);
    EXPECT_EQ(arr.size(), size) << "Array size does not match requested size";
    for (const auto& val : arr) {
        EXPECT_GE(val, -100.0f) << "Value below minimum range (-100.0)";
        EXPECT_LE(val, 100.0f) << "Value above maximum range (100.0)";
    }
}

// Тест однопоточной сортировки
TEST(SingleThreadSortTest, Correctness) {
    std::vector<int> arr = generateRandomArray<int>(1000);
    auto original = arr;
    singleThreadMergeSort(arr);
    EXPECT_TRUE(isSorted(arr)) << "Array is not sorted";
    std::sort(original.begin(), original.end());
    EXPECT_EQ(arr, original) << "Sorted array does not match std::sort result";
}

// Тест многопоточной сортировки с тремя проверками
TEST(ParallelSortTest, MultipleConfigurations) {
    // Проверка 1: 2M элементов, 1 ядро
    {
        std::vector<int> arr = generateRandomArray<int>(2000000);
        auto original = arr;
        parallelMergeSort(arr, 1);
        EXPECT_TRUE(isSorted(arr)) << "2M array with 1 thread is not sorted";
        std::sort(original.begin(), original.end());
        EXPECT_EQ(arr, original) << "2M array with 1 thread does not match std::sort";
    }

    // Проверка 2: 2M элементов, 2 ядра
    {
        std::vector<int> arr = generateRandomArray<int>(2000000);
        auto original = arr;
        parallelMergeSort(arr, 2);
        EXPECT_TRUE(isSorted(arr)) << "2M array with 2 threads is not sorted";
        std::sort(original.begin(), original.end());
        EXPECT_EQ(arr, original) << "2M array with 2 threads does not match std::sort";
    }

    // Проверка 3: 10M элементов, 4 ядра
    {
        std::vector<int> arr = generateRandomArray<int>(10000000);
        auto original = arr;
        parallelMergeSort(arr, 4);
        EXPECT_TRUE(isSorted(arr)) << "10M array with 4 threads is not sorted";
        std::sort(original.begin(), original.end());
        EXPECT_EQ(arr, original) << "10M array with 4 threads does not match std::sort";
    }
}

// Тест чтения/записи MessagePack
TEST(MsgpackIOTest, WriteAndReadInt) {
    std::vector<int> original = generateRandomArray<int>(1000);
    std::string filename = "test.cbor";
    EXPECT_TRUE(writeArrayMsgpack(original, filename)) << "Failed to write Msgpack file";
    std::vector<int> loaded;
    EXPECT_TRUE(readArrayMsgpack(loaded, filename)) << "Failed to read Msgpack file";
    EXPECT_EQ(loaded, original) << "Loaded array does not match original";
    std::remove(filename.c_str()); // Используем std::remove вместо std::filesystem
}

TEST(MsgpackIOTest, WriteAndReadFloat) {
    std::vector<float> original = generateRandomArray<float>(1000);
    std::string filename = "test_float.cbor";
    EXPECT_TRUE(writeArrayMsgpack(original, filename)) << "Failed to write Msgpack file";
    std::vector<float> loaded;
    EXPECT_TRUE(readArrayMsgpack(loaded, filename)) << "Failed to read Msgpack file";
    EXPECT_EQ(loaded, original) << "Loaded array does not match original";
    std::remove(filename.c_str());
}

// Дополнительные тесты для краевых случаев
TEST(ParallelSortTest, EmptyArray) {
    std::vector<int> arr;
    parallelMergeSort(arr, 4);
    EXPECT_TRUE(isSorted(arr)) << "Empty array is not sorted";
    EXPECT_TRUE(arr.empty()) << "Empty array modified unexpectedly";
}

TEST(ParallelSortTest, SingleElement) {
    std::vector<int> arr = { 42 };
    auto original = arr;
    parallelMergeSort(arr, 4);
    EXPECT_TRUE(isSorted(arr)) << "Single element array is not sorted";
    EXPECT_EQ(arr, original) << "Single element array modified";
}

TEST(ParallelSortTest, AlreadySorted) {
    std::vector<int> arr(1000);
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = static_cast<int>(i - 500); // -500, -499, ..., 499
    }
    auto original = arr;
    parallelMergeSort(arr, 4);
    EXPECT_TRUE(isSorted(arr)) << "Already sorted array is not sorted";
    EXPECT_EQ(arr, original) << "Already sorted array modified incorrectly";
}

TEST(ParallelSortTest, ReverseSorted) {
    std::vector<int> arr(1000);
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = static_cast<int>(499 - i); // 499, 498, ..., -500
    }
    auto original = arr;
    parallelMergeSort(arr, 4);
    EXPECT_TRUE(isSorted(arr)) << "Reverse sorted array is not sorted";
    std::sort(original.begin(), original.end());
    EXPECT_EQ(arr, original) << "Reverse sorted array does not match std::sort";
}

TEST(ParallelSortTest, AllZeros) {
    std::vector<int> arr(1000, 0);
    auto original = arr;
    parallelMergeSort(arr, 4);
    EXPECT_TRUE(isSorted(arr)) << "All zeros array is not sorted";
    EXPECT_EQ(arr, original) << "All zeros array modified incorrectly";
}

TEST(ParallelSortTest, AllOnes) {
    std::vector<int> arr(1000, 1);
    auto original = arr;
    parallelMergeSort(arr, 4);
    EXPECT_TRUE(isSorted(arr)) << "All ones array is not sorted";
    EXPECT_EQ(arr, original) << "All ones array modified incorrectly";
}

TEST(ParallelSortTest, RepeatedElements) {
    std::vector<int> arr(1000, 42);
    std::fill(arr.begin(), arr.begin() + 500, -42);
    auto original = arr;
    parallelMergeSort(arr, 4);
    EXPECT_TRUE(isSorted(arr)) << "Repeated elements array is not sorted";
    std::sort(original.begin(), original.end());
    EXPECT_EQ(arr, original) << "Repeated elements array does not match std::sort";
}