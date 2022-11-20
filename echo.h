#pragma once
#include <iostream>
#include "skope.h"
#include <complex>

class echo_object
{
public:
	echo_object() {
		offset = 0;
		type = 0;
		precision = 6;
		tbht = "";
		tbht_count = -1;
	};
	echo_object(const string& display, int display_count) {
		offset = 0;
		type = 0;
		precision = 6;
		tbht = display;
		tbht_count = display_count;
	};
	virtual ~echo_object() {};
	int offset;
	int precision;
	string name;
	string postscript;
	uint16_t type;
	string tbht;
	int tbht_count;
	void print(const string& name, const CVar& obj, int offset);
	string print_vector(const CTimeSeries& obj, int offset);
	void print_temporal(const string& title, const CVar& obj, int offset);
	string row(const CTimeSeries& obj, unsigned int id0, int offset, int prec);
	string tmarks(const CTimeSeries& obj, bool unit);
	string make(const CTimeSeries& sig, bool unit, int offset);
	void header(const string& head)
	{
		if (!name.empty() && !head.empty())
		{
			for (int k = 0; k < offset; k++) cout << " ";
			if (!head.empty()) cout << head << " = ";
			else cout << name << " = ";
		}
	};
};

class echo_cell : public echo_object
{
public:
	echo_cell(const string& _name, int _offset) {
		name = _name; offset = _offset;
	};
	virtual ~echo_cell() {};
	void print(const CVar& obj, const string& head);
};

class echo_struct : public echo_object
{
public:
	echo_struct(const string& _name, int _offset) {
		name = _name; offset = _offset;
	};
	virtual ~echo_struct() {};
	void print(const CVar& obj, const string& head);
};
