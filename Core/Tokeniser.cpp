#include "stdafx.h"
#include "Core/Tokeniser.h"

void Tokenize(const string &str,
	vector<string> &tokens,
	const string &delimiters /* = " " */)
{
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

bool TokenizeCSV(const string& str,
	vector<string>& tokens)
{
	string::size_type index = 0;
	while(index < str.length())
	{
		bool bQuotes = false;

		string strToken;

		if(str[index] == '\"')
		{
			bQuotes = true;
			index++;
			if(index >= str.length())
				break;

			while(index < str.length())
			{
				string::size_type endIndex = str.find('\"', index);
				if(-1 == endIndex)
					return false; // Malformed CSV

				strToken += str.substr(index, endIndex - index);
				index = endIndex + 1;
				if(index < str.length())
				{
					char c = str[index];
					index++;
					if(c == '\"')
						strToken += '\"';
					else if(c == ',')
						break;
					else
						return false; // Malformed CSV
				}
			}
		}
		else
		{
			string::size_type endIndex = str.find(',', index);
			if(string::npos == endIndex)
				endIndex = str.length();
			strToken = str.substr(index, endIndex - index);
			index = endIndex + 1;
		}

		tokens.push_back(strToken);
	}
	if(index == str.length()) // Ended with a comma, last token is empty
		tokens.push_back("");

	return true;
}
