// ppp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "aux_classes.h"
#include "skope.h"
#include "skope_exception.h"
#include "echo.h"
#include "utils.h"
#ifndef _WIN32
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#endif

#define AUXENV_FILE "auxenv.json"
#define DEFAULT_FS 22050
#define PRECISION 6 // the default precision tested in Windows11, Visual Studio 2019

void auxenv(CAstSigEnv* pEnv, const string& cmd); // auxenv.cpp
void auxenv_cd(CAstSigEnv* pEnv, string& targetdir); // auxenv.cpp
void read_auxenv(int& fs0, int& precision, vector<string>& auxpathfromenv, const string& envfilename);
void save_auxenv(CAstSigEnv* pEnv, const string& envfilename); // auxenv.cpp

extern vector<skope*> xscope;

void echo(int precision, int depth, skope& ctx, const AstNode* pn, CVar* pvar, const string& display, int display_count)
{
	if (!pn->suppress)
	{
		const AstNode *plhs, *prhs;
		ctx.get_nodes_left_right_sides(pn, &plhs, &prhs);
		//  plhs NULL: a statement, not an assignment, use pvar from Sig (if that's not available, out of luck)
		if (plhs || !pvar) {
			pvar = ctx.GetVariable(pn->str);
			if (!pvar) return; // this filters out a null statement in a block such as a=1; b=100; 500
		}
		if (skope::IsLooping(pn)) return; // T_IF, T_FOR, T_WHILE
		// if the command required waiting, lock a mutex here
		// ctx.xtree->alt indicates subsequent modifier of TID (e.e., x(10:20) x.sqrt, etc)
		echo_object EO(display, display_count);
		EO.precision = precision;
		if (pn->type == T_ID || pn->type == N_VECTOR) {
			if (pn->alt && pn->alt->type == N_STRUCT)
				EO.print(pn->str, pvar, 0);
			else if (pn->alt && pn->alt->type == N_ARGS)
				EO.print(string(pn->str) + "(__)", pvar, 1);
			else
				EO.print(pn->str, pvar, 1);
		}
		else
		{ // 1+a, 2^5, a' !a a>=1 ...
			ctx.SetVar("ans", pvar);
			EO.print("ans", *pvar, 1);
		}
	}
}

//[ 5  3 2 -1 9 83 7 62 9 7 6 8 9 7 3 2 -1]
static void show_result(skope& sc, int precision, const string& display, int display_count)
{
	int dt = 1;
	if (sc.node->type == N_BLOCK)
	{
		for (const AstNode* pp = sc.node->next; pp; pp = pp->next, dt++)
		{
			if (pp->type == N_VECTOR)
			{
				for (AstNode* p2 = ((AstNode*)pp->str)->alt; !pp->suppress && p2; p2 = p2->next, dt++)
					echo(precision, dt, sc, p2, NULL, display, display_count);
			}
			else
				echo(precision, dt, sc, pp, sc.Sig.IsGO() ? sc.pgo : &sc.Sig, display, display_count);
		}
	}
	else if (sc.node->type == N_VECTOR)// && sc.node->alt && sc.node->alt->type!=N_STRUCT) // sc.node->alt is necessary to ensure that there's a vector on the LHS
	{
		if (sc.node->alt && sc.node->alt->type==N_STRUCT)
			echo(precision, dt, sc, sc.node, &sc.Sig, display, display_count);
		else
			for (AstNode* pp = ((AstNode*)sc.node->str)->alt; !sc.node->suppress && pp; pp = pp->next, dt++)
				echo(precision, dt, sc, pp, NULL, display, display_count);
	}
	else // see if lhs makes more sense than xtree
	{
		CVar* psig;
		if (sc.Sig.IsGO() && sc.Sig.GetFs() != 3) psig = sc.pgo;
		else psig = &sc.Sig;
		echo(precision, dt, sc, sc.node, psig, display, display_count);
	}
	if (sc.statusMsg.empty())
		cout << endl;
	else
		cout << sc.statusMsg << endl;
}

CVar interpreter(skope& sc, int display_precision, const string& instr)
{
	auto nodes = sc.makenodes(instr);
	if (!nodes)
		throw sc.emsg.c_str();
	sc.node = nodes;
	sc.Compute();
	if (nodes->tail && nodes->tail->str)
		show_result(sc, display_precision, nodes->tail->str, (int)nodes->tail->dval);
	else
		show_result(sc, display_precision, "", -1);
	return sc.Sig;
}

int main()
{
	int display_precision(PRECISION);
	int fs0(DEFAULT_FS);
	vector<string> auxpathfromenv;
	read_auxenv(fs0, display_precision, auxpathfromenv, AUXENV_FILE);
	CAstSigEnv* pglobalEnv = new CAstSigEnv(fs0);
	pglobalEnv->display_precision = display_precision;
	pglobalEnv->AuxPath = auxpathfromenv;
	pglobalEnv->AppPath = get_current_dir();
	pglobalEnv->InitBuiltInFunctions();
	skope sc(pglobalEnv);
	xscope.push_back(&sc);
	string input, line;
	CVar paths;

	bool programExit = false;
#ifndef _WIN32
	ifstream historyfstream("aux2.history", istream::in);
	while(getline(historyfstream, line))
	{
		add_history(line.c_str());
	}
	historyfstream.close();
#endif
	int commandid = 0;
	while (1)
	{
		try {
#ifdef _WIN32
			printf("AUX> ");
			getline(cin, input);
#else
			input.clear();
			char* readbuf = programExit ? readline("") : readline("AUX> ");
			if (readbuf == NULL)
			{
				cout << "Internal Error: readline; Program will exit.\n";
				break;
			}
			if (strlen(readbuf) > 0) 
			{
				add_history(readbuf);
				ofstream historyfstream2("aux2.history", ostream::out | ios::app);
				historyfstream2 << readbuf << endl;
				historyfstream2.close();
				input = readbuf;
				free(readbuf);
			}
#endif
			if (!input.empty())
			{
				//if the line begins with #, it bypasses the usual parsing
				if (input.front() == '#') {
					if (input.substr(1).front() == '#') {
						auxenv(pglobalEnv, input.substr(2).c_str());
					}
					else {
						if (input.substr(1, 3) == "cd ") {
							auxenv_cd(pglobalEnv, input);
						}
						else 
							int status = system(input.substr(1).c_str());
					}
				}
				else
					interpreter(sc, pglobalEnv->display_precision, input);
				programExit = false;
			}
			else
			{
				if (programExit) break;
				programExit = true;
				cout << "Press [ENTER] to quit" << endl;
			}
		}
		catch (skope_exception e) {
			cout << "Error: " << e.getErrMsg() << endl;
		}
		catch (const char* msg)	{
			cout << "Error: " << msg << endl;
		}
	}
	save_auxenv(pglobalEnv, AUXENV_FILE);
	delete pglobalEnv;
	return 0;
}

