#include <numeric>
#include "functions_common.h"

Cfunction set_builtin_function_rmsetc(fGate fp)
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
	set<uint16_t> allowedTypes1 = { 1, 2, 3, ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

inline static float _getdB(double x)
{
	// 3 dB is added to make rms of full scale sinusoid 0 dB
	return 20 * log10f(x) + 3.0103f;
}

CSignal __rms(float *buf, unsigned int len, void* pargin, void* pargout)
{
	float res;
	if (len == 0) res = std::numeric_limits<float>::infinity();
	else
	{
		float val = 0.f;
		for_each(buf, buf + len, [&val](float& v) {val += v * v; });
		res = _getdB(sqrt(val / len));
	}
	return CSignal(res);
}


void _rmsetc(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (fname == "rms")
		past->Sig = past->Sig.evoke_getsig2(__rms);
	else if (fname == "begint")
		past->Sig = past->Sig.evoke_getval(&CSignal::begint);
	else if (fname == "endt")
		past->Sig = past->Sig.evoke_getval(&CSignal::endt);
	else if (fname == "dur")
		past->Sig = past->Sig.evoke_getval(&CSignal::dur);
	else if (fname == "rmsall")
		past->Sig = past->Sig.evoke_getval(&CSignal::dur);
}

