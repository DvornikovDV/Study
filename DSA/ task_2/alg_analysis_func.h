// ��������� ������

#pragma once

#include "alg_analysis_func.h"
#include <fstream>
#include <string>
#include <exception>

using std::string;
using std::ifstream;
using std::invalid_argument;
using std::getline;

/// ������� ���������� ��������� ���� � ������ data
size_t count_capital_let( string& data);

/// ������ ��� ������� ����� file_name � ������ data
string file_read(string& data, string& file_name);