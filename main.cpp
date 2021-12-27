// ppp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "aux_classes.h"
#include "skope.h"
#include "skope_exception.h"
#include "echo.h"

//[ 5  3 2 -1 9 83 7 62 9 7 6 8 9 7 3 2 -1]
static void show_result(skope& sc)
{
	auto tp = sc.Sig.type();
	cout << "type = " << tp << ", ";
	if (sc.Sig.IsString())
	{
		echo_object().print(sc.node->type == T_ID ? sc.node->str : "", sc.Sig, 1);
	}
	else if (ISNULL(tp))
		cout << "(null)";
	else if (ISSCALAR(tp))
		cout << sc.Sig.value();
	else if (ISVECTOR(tp) || ISAUDIO(tp))
		echo_object().print(sc.node->type == T_ID ? sc.node->str : "", sc.Sig, 1);
	cout << endl;
}

CVar interpreter(skope& sc, const string& instr)
{
	auto nodes = sc.makenodes(instr);
//	cout << instr << endl;
	if (!nodes)
		throw sc.emsg.c_str();
	sc.node = nodes;
	sc.Compute(nodes);
	show_result(sc);
	return sc.Sig;
}

int main()
{
	auto ss = (uint16_t)0x0008;
	auto ss2 = ss * 2;
	CAstSigEnv* pglobalEnv = new CAstSigEnv(22050);
	pglobalEnv->AppPath = "";
	pglobalEnv->InitBuiltInFunctions();
	skope sc(pglobalEnv);
	string input;
	while (1)
	{
		try {
			getline(cin, input);
			if (input.empty())
			{
				cout << "Press [ENTER] to quit" << endl;
				getline(cin, input);
				if (input.empty())
					break;
			}
			auto sss = input.c_str();
			interpreter(sc, input);
		}
		catch (const char* msg)
		{
			cout << "Error:" << msg << endl;
		}
	}
	//try {
	//	interpreter(sc, "a");
	//	interpreter(sc, "43-+2");
	//	interpreter(sc, "9/2");
	//	interpreter(sc, "sqrt(3)");
	//}
	delete pglobalEnv;

    return 0;
}

