#include "functions_common.h"

static inline void __noise(float* buf, unsigned int length, int fs)
{
	auto k = (unsigned int)0;
	for_each(buf, buf + length, [&k, fs](float& v)
		{ v = 2.f * ((float)rand() / (float)RAND_MAX - .5); });
}

static inline void __gnoise(float* buf, unsigned int length, int fs)
{ //Gaussian noise
	float fac, r, v1, v2, sum(0.);
	for (unsigned int k = 0; k < length; k++)
	{
		do {
			do {
				v1 = (2.f * (float)rand() / (float)RAND_MAX) - 1.0;
				v2 = (2.f * (float)rand() / (float)RAND_MAX) - 1.0;
				r = (v1 * v1) + (v2 * v2);
			} while (r >= 1.0);
			fac = sqrt(-2.0 * log(r) / r);
		} while (v2 * fac >= 1.f || v2 * fac <= -1.f);
		buf[k] = v2 * fac;
		sum += v2 * fac;
	}
}

void _tparamonly(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	CVar dur = past->Sig;
	auto fs = past->GetFs();
	if (dur.value() < 0.)
		throw exception_func(*past, pnode, "duration must be non-negative .", pnode->str, 2).raise();
	past->Sig.Reset(fs);
	unsigned int nSamplesNeeded = (unsigned int)round(dur.value() / 1000. * fs);
	past->Sig.UpdateBuffer(nSamplesNeeded); //allocate memory if necessary
	if (!strcmp(pnode->str, "noise"))
		__noise(past->Sig.buf, nSamplesNeeded, fs);
	else if (!strcmp(pnode->str, "gnoise"))
		__gnoise(past->Sig.buf, nSamplesNeeded, fs);
	else if (!strcmp(pnode->str, "silence"))
		memset(past->Sig.buf, 0, sizeof(float) * nSamplesNeeded);
	else if (!strcmp(pnode->str, "dc"))
		for_each(past->Sig.buf, past->Sig.buf + nSamplesNeeded, [](float& v) {v = 1.; });
	else
		throw exception_etc(*past, pnode, "(not to be seen)").raise();
}