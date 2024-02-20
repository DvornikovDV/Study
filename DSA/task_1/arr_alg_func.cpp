// Дворников Даниил

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
	
	assert((incremental_search(b, 5.5, n) - 4) < eps);

	assert(static_cast<int>(is_sorted_up(b, n)) < eps);
	assert(static_cast<int>(is_sorted_down(b, n)) < eps);

	a[2] = 2.2; a[3] = 4.4;
	assert(static_cast<int>(is_sorted_up(b, n)) - 1 < eps);
}
// assert(() < eps);