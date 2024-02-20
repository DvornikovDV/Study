// Дворников Даниил

#include "alg_analysis_func.h"

/// Считает количество прописных букв в строке data
size_t count_capital_let( string& data) {
	size_t count{};

	for (char& i: data) {
		if ( isupper(i)) count++;
	}

	return count;
}

/// Читает все символы файла file_name в строку data
string file_read(string &data, string& file_name) {
	ifstream file_read(file_name);

	if (file_read.is_open()) { // читает по строкам
		while (getline(file_read, file_name)) {
			data += file_name;
		}
	}
	else throw invalid_argument("No such file in directory");

	file_read.close();

	return data;
}