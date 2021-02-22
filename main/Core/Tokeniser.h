#pragma once

#include <vector>
#include <string>

using namespace std;

void Tokenize(const string &str,
	vector<string> &tokens,
	const string &delimiters = " ");

bool TokenizeCSV(const string &str,
	vector<string> &tokens);
