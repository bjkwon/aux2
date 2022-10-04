#pragma once
#include "skope.h"
#include <sstream>

typedef void(*fGate) (skope* past, const AstNode* pnode, const vector<CVar>& args);

/* Error message must be on of the following patterns:
* 1) 2nd arg must be ______ in func()
* 2) >> requires a scalar operand
* 3) Out of range: 2nd index of TID 
* 4) any other error message: for example, "var" not defined or not a built_in function
*/

class skope_exception
{
public:
	skope_exception() {
		pnode = NULL;  pCtx = NULL;
	};
	skope_exception(const skope& base, const AstNode* _pnode) {
		pnode = _pnode;  pCtx = &base;
		line = pnode->line;
		col = pnode->col;
	};
	virtual ~skope_exception() {};
	skope_exception& raise(); 
	void addLineCol();
	string getErrMsg() const { return outstr; };
	int line, col;

//protected:
	const AstNode* pnode;
	const skope* pCtx; // pointer to the context, AKA AstSig, that threw the exception
	void findTryLine(const skope& skope);
	string basemsg, tidstr;
	string msgonly; // including basemsg, tidstr, and extra
	string sourceloc; // source location; where the error occurred (line, col and file)
	string outstr; // msgonly \n sourceloc
};


class exception_func : public skope_exception
{
public:
	exception_func(const skope& base, const AstNode* _pnode, const string& msg, const string& fname="", int id = 0) {
		ostringstream oss;
		pnode = _pnode;  pCtx = &base;
		oss << "Invalid arg";
		if (id > 0)
			oss << id;
		oss << " " << msg;
		if (!fname.empty())
			oss << " in " << fname << "()";
		msgonly = oss.str().c_str();
		line = pnode->line;
		col = pnode->col;
	};
	~exception_func() {};
};

class exception_misuse : public skope_exception
{
public:
	exception_misuse(const skope& base, const AstNode* _pnode, const string& msg, int id = 0) {
		ostringstream oss;
		pnode = _pnode;  pCtx = &base;
		oss << "invalid operand";
		if (id > 0)
			 oss << id;
		oss << " " << msg;
		msgonly = oss.str().c_str();
		line = pnode->line;
		col = pnode->col;
	};
	~exception_misuse() {};
};

class exception_range : public skope_exception
{
public:
	exception_range(const skope& base, const AstNode* _pnode, const string& idpos, const string& msg) {
		ostringstream oss;
		pnode = _pnode;  pCtx = &base;
		oss << "Out of range: " << idpos << " " << msg;
		msgonly = oss.str().c_str();
		line = pnode->line;
		col = pnode->col;
	};
	~exception_range() {};
};

class exception_etc : public skope_exception
{
public:
	exception_etc(const skope& base, const AstNode* _pnode, const string& msg) {
		pnode = _pnode;  pCtx = &base;
		msgonly = msg;
		line = pnode->line;
		col = pnode->col;
	};
	~exception_etc() {};
};

