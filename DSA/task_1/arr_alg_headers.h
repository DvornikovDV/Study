// ��������� ������

#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <exception>
#include <fstream>
#include <random>
#include <string>

// P���������������� ������ ����, ����� ��������� assert()
//#define NDEBUG
#include <cassert>

using namespace std::chrono;
using std::size_t;
using namespace std;
using std::string;

const double eps = 1e-6;

// double*, double, double
/// ��������� ������ arr ������� n ���������������� ���������� � ��������� �� lower_limit �� upper_limit
template<class Arr_type> void fill_arr_rand(Arr_type* arr, size_t n, double upper_limit, double lower_limit) {
	for (size_t i{}; i < n; i++) {
		arr[i] = lower_limit +  (rand()) / ( (RAND_MAX / (upper_limit - lower_limit)));
	}
};

/// ������� � ������� ������ arr ������� n
template<class Arr_type> void print_arr(Arr_type arr[], size_t n) {
	cout << "Array : ";
	for (size_t i{}; i < n; i++) {
		cout << arr[i] << "\t";
	}

	cout << endl;
}

/// ������ ������ �� ����� file_name � ������ arr ������� n
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

/// ���������� ������ � ���� file_name � ������ arr ������� n
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

/// ���������� ������ ���������� ����� value � ������� arr ������� n, ���� -1, ���� ����� �� �������
template<class Arr_type> long long incremental_search(Arr_type arr[], Arr_type value, size_t n) {
	for (size_t i{}; i < n; i++) {
		if (arr[i] == value) return i;
	}
	
	return -1;
}

/// ���������, ������������ �� ������ �� �����������
template<class Arr_type> bool is_sorted_up(Arr_type arr[], size_t n) {
	Arr_type temp{ arr[0] };

	for (size_t i{ 1 }; i < n; i++) {
		if (arr[i] < temp) return false;
		temp = arr[i];
	}

	return true;
}

/// ���������, ������������ �� ������ �� ��������
template<class Arr_type> bool is_sorted_down(Arr_type arr[], size_t n) {
	Arr_type temp{ arr[0] };

	for (size_t i{ 1 }; i < n; i++) {
		if (arr[i] > temp) return false;
		temp = arr[i];
	}
	�
	return true;
}

/// ��������� �������
void test();