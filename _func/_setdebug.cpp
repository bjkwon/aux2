#include "functions_common.h"

Cfunction set_builtin_function_setdebug(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "udf_name", "lines for breakpoints"};
	vector<string> desc_arg_opt = { "" };
	vector<CVar> default_arg = { /*CVar(default_value), CVar(string("default_str")), same number of desc_arg_opt*/ };
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	set<uint16_t> allowedTypes2 = { 0, 1, 2, 3 };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _setdebug(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	for (uint64_t k = 0; k< args.front().nSamples; k++)
	{
		past->pEnv->udf[past->Sig.str()].DebugBreaks.push_back((int)args.front().buf[k]);
	}
}

