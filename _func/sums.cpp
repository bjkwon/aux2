#include <numeric>
#include "functions_common.h"

Cfunction set_builtin_function_sums(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "arrray"};
	vector<string> desc_arg_opt = { "" };
	vector<CVar> default_arg = { /*CVar(default_value), CVar(string("default_str")), same number of desc_arg_opt*/ };
	set<uint16_t> allowedTypes1 = { 0, 1, 2, 3, ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

typedef bool (*checkfuncpt)(uint16_t tp, uint16_t mask, vector<uint16_t> items);

Cfunction set_builtin_function_lens(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "arrray/signal/string" };
	vector<string> desc_arg_opt = { "" };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 1, 2, 3, ALL_AUDIO_TYPES, TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, TYPEBIT_STRING + 3, 
		TYPEBIT_SIZE1, TYPEBIT_SIZE1 + 1, TYPEBIT_SIZE1 + 2, TYPEBIT_SIZE1 + 3,  TYPEBIT_BYTE + 1, TYPEBIT_BYTE + 2, TYPEBIT_BYTE + 3,
	    TYPEBIT_CELL };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// Sig
	set<pfunc_typecheck> allowedCheckFunc = { Cfunction::IsAUDIOG, Cfunction::IsSTRINGG, Cfunction::IsScalarG, Cfunction::IsVectorG, Cfunction::IsCellG };
	ft.qualify.push_back(allowedCheckFunc);
	set<pfunc_typecheck> prohibitFunc = { Cfunction::AllFalse, }; // prohibit false (none)
	ft.reject.push_back(prohibitFunc);
	// No args
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

CSignal __sum(auxtype *buf, unsigned int len, void* pargin, void* pargout)
{
	auto res = accumulate(buf, buf + len, 0.f);
	return CSignal(res);
}

CSignal __mean(auxtype* buf, unsigned int len, void* pargin, void* pargout)
{
	auto res = accumulate(buf, buf + len, 0.f);
	return CSignal(res/len);
}

CSignal __length(auxtype* buf, unsigned int len, void* pargin, void* pargout)
{
	return CSignal((auxtype)len);
}

CSignal __stdev(auxtype* buf, unsigned int len, void* pargin, void* pargout)
{
	auto sum = std::accumulate(buf, buf + len, 0.f);
	auto mean = sum / len;
	auto adder = 0.0f;
	std::for_each(buf, buf + len, [&](const auxtype d) {
		adder += (d - mean) * (d - mean);
		});
	auto stdev = sqrtf(adder / (len - 1));
	return CSignal(stdev);
}

void _sums(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (fname == "sum")
		past->Sig = past->Sig.evoke_getsig2(__sum);
	else if (fname == "mean")
		past->Sig = past->Sig.evoke_getsig2(__mean);
	else if (fname == "stdev")
		past->Sig = past->Sig.evoke_getsig2(__stdev);
}

void _lens(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (fname == "length")
	{
		if (!past->Sig.cell.empty())
		{
			past->Sig.SetValue((auxtype)past->Sig.cell.size());
			past->Sig.cell.clear();
		}
		else
			past->Sig = past->Sig.evoke_getsig2(__length);
	}
	else if (fname == "size")
	{
		CVar out((auxtype)past->Sig.nGroups);
		out.UpdateBuffer(2);
		out.buf[1] = (auxtype)past->Sig.Len();
		past->Sig = out;
	}
}

