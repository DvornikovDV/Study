// Дворников Даниил

#pragma once

#include "alg_analysis_func.h"
#include <fstream>
#include <string>
#include <exception>

using std::string;
using std::ifstream;
using std::invalid_argument;
using std::getline;

/// Считает количество прописных букв в строке data
size_t count_capital_let( string& data);

/// Читает все символы файла file_name в строку data
string file_read(string& data, string& file_name);