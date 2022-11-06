#include "skope.h"

vector<uint16_t> Cfunction::audiotype_real = {};
vector<uint16_t> Cfunction::stringtype = {};

Cfunction::Cfunction() {
	funcsignature = "";
	alwaysstatic = false;
	narg1 = 0;
	narg2 = 0;
	audiotype_complex = audiotype_real;
	audiotype_bool = audiotype_real;
	for (auto v : audiotype_real) audiotype_complex.push_back( v + 0b01100000);
	for (auto v : audiotype_real) audiotype_bool.push_back( v + TYPEBIT_SIZE1);
	Cfunction::audiotype_real = { TYPEBIT_TEMPO_ONE + 2, TYPEBIT_TEMPO_ONE + 3, TYPEBIT_TEMPO_CHAINS + 2, TYPEBIT_TEMPO_CHAINS + 3, };
	Cfunction::stringtype = { TYPEBIT_STRING };
}
Cfunction& Cfunction::operator=(const Cfunction& rhs)
{
	if (this != &rhs) {
		funcsignature = rhs.funcsignature;
		alwaysstatic = rhs.alwaysstatic;
		narg1 = rhs.narg1;
		narg2 = rhs.narg2;
	}
	return *this;
}

bool Cfunction::IsAUDIO(uint16_t tp)
{
	auto items = Cfunction::audiotype_real;
	uint16_t masked = tp & 0xF0FF;
	return find(items.begin(), items.end(), masked) != items.end();
}

bool Cfunction::IsAUDIOG(uint16_t tp)
{
	auto items = Cfunction::audiotype_real;
	uint16_t masked = tp & 0xF0FF;
	return find(items.begin(), items.end(), masked) != items.end();
}

bool Cfunction::IsSTRING(uint16_t tp)
{
	auto items = Cfunction::stringtype;
	uint16_t masked = tp & 0xFFF0;
	return find(items.begin(), items.end(), masked) != items.end();
}

bool Cfunction::IsSTRINGG(uint16_t tp)
{
	auto items = Cfunction::stringtype;
	uint16_t masked = tp & 0x00F0;
	return find(items.begin(), items.end(), masked) != items.end();
}

