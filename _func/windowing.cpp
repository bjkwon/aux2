#include "functions_common.h"

Cfunction set_builtin_function_ramp(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj", "ramp_dur" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
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

Cfunction set_builtin_function_blackman(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj" };
	vector<string> desc_arg_opt = { "window parameter" };
	vector<CVar> default_arg = { CVar(.16f) };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
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

Cfunction set_builtin_function_hamming(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_sam(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj", "SAM_rate(Hz)" };
	vector<string> desc_arg_opt = { "depth=1.", "initial phase=0"};
	vector<CVar> default_arg = { CVar(1.f), CVar(0.f)};
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void dramp(float* buf, uint64_t len, void* parg, void* parg2)
{
	float dur_ms = *(float*)parg;
	int fs = (int)*((float*)parg + 1);
	float drampFs = 1.e3 / (4. * dur_ms);
	uint64_t nRamping = (uint64_t)round(dur_ms / 1000. * fs);
	nRamping = min(len, nRamping);
	for (uint64_t k = 0; k < nRamping; k++)
	{
		float x = sin(2.f * PI * drampFs * k / fs);
		buf[k] *= x * x;
		buf[len - k - 1] *= x * x;
	}
}

void __sam(float* buf, uint64_t len, void* parg, void* parg2)
{
	float rate = *(float*)parg;
	float depth = *((float*)parg + 1);
	float initphase = *((float*)parg + 2);
	int fs = (int)*((float*)parg + 3);
	for (unsigned int k = 0; k < len; k++)
	{
		float env = (1. + depth * sin(2 * PI * (k * rate / fs + initphase - .25))) / (1. + depth);
		buf[k] *= env;
	}
}

static void __hamming(float* buf, uint64_t len, void* parg, void* parg2)
{
	for (unsigned int k = 0; k < len; k++)
		buf[k] *= 0.54 - 0.46 * cos(2.0 * PI * k / (len - 1.0));
}

void _hamming(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	past->Sig.evoke_modsig(__hamming, NULL);
}

static void __blackman(float* buf, uint64_t len, void* parg, void* parg2)
{
	float alpha = *(float*)parg;
	for (unsigned int k = 0; k < len; k++)
		buf[k] *= (1 - alpha) / 2 - 0.5 * cos(2.0 * PI * k / (len - 1.0)) + alpha / 2 * cos(4.0 * PI * k / (len - 1.0));
}

void _blackman(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (fname == "blackman")
	{
		float alpha[1] = { args[0].value() };
		past->Sig.evoke_modsig(__blackman, &alpha);
	}
	else if (fname == "hann")
	{
		float alpha[1] = { 0.f };
		past->Sig.evoke_modsig(__blackman, &alpha);
	}
}

void _ramp(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	float rampdur_fs[2] = { args[0].value(), (float)past->Sig.GetFs() };
	if (rampdur_fs[0] <= 0.)
		throw exception_func(*past, pnode, "ramp duration must be positive.", "ramp()", 2).raise();
	past->Sig.evoke_modsig(dramp, &rampdur_fs);
}

void _sam(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	float samrate_depth_initphase_fs[4] = { args[0].value(), args[1].value(), args[2].value(), (float)past->Sig.GetFs() };
	past->Sig.evoke_modsig(__sam, &samrate_depth_initphase_fs);
}
