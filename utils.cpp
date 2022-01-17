// ppp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "utils.h"

int GetFileText(FILE* fp, string& strOut)
{
	if (!fp) return -1;
	fseek(fp, 0, SEEK_END);
	int filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* buf = new char[filesize + 1];
	size_t pos = fread(buf, 1, (size_t)filesize, fp);
	// In case logfile contains EOF character in the middle (even accidentially)
	// making sure fread continues beyond the position of EOF
	size_t res = 1;
	while (pos > 0 && pos < filesize && res)
	{
		buf[pos++] = '\n';
		fseek(fp, (long)pos + 1 + 1, SEEK_SET);
		res = fread(buf + pos, 1, (size_t)(filesize - pos), fp);
		pos += res;
	}
	fclose(fp);
	if (pos <= 0)
		return -2;
	else
	{
		buf[pos] = 0;
		strOut = buf;
		delete[] buf;
	}
	return (int)pos;
}

int GetFileText(const char* fname, const char* mod, string& strOut)
{ // mod is either "rb" or "rt"
	FILE* fp = fopen(fname, mod);
	if (!fp) return -1;
	return GetFileText(fp, strOut);
}

int countDeliminators(const char* buf, const char* deliminators)
{
	char* token, * newBuffer;
	int i = 0;
	newBuffer = new char[strlen(buf) + 1];
	strcpy(newBuffer, buf);

	token = strtok(newBuffer, deliminators);
	while (token != NULL)
	{
		i++;
		token = strtok(NULL, deliminators);
	}
	delete[] newBuffer;
	return i;
}

size_t str2vect(vector<string>& _out, const char* input, const char* delim, int maxSize_x)
{ // Bug found--If delim contains repeating character(s), it goes into an infinite loop. Avoid that. 8/4/2017

	size_t pos;
	string str(input);
	string delim2(delim);
	vector<string> out;
	if (maxSize_x < 0) return 0;
	if (!input[0]) return 0;
	int nitems = countDeliminators(input, delim);
	out.reserve(nitems);
	trimr(str, delim);
	delim2 = delim2.front();
	for (unsigned int k = 1; k < strlen(delim); k++)
		while ((pos = str.find(delim[k])) != string::npos)
			str.replace(pos, 1, delim2);

	while ((pos = str.find(delim[0])) != string::npos)
	{
		if (maxSize_x > 0)
			if (out.size() == maxSize_x - 1)
				break;
		if (pos == 0) pos++;
		else		out.push_back(str.substr(0, pos));
		str.erase(0, pos);
	}
	out.push_back(str);
	_out = out;
	return out.size();
}

int str2vector(vector<string>& out, const string& in, const string& delim_chars)
{
	string _path = in;
	size_t pos = _path.find_first_of(delim_chars);
	size_t off = 0;
	string pcs;
	while (1)
	{
		if (pos == string::npos)
			pcs = _path.substr(off);
		else
			pcs = _path.substr(off, pos - off);
		if (!pcs.empty()) out.push_back(pcs);
		if (pos == string::npos) break;
		off = pos + 1;
		pos = _path.find_first_of(delim_chars, off);
	}
	return (int)out.size();
}

void triml(string& str, string delim)
{
	string::size_type pos = str.find_first_not_of(delim);
	if (pos != string::npos) str.erase(0, pos);
	else str.erase(str.begin(), str.end());
}

void trimr(string& str, string delim)
{
	string::size_type pos = str.find_last_not_of(delim);
	if (pos != string::npos) str.erase(pos + 1);
	else str.erase(str.begin(), str.end());
}

void trim(string& str, string delim)
{
	triml(str, delim);
	trimr(str, delim);
}

void triml(string& str, char delim)
{
	string _delim(1, delim);
	triml(str, _delim);
}

void trimr(string& str, char delim)
{
	string _delim(1, delim);
	trimr(str, _delim);
}

void trim(string& str, char delim)
{
	string _delim(1, delim);
	trim(str, _delim);
}

void triml(string& str, char* delim)
{
	string _delim(delim);
	triml(str, _delim);
}

void trimr(string& str, char* delim)
{
	string _delim(delim);
	trimr(str, _delim);
}

void trim(string& str, char* delim)
{
	string _delim(delim);
	trim(str, _delim);
}
