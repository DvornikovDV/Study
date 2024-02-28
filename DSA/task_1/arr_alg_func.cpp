// Дворников Даниил ИВТ-22

#include "arr_alg_headers.h"

/// Тестирует функции
void test() {
	srand(time(NULL));

	size_t n{ 5 };
	double* a = new double[n];
	double* b{};

	/*fill_arr_rand(a, n, 2.1, 5.8);
	print_arr(a, n);*/

	a[0] = 1.1; a[1] = 2.2; a[2] = 4.4; a[3] = 3.3; a[4] = 5.5;

	/// тест файлов
	{const string file_name = "test_file.txt";
		try {
			f_save_arr(a, file_name, n);
		}
		catch (const invalid_argument& err) {
			cout << err.what();
		}

		try {
			b = f_read_arr(b, file_name, n);
		}
		catch (const invalid_argument& err) {
			cout << err.what();
		}
		//print_arr(b, n);
		assert((b[4] - 5.5) < eps);
	}
	
	
	/// тест последовательного поиска
	assert((incremental_search(b, 5.5, n) - 4) == 0);
	assert((incremental_search(b, 2.2, n) - 1) == 0);
	assert(incremental_search(b, 1.1, n) == 0);
	assert((incremental_search(b, 0.0, n) + 1) == 0);
	
	{ /// тест бинарного поиска
		size_t n{ 5 };
		double* c = new double[n];
		a[0] = 1.1; a[1] = 2.2; a[2] = 3.3; a[3] = 3.3; a[4] = 5.5;

		assert((bin_search(b, 0.0, n) + 1) < eps);
		assert(bin_search(b, 1.1, n) < eps);
		assert((bin_search(b, 2.2, n) - 1) < eps);
		assert((bin_search(b, 5.5, n) - 4) < eps);

		delete[] c;
	}

	assert(static_cast<int>(is_sorted_up(b, n)) == false);
	assert(static_cast<int>(is_sorted_down(b, n)) == false);

	/// тест сортировок
	bubble_sort(b, n);
	assert(is_sorted_up(b, n) == true);

	a[0] = 1.1; a[1] = 2.2; a[2] = 3.3; a[3] = 4.4; a[4] = 5.5;
	bubble_sort(a, n);
	assert(static_cast<int>(is_sorted_up(a, n)) == true);

	a[0] = 5.5; a[1] = 4.4; a[2] = 3.3; a[3] = 2.2; a[4] = 1.1;
	bubble_sort(a, n);
	assert(static_cast<int>(is_sorted_up(a, n)) == true);

	delete[] b;
	delete[] a;
}
// assert(() < eps);