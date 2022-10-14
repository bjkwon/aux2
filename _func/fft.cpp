#include "fftw3.h"
#include "functions_common.h"

Cfunction set_builtin_function_fft(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "signal" };
	vector<string> desc_arg_opt = { "size", };
	vector<CVar> default_arg = { CVar(0.f), };
	set<uint16_t> allowedTypes1 = { 2, ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_ifft(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "FFT_result" };
	vector<string> desc_arg_opt = { "size", };
	vector<CVar> default_arg = { CVar(0.f), };
	set<uint16_t> allowedTypes1 = { FFT_RESULTS_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

CSignal __fft2(float *buf, unsigned int len, void* pargin, void* pargout)
{
	const vector<CVar>* pp = (const vector<CVar> *)pargin;
	auto pit = pp->begin();
	CVar fftlen = *pit;
	pit++;
	CVar fs = *pit;
	int fftsize = fftlen.value() == 0 ? len : (int)fftlen.value();
	fftsize = min(fftsize, (int)len);
	int fftRealsize = fftsize / 2 + 1;
	float* in;
	fftwf_complex* out;
	fftwf_plan p;

	in = (float*)fftwf_malloc(sizeof(float) * fftsize);
	out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * fftRealsize);
	memcpy(in, buf, sizeof(float) * fftsize);

	p = fftwf_plan_dft_r2c_1d(fftsize, in, out, FFTW_ESTIMATE);
	fftwf_execute(p);

	CSignal res((int)fs.value());
	res.UpdateBuffer(fftsize);
	res.SetComplex();
	memcpy(res.cbuf, out, sizeof(*res.cbuf) * fftRealsize);
	complex<float>* tp = reinterpret_cast<complex<float>*> (out);
	for (int k = 1; k < fftRealsize; k++)
		res.cbuf[fftsize - k] = conj(tp[k]);

	fftwf_destroy_plan(p);
	fftwf_free(in);
	fftwf_free(out);
	return res;
}

CSignal __ifft2(float* buf, unsigned int len, void* pargin, void* pargout)
{ // parg: ffsize fs object_type 
	const vector<CVar>* pp = (const vector<CVar> *)pargin;
	CVar param = *pp->begin();
	int fs = (int)(*(pp->begin() + 1)).value();
	uint16_t isbasesigComplex = (uint16_t)pp->back().value();
	int fftsize = param.value() == 0 ? len : (int)param.value();
	fftsize = min(fftsize, (int)len);
	int fftRealsize = fftsize / 2 + 1;
	CSignal res(fs);
	res.UpdateBuffer(fftsize);
	fftwf_plan p;
	bool hermit(true);
	if (!isbasesigComplex)
	{
		res.SetComplex();
		hermit = false;
	}
	fftwf_complex* in;
	//check if it's Hermitian
	complex<float>* cbuf = (complex<float>*)(void*)buf;
	for (int k = 1; hermit && k < (fftsize + 1) / 2; k++)
		if (cbuf[k] != conj(cbuf[fftsize - k])) hermit = false;
	if (hermit)
	{
		float* out = (float*)fftwf_malloc(sizeof(float) * fftsize);
		in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * fftRealsize);
		auto halfsize = fftsize / 2 + 1;
		memcpy(in, cbuf, sizeof(fftwf_complex) * halfsize);
		if (fftsize % 2 == 0)
			memcpy(&in[fftsize / 2], &cbuf[fftsize / 2], sizeof(*cbuf));
		p = fftwf_plan_dft_c2r_1d(fftsize, in, out, FFTW_ESTIMATE);
		fftwf_execute(p);
		memcpy(res.buf, out, sizeof(float) * fftsize);
		res.bufBlockSize = sizeof(float);
		res /= (float)fftsize;
		fftwf_free(out);
	}
	else
	{
		res.SetComplex();
		fftwf_complex* out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * fftsize);
		in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * fftsize);
		if (isbasesigComplex)
			memcpy(in, cbuf, sizeof(*in) * fftsize);
		else
			for (int k = 0; k < fftsize; k++)
			{
				memcpy(in + k, buf + k, sizeof(*res.buf));
				memset((char*)(in + k) + sizeof(*res.buf), 0, sizeof(*res.buf));
			}
		p = fftwf_plan_dft_1d(fftsize, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
		fftwf_execute(p);
		memcpy(res.cbuf, out, sizeof(fftwf_complex) * fftsize);
		res /= (float)fftsize;
		fftwf_free(out);
	}
	fftwf_free(in);
	fftwf_destroy_plan(p);
	res.snap = 0; // this should be zero, but just to make sure
	return res;
}

CTimeSeries __fft(const CTimeSeries& base, void* parg)
{
	const vector<CVar>* pp = (const vector<CVar> *)parg;
	CVar param = *pp->begin();
	int fftsize = param.value() == 0 ? base.nSamples : (int)param.value();
	fftsize = min(fftsize, (int)base.nSamples);
	int fftRealsize = fftsize / 2 + 1;
	float* in;
	fftwf_complex* out;
	fftwf_plan p;

	in = (float*)fftwf_malloc(sizeof(float) * fftsize);
	out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * fftRealsize);
	memcpy(in, base.buf, sizeof(float) * fftsize);

	p = fftwf_plan_dft_r2c_1d(fftsize, in, out, FFTW_ESTIMATE);
	fftwf_execute(p);

	CTimeSeries res(1);
	res.UpdateBuffer(fftsize);
	res.SetComplex();
	memcpy(res.cbuf, out, sizeof(*res.cbuf) * fftRealsize);
	complex<float>* tp = reinterpret_cast<complex<float>*> (out);
	for (int k = 1; k < fftRealsize; k++)
		res.cbuf[fftsize - k] = conj(tp[k]);

	fftwf_destroy_plan(p);
	fftwf_free(in);
	fftwf_free(out);
	return res;
}

CTimeSeries __ifft(const CTimeSeries& base, void* parg)
{
	const vector<CVar>* pp = (const vector<CVar> *)parg;
	CVar param = *pp->begin();
	int fs = pp->back().value();
	int fftsize = param.value() == 0 ? base.nSamples : (int)param.value();
	fftsize = min(fftsize, (int)base.nSamples);
	int fftRealsize = fftsize / 2 + 1;
	CTimeSeries res(fs);
	res.UpdateBuffer(fftsize);
	fftwf_plan p;
	bool hermit(true);
	auto tp = base.type();
	if (!ISCOMPLEX(tp))
	{
		res.SetComplex();
		hermit = false;
	}
	fftwf_complex* in;
	//check if it's Hermitian
	for (int k = 1; hermit && k < (fftsize + 1) / 2; k++)
		if (base.cbuf[k] != conj(base.cbuf[fftsize - k])) hermit = false;
	if (hermit)
	{
		float* out = (float*)fftwf_malloc(sizeof(float) * fftsize);
		in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * fftRealsize);
		memcpy(in, base.cbuf, sizeof(fftwf_complex) * fftsize / 2);
		if (fftsize % 2 == 0)
			memcpy(&in[fftsize / 2], &base.cbuf[fftsize / 2], sizeof(*base.cbuf));
		p = fftwf_plan_dft_c2r_1d(fftsize, in, out, FFTW_ESTIMATE);
		fftwf_execute(p);
		memcpy(res.buf, out, sizeof(float) * fftsize);
		res.bufBlockSize = sizeof(float);
		res /= (float)fftsize;
		fftwf_free(out);
	}
	else
	{
		res.SetComplex();
		fftwf_complex* out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * fftsize);
		in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * fftsize);
		if (ISCOMPLEX(tp))
			memcpy(in, base.cbuf, sizeof(*in) * fftsize);
		else
			for (int k = 0; k < fftsize; k++)
			{
				memcpy(in + k, base.buf + k, sizeof(*res.buf));
				memset((char*)(in + k) + sizeof(*res.buf), 0, sizeof(*res.buf));
			}
		p = fftwf_plan_dft_1d(fftsize, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
		fftwf_execute(p);
		memcpy(res.cbuf, out, sizeof(fftwf_complex) * fftsize);
		res /= (float)fftsize;
		fftwf_free(out);
	}
	fftwf_free(in);
	fftwf_destroy_plan(p);
	res.snap = 0; // this should be zero, but just to make sure
	return res;
}

void _fft(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	float val = args[0].value();
	if (val != (float)(int)val)
		exception_func(*past, pnode, "argument must be an integer.", "fftsize", 2).raise();
	if (val < 0)
		exception_func(*past, pnode, "argument must be positive or zero (for the entire array length).", "fftsize", 2).raise();
	vector<CVar> copy = args;
	copy.push_back(CVar((float)past->GetFs()));
	past->Sig = past->Sig.evoke_getsig2(__fft2, (void*)&copy);
	past->Sig.setsnap();
}

void _ifft(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	float val = args[0].value();
	if (val != (float)(int)val)
		exception_func(*past, pnode, "argument must be an integer.", "fftsize", 2).raise();
	if (val < 0)
		exception_func(*past, pnode, "argument must be positive or zero (for the entire array length).", "fftsize", 2).raise();
	vector<CVar> copy = args;
	copy.push_back(CVar((float)past->GetFs()));
	copy.push_back(CVar((float)ISCOMPLEX(past->Sig.type())));
	past->Sig = past->Sig.evoke_getsig2(__ifft2, (void*)&copy);
	past->Sig.setsnap();
//	if (past->Sig.type() & TYPEBIT_TEMPORAL)  // ?? 12/28/2021
	past->Sig.setsnap(0);
}