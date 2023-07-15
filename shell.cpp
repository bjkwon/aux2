#include <iostream>
#include <algorithm>
#include "skope.h"
#include "skope_exception.h"
#include "typecheck.h"
#include "utils.h"
#include <assert.h>
#include <thread>

using namespace std;

void show_result(skope& sc, int precision, const string& display, int display_count); // main.cpp
void auxenv(CAstSigEnv* pEnv, const string& cmd, skope* psk = NULL); // auxenv.cpp
void auxenv_cd(CAstSigEnv* pEnv, string& targetdir); // auxenv.cpp

//Application-wide global variables
extern vector<skope*> xscope;


static CVar interpreter(skope& sc, int display_precision, const string& instr, bool show)
{
	sc.statusMsg.clear();
	auto nodes = sc.makenodes(instr);
	if (!nodes)
		throw sc.emsg.c_str();
	sc.node = nodes;
	sc.Compute();
	if (show) {
		if (nodes->tail && nodes->tail->str)
			show_result(sc, display_precision, nodes->tail->str, (int)nodes->tail->dval);
		else
			show_result(sc, display_precision, "", -1);
	}
	return sc.Sig;
}

static map <int, string> stringSplit(const string& in, const string& delim)
{
	map <int, string> out;
	size_t pos0 = 0;
	size_t pos = in.find_first_of(delim);
	string next = in;
	int line = 2;
	auto extract = next.substr(pos0, pos);
	while (pos != string::npos) {
		pos0 = pos + delim.size() - 1;
		next = next.substr(pos0);
		pos = next.find_first_of(delim);
		extract = next.substr(0, pos);
		if (!extract.empty())
			out[line] = extract;
		line++;
	}
	return out;
}

void skope::command_shell(const string& prompt, const AstNode* p)
{
	string input;
	bool loop = true;
	while (loop) {
		cout << prompt;
		getline(cin, input);
		if (!input.empty())
		{
			//if the line begins with #, it bypasses the usual parsing
			if (input.front() == '#') {
				if (input.substr(1).front() == '#') {
					auxenv(pEnv, input.substr(2).c_str(), this);
				}
				else {
					if (input.substr(1, 3) == "cd ") {
						auxenv_cd(pEnv, input);
					}
					else
						int status = system(input.substr(1).c_str());
				}
			}
			else if (input.front() == '?') {
				auto linecommand = input.substr(1);
				if (linecommand.empty()) break;
				char c = linecommand.front();
				switch (c) {
				case 's':
				case 'S':
					u.debugstatus = step;
					loop = false;
					break;
				case 'i':
				case 'I':
					u.debugstatus = step_in;
					loop = false;
					break;
				case 'o':
				case 'O':
					u.debugstatus = step_out;
					loop = false;
					break;
				case 'c':
				case 'C':
					u.debugstatus = continu;
					loop = false;
					break;
				case 'x':
				case 'X':
					u.debugstatus = abort2base;
					loop = false;
					break;
				case 'v':
				case 'V':
				{
					// Break into each line and fill up u.source
					auto lines = stringSplit(pEnv->udf[u.base].content, "\n\r");
					// u.title should be the udf file name
					// Todo: lines[p->line] is incorrect if there are empty lines in the file
					cout << "* " << lines[p->line] << endl;
					break;
				}
				}
			}
			else {
				interpreter(*this, pEnv->display_precision, input, true);
			}
		}
	}

}

static void shell(skope* ths, const AstNode* p)
{
	char buf[256];
	string prompt = ths->u.title + ":" + itoa(p->line, buf, 10) + "> ";
	ths->command_shell(prompt, p);
}

static bool isItBreakPoint(const vector<int>& breakpoints, int currentLine)
{
	return find(breakpoints.begin(), breakpoints.end(), currentLine) != breakpoints.end();
}

void skope::hold_at_break_point(const AstNode* pnode)
{
	// if the current line is one of breakpoints
	// if currently stepping
	if (isItBreakPoint(pEnv->udf[u.title].DebugBreaks, pnode->line) ||
		u.debugstatus == step ||
		u.debugstatus == step_in
		)
	{
		if (u.debugstatus == step_in)  u.debugstatus = progress;
		thread thd(shell, this, pnode);
		thd.join();
	}
}

const AstNode* skope::linebyline(const AstNode* p)
{
	while (p)
	{
		pLast = p;
		// T_IF, T_WHILE, T_FOR are checked here to break right at the beginning of the loop
		u.currentLine = p->line;
		if (p->type == T_ID || p->type == T_FOR || p->type == T_IF || p->type == T_WHILE || p->type == N_IDLIST || p->type == N_VECTOR)
			hold_at_break_point(p);
		if (u.debugstatus == abort2base)
		{
			u.currentLine = -1;
			throw this;
		}
		process_statement(p);
		Sig.Reset(1); // without this, fs=3 lingers on the next line; if Sig is a cell or struct, it lingers on the next line and may cause an error
		if (pEnv->inTryCatch)
			pTryLast = p;
		if (fExit) return p;
		p = p->next;
	}
	return NULL;
}

void skope::CallUDF(const AstNode* pnode4UDFcalled, CVar* pBase, size_t nargout_requested)
{
	// Returns the number of output arguments requested in the call
	// 
	// t_func: the T_FUNCTION node pointer for the current UDF call, created after ReadUDF ("formal" context--i.e., how the udf file was read with variables used in the file)
	// pOutParam: AstNode for formal output variable (or LHS), just used inside of this function.
	// Output parameter dispatching (sending the output back to the calling worksapce) is done with pOutParam and lhs at the bottom.

	// u.debugstatus is set when debug key is pressed (F5, F10, F11), prior to this call.
	// For an initial entry UDF, u.debugstatus should be null
	CVar nargin((auxtype)u.nargin);
	CVar nargout((auxtype)nargout_requested);
	SetVar("nargin", &nargin);
	SetVar("nargout", &nargout);
	// If the udf has multiple statements, p->type is N_BLOCK), then go deeper
	// If it has a single statement, take it from there.
	AstNode* pFirst = u.t_func->child->next;
	if (pFirst->type == N_BLOCK)	pFirst = pFirst->next;
	//Get the range of lines for the current udf
	u.currentLine = pFirst->line;
	linebyline(pFirst);

}

