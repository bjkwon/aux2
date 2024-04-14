#include <stdlib.h>
#include "functions_common.h"

Cfunction set_builtin_function_run(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "cmd", };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = {  TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}


void __run(auxtype* buf, uint64_t len, void* parg, void* parg2)
{
	auxtype rate = *(auxtype*)parg;
	auxtype depth = *((auxtype*)parg + 1);
	auxtype initphase = *((auxtype*)parg + 2);
	int fs = (int)*((auxtype*)parg + 3);
	for (unsigned int k = 0; k < len; k++)
	{
		auxtype env = (1. + depth * sin(2 * PI * (k * rate / fs + initphase - .25))) / (1. + depth);
		buf[k] *= env;
	}


}


void _run(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	system(past->Sig.str().c_str());
//	past->Sig.evoke_modsig(__run, &samrate_depth_initphase_fs);
}
