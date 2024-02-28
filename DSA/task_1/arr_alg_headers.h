// Дворников Даниил ИВТ-22

#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <exception>
#include <fstream>
#include <random>
#include <string>

// Pаскомментировать строку ниже, чтобы отключить assert()
//#define NDEBUG
#include <cassert>

using namespace std::chrono;
using std::size_t;
using namespace std;
using std::string;

const double eps = 1e-6;

// double*, double, double
/// Заполняет массив arr размера n псевдослучайными значениями в диапазоне от lower_limit до upper_limit
template<class Arr_type> void fill_arr_rand(Arr_type* arr, size_t n, double upper_limit, double lower_limit) {
	for (size_t i{}; i < n; i++) {
		arr[i] = lower_limit +  (rand()) / ( (RAND_MAX / (upper_limit - lower_limit)));
	}
};

/// Выводит в консоль массив arr размера n
template<class Arr_type> void print_arr(Arr_type arr[], size_t n) {
	cout << "Array : ";
	for (size_t i{}; i < n; i++) {
		cout << arr[i] << "\t";
	}

	cout << endl;
}

/// Читает данные из файла file_name в массив arr и размерность n
template<class Arr_type> Arr_type* f_read_arr(Arr_type arr[], const string& file_name, size_t n) {
	ifstream file_read(file_name);
	//Arr_type* a;

	if (file_read.is_open()) {
		file_read >> n;

		arr = new Arr_type[n];

		for (size_t i = 0; i < n; i++) {
			file_read >> arr[i];
		}
	}
	else {
		throw invalid_argument("No such file in directory");
	}

	file_read.close();

	return arr;
}

/// Записывает данные в файл file_name из массив arr и размерности n
template<class Arr_type> void f_save_arr(Arr_type arr[], const string& file_name, size_t n) {
	ofstream file_write(file_name);

	if (file_write.is_open()) {
		file_write << n << endl;

		for (size_t i = 0; i < n; i++) {
			file_write << arr[i] << " ";
		}
	}
	else {
		throw invalid_argument("Access error - unable to create file");
	}

	file_write.close();
}

/// Возвращает индекс найденного числа value в массиве arr размера n, либо -1, если число не найдено (последовательный поиск best - O(1), average - O(n), worst - O(n))
template<class Arr_type> long long incremental_search(Arr_type arr[], Arr_type key, size_t n) {
	for (size_t i{}; i < n; i++) {
		if (arr[i] == key) return i;
	}
	
	return -1;
}

/// Возвращает индекс найденного числа value в массиве arr размера n, либо -1, если число не найдено (бинарный поиск  best - О(1), average - О(log(n)), worst - О(log(n)))
template<class Arr_type> long long bin_search(Arr_type arr[], Arr_type key, size_t n) {

	long midd = 0;
	long left = 0;
	long right = n;

	while (1)
	{
		midd = (left + right) / 2;

		if (key < arr[midd])       // если искомое меньше значения в ячейке
			right = midd - 1;      // смещаем правую границу поиска
		else if (key > arr[midd])  // если искомое больше значения в ячейке
			left = midd + 1;	   // смещаем левую границу поиска
		else                       // иначе (значения равны)
			return midd;           // функция возвращает индекс ячейки

		if (left > right)          // если границы сомкнулись 
			return -1;
	}
}

/// Проверяет, отсортирован ли массив arr размера n по возрастанию
template<class Arr_type> bool is_sorted_up(Arr_type arr[], size_t n) {
	Arr_type temp{ arr[0] };

	for (size_t i{ 1 }; i < n; i++) {
		if (arr[i] < temp) return false;
		temp = arr[i];
	}

	return true;
}

/// Проверяет, отсортирован ли массив arr размера n по убыванию
template<class Arr_type> bool is_sorted_down(Arr_type arr[], size_t n) {
	Arr_type temp{ arr[0] };

	for (size_t i{ 1 }; i < n; i++) {
		if (arr[i] > temp) return false;
		temp = arr[i];
	}
	
	return true;
}

/// Сортирует массив по возрастанию (пузырьковая сортировка O(n^2))
template<class Arr_type> void bubble_sort(Arr_type arr[], size_t n) {
	Arr_type tmp{};

	for (size_t i{}; i < n; i++) {
		for (size_t j{}; j < n - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
			}
		}
	}
}

/// Тестирует функции
void test();