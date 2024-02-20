// Дворников Даниил

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

/// Читает данные из файла file_name в массив arr размера n
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

/// Записывает данные в файл file_name в массив arr размера n
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

/// Возвращает индекс найденного числа value в массиве arr размера n, либо -1, если число не найдено
template<class Arr_type> long long incremental_search(Arr_type arr[], Arr_type value, size_t n) {
	for (size_t i{}; i < n; i++) {
		if (arr[i] == value) return i;
	}
	
	return -1;
}

/// Проверяет, отсортирован ли массив по возрастанию
template<class Arr_type> bool is_sorted_up(Arr_type arr[], size_t n) {
	Arr_type temp{ arr[0] };

	for (size_t i{ 1 }; i < n; i++) {
		if (arr[i] < temp) return false;
		temp = arr[i];
	}

	return true;
}

/// Проверяет, отсортирован ли массив по убыванию
template<class Arr_type> bool is_sorted_down(Arr_type arr[], size_t n) {
	Arr_type temp{ arr[0] };

	for (size_t i{ 1 }; i < n; i++) {
		if (arr[i] > temp) return false;
		temp = arr[i];
	}
	в
	return true;
}

/// Тестирует функции
void test();