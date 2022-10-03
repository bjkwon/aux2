#include "functions_common.h"

//void _include(skope* past, const AstNode* pnode)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	string dummy, emsg;
//	string filename = past->ComputeString(p);
//	if (FILE* auxfile = past->fopen_from_path(filename, "", dummy)) {
//		try {
//			skope qscope(past);
//			string filecontent;
//			if (GetFileText(auxfile, filecontent) <= 0)
//			{ // File reading error or empty file
//				past->statusMsg += "Cannot read specified file: " + filename;
//				fclose(auxfile);
//				return;
//			}
//			fclose(auxfile);
//			qscope.xtree = qscope.parse_aux(filecontent.c_str(), emsg);
//			if (!qscope.xtree)
//				throw emsg.c_str();
//			vector<CVar*> res = qscope.Compute();
//			past->Sig = res.back();
//			for (map<string, CVar>::iterator it = qscope.Vars.begin(); it != qscope.Vars.end(); it++)
//				past->Vars[it->first] = it->second;
//			for (auto it = qscope.GOvars.begin(); it != qscope.GOvars.end(); it++)
//				past->GOvars[it->first] = it->second;
//		}
//		catch (const char* errmsg) {
//			fclose(auxfile);
//			throw CAstException(USAGE, *past, pnode).proc((string(errmsg) + "while including " + filename + " in the file: ").c_str());
//		}
//	}
//	else
//		throw CAstException(USAGE, *past, pnode).proc("Cannot read file: ", "", filename);
//}


Cfunction set_builtin_function_eval(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "expression or commands", };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING+1, TYPEBIT_STRING+2 }; 
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}
void _eval(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
    // eval() is one of the functions where echoing in the xcom command window doesn't make sense.
  // but the new variables created or modified within the eval call should be transported back to the calling scope
	// As of 5/17/2020, there is no return of eval (null returned if assigned) for when there's no error
	// If there's an error, the error is caught here and the error message is sent to the calling function.
	string emsg, exp = past->Sig.str();
	skope qscope(past);
	try {
		qscope.node = qscope.makenodes(exp.c_str());
		if (!qscope.node) // syntax error in the expression
			throw exception_etc(*past, pnode, qscope.emsg).raise();

		qscope.process_statement(qscope.node);
		//transporting variables
		for (map<string, CVar>::iterator it = qscope.Vars.begin(); it != qscope.Vars.end(); it++)
			past->SetVar(it->first.c_str(), &it->second);
		past->Sig = qscope.Sig; // temp hack; just to port out the last result during the eval call
	}
	catch (skope_exception e) {
		e.line = pnode->line;
		e.col = pnode->col;
		e.pCtx = past;
		e.pnode = pnode;
		throw e;
	}
}