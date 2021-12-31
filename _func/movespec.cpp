#include "functions_common.h"

Cfunction set_builtin_function_movespec(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio obj", "arg2" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, 2, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void __movespec(float* buf, uint64_t len, void* parg)
{
	float shift = *(float*)parg;
	float fs = *((float*)parg + 1);
	vector<float> copy(buf, buf + len);
	//	Hilbert();
	float t(0), grid(1.f / fs);
	const complex<float> j(0.0, 1.0);
	complex<float> datum;
	for (unsigned int k = 0; k < len; k++)
	{
		datum = (copy.data()[k] + buf[k] * j) * exp(j * shift * 2.f * PI * t);
		buf[k] = real(datum);
		t += grid;
	}
}

void _movespec(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	float shift_fs[2] = { args[0].value(), (float)past->Sig.GetFs() };
	past->Sig.evoke_modsig(__movespec, NULL);
}