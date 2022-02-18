// ppp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "aux_classes.h"
#include "skope.h"
#include "skope_exception.h"
#include "echo.h"

extern vector<skope*> xscope;

void echo(int depth, skope& ctx, const AstNode* pnode, CVar* pvar)
{
	if (!pnode->suppress)
	{
		if (!pvar)
		{
			pvar = ctx.GetVariable(pnode->str);
			if (!pvar) return; // this filters out a null statement in a block such as a=1; b=100; 500
		}
		if (skope::IsLooping(pnode)) return; // T_IF, T_FOR, T_WHILE
		// if the command required waiting, lock a mutex here
		// ctx.xtree->alt indicates subsequent modifier of TID (e.e., x(10:20) x.sqrt, etc)
		if (pnode->type==T_ID && !ctx.node->alt)
		{
			auto body = skope::findDadNode(ctx.node, pnode);
			auto lhs = skope::findDadNode(ctx.node, body);
			bool variablenameset = false;
			if (pnode == ctx.node && body == lhs && lhs->type == T_ID && ctx.Vars.find(lhs->str) != ctx.Vars.end())
				variablenameset = true;
			else if (pnode->child)
				variablenameset = true; // pnode is LHS
			else if (lhs->type == N_VECTOR)
				variablenameset = true; // c=1; [a,b]=x.max
			string varname;
			if (variablenameset)
				varname = pnode->str;
			else
			{//c=1, x.max
				varname = "ans";
				ctx.SetVar(varname.c_str(), pvar);
			}
			echo_object().print(varname, *pvar, 1);
		}
		else
		{ // 1+a, 2^5, a' !a a>=1 ...
			ctx.SetVar("ans", pvar);
			echo_object().print("ans", *pvar, 1);
		}
	}
}


//[ 5  3 2 -1 9 83 7 62 9 7 6 8 9 7 3 2 -1]
static void show_result(skope& sc)
{
	int dt = 1;
	if (sc.node->type == N_BLOCK)
	{
		for (const AstNode* pp = sc.node->next; pp; pp = pp->next, dt++)
		{
			if (pp->next)
			{// For a block statement, screen echoing applies only to the last item. The statements in the middle are not echoed regardless of suppress (;)
				((AstNode*)pp)->suppress = true;
				echo(dt, sc, pp, NULL);
			}
			else if (pp->type == N_VECTOR)
			{
				for (AstNode* p2 = ((AstNode*)pp->str)->alt; !pp->suppress && p2; p2 = p2->next, dt++)
					echo(dt, sc, p2, NULL);
			}
			else
				echo(dt, sc, pp, sc.Sig.IsGO() ? sc.pgo : &sc.Sig);
		}
	}
	else if (sc.lhs && sc.lhs->type == N_VECTOR)// && sc.node->alt && sc.node->alt->type!=N_STRUCT) // sc.node->alt is necessary to ensure that there's a vector on the LHS
	{
		for (AstNode* pp = ((AstNode*)sc.node->str)->alt; !sc.lhs->suppress && pp; pp = pp->next, dt++)
			echo(dt, sc, pp, NULL);
	}
	else // see if lhs makes more sense than xtree
	{
		CVar* psig;
		if (sc.Sig.IsGO() && sc.Sig.GetFs() != 3) psig = sc.pgo;
		else psig = &sc.Sig;
		echo(dt, sc, sc.node, psig);
	}
	if (sc.statusMsg.empty())
		cout << endl;
	else
		cout << sc.statusMsg << endl;
}

CVar interpreter(skope& sc, const string& instr)
{
	auto nodes = sc.makenodes(instr);
	sc.node = nodes;
	sc.Compute();
	show_result(sc);
	return sc.Sig;
}

int main()
{
	CAstSigEnv* pglobalEnv = new CAstSigEnv(22050);
	pglobalEnv->AppPath = "";
	pglobalEnv->InitBuiltInFunctions();
	skope sc(pglobalEnv);
	xscope.push_back(&sc);
	string input;
	while (1)
	{
		try {
			printf("AUX> ");
			getline(cin, input);
			if (input.empty())
			{
				cout << "Press [ENTER] to quit" << endl;
				getline(cin, input);
				if (input.empty())
					break;
			}
			interpreter(sc, input);
		}
		catch (skope_exception e) {
			cout << "Error: " << e.getErrMsg() << endl;
		}
		catch (const char* msg)	{
			cout << "Error: " << msg << endl;
		}
	}
	delete pglobalEnv;
    return 0;
}

