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
void auxenv(CAstSigEnv* pEnv, const AstNode* pnode, skope* psk); // auxenv.cpp
void auxenv_cd(CAstSigEnv* pEnv, string& targetdir); // auxenv.cpp
bool interpreter(skope& sc, int display_precision, const string& instr, bool show, bool debug); // main.cpp

//Application-wide global variables
extern vector<skope*> xscope;

void skope::command_shell(const string& prompt, const AstNode* p)
{
	string input;
	bool loop = true;
	bool lineprinted = false;
	while (loop) {
		try {
			if (!lineprinted) {
			cout << p->line << " " << pEnv->udf[u.base].lines[p->line] << endl;
			lineprinted = true;
		}
		cout << "(trace)" << prompt;
		getline(cin, input);
		if (!input.empty())
		{
			loop = interpreter(*this, pEnv->display_precision, input, true, true);
		}
	}
	catch (skope_exception e) {
		cout << "Error: " << e.getErrMsg() << endl;
	}
	catch (const char* msg) {
		cout << "Error: " << msg << endl;
	} 
	}
}

static void shell(skope* ths, const AstNode* p)
{
	string prompt = ths->u.title + "> ";
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
		// N_IDLIST here is probably outdated. 7/26/2023
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

