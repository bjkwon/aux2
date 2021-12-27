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
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void dramp(float* buf, uint64_t len, void* parg)
{
	float dur_ms = *(float*)parg;
	int fs = (int)*((float*)parg + 1);
	float drampFs = 1.e3 / (4. * dur_ms);
	uint64_t nSamplesNeeded = (uint64_t)round(dur_ms / 1000. * fs);
	nSamplesNeeded = min(len, nSamplesNeeded);
	for (uint64_t k = 0; k < nSamplesNeeded; k++)
	{
		float x = sin(2.f * PI * drampFs * k / fs);
		buf[k] *= x * x;
		buf[len - k - 1] *= x * x;
	}
}

//void _hamming(skope* past, const AstNode* pnode, const vector<CVar>& args)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	past->checkSignal(pnode, past->Sig);
//	past->Sig.evoke_modsig(&CSignal::Hamming);
//}
//
//void _blackman(skope* past, const AstNode* pnode, const vector<CVar>& args)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	past->checkSignal(pnode, past->Sig);
//	double alpha;
//	string fname = pnode->str;
//	if (fname == "blackman")
//		alpha = .16;
//	else
//		alpha = 0;
//	if (p) // if hann, p should be NULL, so alpha stays zero
//	{
//		skope temp(past);
//		temp.Compute(p);
//		temp.checkScalar(pnode, temp.Sig);
//		alpha = temp.Sig.value();
//	}
//	past->Sig.evoke_modsig(&CSignal::Blackman, &alpha);
//}

void _ramp(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	float rampdur_fs[2] = { args[0].value(), (float)past->Sig.GetFs() };
	if (rampdur_fs[0] <= 0.)
		throw exception_func(*past, pnode, "ramp duration must be positive.", "ramp()", 2).raise();
	past->Sig.evoke_modsig(dramp, &rampdur_fs);
}

//void _sam(skope* past, const AstNode* pnode, const vector<CVar>& args)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	double modRate, amDepth(1.), initPhase(0.);
//	CVar rate, depth, initphase;
//	past->checkAudioSig(pnode, past->Sig);
//	try {
//		skope tp(past);
//		rate = tp.Compute(p);
//		if (!rate.IsScalar())
//			throw CAstException(FUNC_SYNTAX, *past, p).proc("AM_rate must be a scalar.");
//		modRate = rate.value();
//		int nArgs(0);
//		for (const AstNode* cp = p; cp; cp = cp->next)
//			++nArgs;
//		if (nArgs >= 2) //either 2 or 3
//		{
//			depth = tp.Compute(p->next);
//			if (!depth.IsScalar())
//				throw CAstException(FUNC_SYNTAX, *past, p).proc("AM_depth_m must be a scalar.");
//			amDepth = depth.value();
//			if (nArgs == 3)
//			{
//				initphase = tp.Compute(p->next->next);
//				if (!initphase.IsScalar())
//					throw CAstException(FUNC_SYNTAX, *past, p).proc("initial_phase must be a scalar.");
//				initPhase = initphase.value();
//			}
//		}
//	}
//	catch (const CAstException & e) { throw CAstException(FUNC_SYNTAX, *past, pnode).proc(e.getErrMsg().c_str()); }
//	past->Sig.SAM(modRate, amDepth, initPhase);
//}
//
//CSignal& CSignal::Hamming(unsigned int id0, unsigned int len, void *p)
//{
//	if (len == 0) len = nSamples;
//	for (unsigned int k = 0; k < len; k++)
//		buf[id0 + k] *= 0.54 - 0.46 * cos(2.0 * PI * k / (len - 1.0));
//	return *this;
//}
//
//CSignal& CSignal::Blackman(unsigned int id0, unsigned int len, void *parg)
//{
//	double alpha = *(double*)parg;
//	for (unsigned int k = 0; k < len; k++)
//		buf[id0 + k] *= (1 - alpha) / 2 - 0.5 * cos(2.0 * PI * k / (len - 1.0)) + alpha / 2 * cos(4.0 * PI * k / (len - 1.0));
//	return *this;
//}

//CSignal& CSignal::SAM(double modRate, double modDepth, double initPhase)
//{
//	double* env = new double[nSamples];
//	for (unsigned int k = 0; k < nSamples; k++)
//		env[k] = (1. + modDepth * sin(2 * PI * (k * modRate / fs + initPhase - .25))) / (1. + modDepth);
//	Modulate(env, nSamples);
//	delete[] env;
//	return *this;
//}
//
//CTimeSeries& CTimeSeries::SAM(double rate, double depth, double phase)
//{
//	for (CTimeSeries* p = this; p; p = p->chain)
//		p->CSignal::SAM(rate, depth, phase);
//	return *this;
//}
//
//CSignals& CSignals::SAM(double rate, double depth, double phase)
//{
//	CTimeSeries::SAM(rate, depth, phase);
//	if (next)	next->CTimeSeries::SAM(rate, depth, phase);
//	return *this;
//}
