#include "skope.h"
#include "skope_exception.h"

// Block a cell, strut or struts object; used for function args
void blockCellStruct1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname, int id)
{
	if (checkthis.GetFs() == 3)
		throw exception_func(sc, pnode, "array of handles", fname, id).raise();
	if (checkthis.type() & TYPEBIT_CELL)
		throw exception_func(sc, pnode, "cell object", fname, id).raise();
	if (checkthis.type() & TYPEBIT_STRUT)
		throw exception_func(sc, pnode, "struct object", fname, id).raise();
}

// Block a cell, strut or struts object; used for non-function operands
void blockCellStruct2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id)
{
	if (checkthis.GetFs() == 3)
		throw exception_misuse(sc, pnode, "array of handles", id).raise();
	if (checkthis.type() & TYPEBIT_CELL)
		throw exception_misuse(sc, pnode, "cell object", id).raise();
	if (checkthis.type() & TYPEBIT_STRUT)
		throw exception_misuse(sc, pnode, "struct object", id).raise();
}

void blockString1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname, int id)
{
	if (checkthis.IsString()) {
		throw exception_func(sc, pnode, "Not valid with a string object; ", fname, id).raise();
	}
}

void blockString2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id)
{
	if (checkthis.IsString()) {
		throw exception_misuse(sc, pnode, "Not valid with a string object", id).raise();
	}
}

void blockLogical1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname, int id)
{
	if (checkthis.type() & TYPEBIT_LOGICAL) {
		throw exception_func(sc, pnode, "Not valid with a string object; ", fname, id).raise();
	}
}
void blockLogical2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id)
{
	if (checkthis.type() & TYPEBIT_LOGICAL) {
		throw exception_misuse(sc, pnode, "Not valid with a string object", id).raise();
	}
}

void ensureVector1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname, int id)
{ // Vector must not be temporal; must not be GO, cell, strut
	if (checkthis.type() & TYPEBIT_TEMPORAL)
	{
		string msg("Audio or tseq object not allowed.");
		throw exception_func(sc, pnode, msg, fname, id).raise();
	}
	if (checkthis.type() & TYPEBIT_GO ||
		checkthis.type() & TYPEBIT_CELL ||
		checkthis.type() & TYPEBIT_STRUT ||
		checkthis.type() & TYPEBIT_STRUTS)
	{
		string msg("GO, cell, or class objects not allowed.");
		throw exception_func(sc, pnode, msg, fname, id).raise();
	}
}

void ensureVector2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id)
{ // Vector must not be temporal; must not be GO, cell, strut
	if (checkthis.type() & TYPEBIT_TEMPORAL)
	{
		string msg("Audio or tseq object not allowed.");
		throw exception_misuse(sc, pnode, msg, id).raise();
	}
	if (checkthis.type() & TYPEBIT_GO ||
		checkthis.type() & TYPEBIT_CELL ||
		checkthis.type() & TYPEBIT_STRUT ||
		checkthis.type() & TYPEBIT_STRUTS)
	{
		string msg("GO, cell, or class objects not allowed.");
		throw exception_misuse(sc, pnode, msg, id).raise();
	}
}

void ensureVector3(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& errormsg)
{
	if (checkthis.type() & TYPEBIT_TEMPORAL)
	{
		string msg = errormsg + " Audio or tseq object not allowed.";
		throw exception_etc(sc, pnode, msg).raise();
	}
	if (checkthis.type() & TYPEBIT_GO ||
		checkthis.type() & TYPEBIT_CELL ||
		checkthis.type() & TYPEBIT_STRUT ||
		checkthis.type() & TYPEBIT_STRUTS)
	{
		string msg = errormsg + " GO, cell, or class objects not allowed.";
		throw exception_etc(sc, pnode, msg).raise();
	}
}

void ensureScalar1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname, int id)
{
	if (checkthis.type() != 0x0001)
	{
		string msg("must be a scalar.");
		throw exception_func(sc, pnode, msg, fname, id).raise();
	}
}
void ensureScalar2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id)
{
	if (checkthis.type() != 0x0001)
	{
		string msg("must be a scalar.");
		throw exception_misuse(sc, pnode, msg, id).raise();
	}
}

void ensureAudio1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname = "", int id = 0)
{
	if (!checkthis.IsAudio())
	{
		string msg("must be an audio obj.");
		throw exception_func(sc, pnode, msg, fname, id).raise();
	}
}
void ensureAudio2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id = 0)
{
	if (!checkthis.IsAudio())
	{
		string msg("must be an audio obj.");
		throw exception_misuse(sc, pnode, msg, id).raise();
	}
}
