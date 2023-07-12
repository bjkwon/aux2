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
				char c = linecommand.front();
				switch (c) {
				case 's':
				case 'S':
					u.debug.status = step;
					loop = false;
					break;
				case 'i':
				case 'I':
					u.debug.status = step_in;
					loop = false;
					break;
				case 'o':
				case 'O':
					u.debug.status = step_out;
					loop = false;
					break;
				case 'c':
				case 'C':
					u.debug.status = continu;
					loop = false;
					break;
				case 'x':
				case 'X':
					u.debug.status = abort2base;
					loop = false;
					break;
				case 'v':
				case 'V':
				{
					// Break into each line and fill up u.source
					vector <string> lines;
					int res = str2vector(lines, pEnv->udf[u.title].content, "\n\r");
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
		u.debug.status == step)
	{
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
	//	u.debug.
		if (p->type == T_ID || p->type == T_FOR || p->type == T_IF || p->type == T_WHILE || p->type == N_IDLIST || p->type == N_VECTOR)
			hold_at_break_point(p);
		process_statement(p);
		//		pgo = NULL; // without this, go lingers on the next line
		Sig.Reset(1); // without this, fs=3 lingers on the next line; if Sig is a cell or struct, it lingers on the next line and may cause an error
		if (fExit) return p;
		p = p->next;
	}
	return NULL;
}

int skope::CallUDF(const AstNode* pnode4UDFcalled, CVar* pBase, size_t nargout_requested)
{
	// Returns the number of output arguments requested in the call
	// 
	// t_func: the T_FUNCTION node pointer for the current UDF call, created after ReadUDF ("formal" context--i.e., how the udf file was read with variables used in the file)
	// pOutParam: AstNode for formal output variable (or LHS), just used inside of this function.
	// Output parameter dispatching (sending the output back to the calling worksapce) is done with pOutParam and lhs at the bottom.

	// u.debug.status is set when debug key is pressed (F5, F10, F11), prior to this call.
	// For an initial entry UDF, u.debug.status should be null
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
	AstNode* p;
	int line2;
	for (p = pFirst; p; p = p->next)
	{
		line2 = p->line;
		if (!p->next) // if the node is T_FOR, T_WHILE or T_IF, p-next is NULL is it should continue through p->child
		{
			if (p->type == T_FOR || p->type == T_WHILE)
				p = p->alt;
			else if (p->type == T_IF)
			{
				if (p->alt)
					p = p->alt;
				else
					p = p->child;
			}
		}
	}
	//probably needed to enter a new, external udf (if not, may skip)
//	if (pEnv->udf[u.base].newrecruit)
//		fpmsg.UpdateDebuggerGUI(this, refresh, -1); // shouldn't this be entering instead of refresh? It seems that way at least to F11 an not-yet-opened udf 10/16/2018. But.. it crashes. It must not have been worked on thoroughly...
														//if this is auxconscript front astsig, enter call fpmsg.UpdateDebuggerGUI()
//	if (u.debug.status == stepping)
//		/*u.debug.GUI_running = true, */ fpmsg.UpdateDebuggerGUI(this, entering, -1);
//	else
	{ // probably entrance udf... First, check if current udfname (i.e., Script) is found in DebugBreaks
		// if so, mark u.debug.status as progress and set next breakpoint
		// and call debug_GUI
		vector<int> breakpoint = pEnv->udf[u.base].DebugBreaks;
		for (vector<int>::iterator it = breakpoint.begin(); it != breakpoint.end(); it++)
		{
			if (*it < u.currentLine) continue;
			if (*it <= line2) {
				u.debug.status = progress; u.nextBreakPoint = *it;
				//u.debug.GUI_running = true, fpmsg.UpdateDebuggerGUI(this, entering, -1);
				break;
			}
		}
	}
	linebyline(pFirst);
	if (u.debug.status == abort2base)
	{
		u.currentLine = -1;
		throw this;
		return 1;
	}

	if (u.debug.status != null)
	{
		//		currentLine = -1; // to be used in CDebugDlg::ProcessCustomDraw() (re-drawing with default background color)... not necessary for u.debug.status==stepping (because currentLine has been updated stepping) but won't hurt
		if (u.debug.GUI_running == true)
		{
			// send to purgatory and standby for another debugging key action, if dad is the base scope
			//if (dad == xscope.front() && u.debug.status == stepping)
			//{
			//	fpmsg.UpdateDebuggerGUI(this, purgatory, -1);
			//	fpmsg.HoldAtBreakPoint(this, pLast);
			//}
			u.currentLine = -1;
			u.debug.inPurgatory = false; // necessary to reset the color of debug window listview.
			//when exiting from a inside udf (whether local or not) to a calling udf with F10 or F11, the calling udf now should have stepping.
//			fpmsg.UpdateDebuggerGUI(this, exiting, -1);
		}
		if (xscope.size() > 2) // pvevast hasn't popped yet... This means son is secondary udf (either a local udf or other udf called by the primary udf)
		{//why is this necessary? 10/19/2018----yes this is...2/16/2019
//			if (u.debug.status == stepping && fpmsg.IsCurrentUDFOnDebuggerDeck && !fpmsg.IsCurrentUDFOnDebuggerDeck(Script.c_str()))
//				fpmsg.UpdateDebuggerGUI(dad, entering, -1);
		}
	}
	return 0;
}

