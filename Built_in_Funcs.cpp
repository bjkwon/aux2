// AUXLAB 
//
// Copyright (c) 2009-2018 Bomjun Kwon (bjkwon at gmail)
// Licensed under the Academic Free License version 3.0
//
// Project: sigproc
// Signal Generation and Processing Library
// Platform-independent (hopefully) 
// 
// Version: 1.495
// Date: 12/13/2018
// 

#include <math.h>
#include <map>
#include <string>
#include <time.h>
#include "aux_classes.h"
#include "skope.h"
using namespace std;

static map<string, string> builtin_fnsigs;
static map<string, bool> builtin_staticfn;
static map<string, int> builtin_fn_nArg1, builtin_fn_nArg2;

complex<float> cmpexp(complex<float> x) { return exp(x); }
complex<float> cmpcos(complex<float> x) { return cos(x); }
complex<float> cmpcosh(complex<float> x) { return cosh(x); }
complex<float> cmplog(complex<float> x) { return log(x); }
complex<float> cmplog10(complex<float> x) { return log10(x); }
complex<float> cmpsin(complex<float> x) { return sin(x); }
complex<float> cmpsinh(complex<float> x) { return sinh(x); }
complex<float> cmptan(complex<float> x) { return tan(x); }
complex<float> cmptanh(complex<float> x) { return tanh(x); }

float cmpreal(complex<float> x) { return real(x); }
float cmpimag(complex<float> x) { return imag(x); }
float cmpabs(complex<float> x) { return abs(x); }
complex<float> cmpconj(complex<float> x) { return conj(x); }
complex<float> cmpsqrt(complex<float> x) { return sqrt(x); }
//float cmpnorm(complex<float> x) { return norm(x); }
float cmpangle(complex<float> x) { return arg(x); }

float aux_db(float x)
{
	return powf(10.f, x / 20.f);
}

float aux_sign(float x)
{
	return (x == 0.f) ? 0.f : ((x>0.f) ? 1.f : -1.f);
}

float aux_round(float x)
{
	if (x >= 0.f)
		return (float)(int)(x + .5f);
	else
		return -aux_round(-x);
}

float aux_fix(float x)
{
	return (float)(int)x;
}

complex<float> aux_pow_comp(complex<float> base, float exponent)
{
	return pow(base, exponent);
}

CSignal aux_pow(CSignal& base, unsigned int id0, unsigned int len, void* p)
{
	body operand = *(body*)p;
	if (len == 0) len = base.nSamples;
	if (base.IsAudio())
	{
		base.each_sym2(powf, operand);
	}
	else if (base._min(id0, len) < 0)
	{
		base.SetComplex();
		if (operand.nSamples == 1)
		{
			for (auto k = id0; k < id0 + len; k++) base.cbuf[k] = pow(base.cbuf[k], operand.value());
		}
		else if (len == 1)
		{
			auto baseval = base.value();
			base.UpdateBuffer(operand.nSamples);
			for (auto k = id0; k < id0 + len; k++) base.cbuf[k] = pow(baseval, operand.buf[k]);
		}
		else
		{
			len = min(len, (unsigned int)operand.nSamples);
			for (auto k = id0; k < id0 + len; k++) base.cbuf[k] = pow(base.cbuf[k], operand.buf[k]);
		}
	}
	else
	{
		base.each(powf, operand);
	}
	return base;
}

void _pow(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	CVar base = past->Sig;
	CVar exponent = args[0];
	past->Sig = past->Sig.evoke_getsig(&aux_pow, (void*)&exponent);
}

CSignal aux_mod(CSignal& base, unsigned int id0, unsigned int len, void* p)
{
	body operand = *(body*)p;
	if (len == 0) len = base.nSamples;
	base.each_sym2(fmod, operand);
	return base;
}

void _mod(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	CVar base = past->Sig;
	CVar oper = args[0];
	past->Sig = past->Sig.evoke_getsig(&aux_mod, (void*)&oper);
}

void _sqrt(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
//	past->Sig.evoke_getval();
}

//void _sin(skope* past, const AstNode* pnode, const vector<CVar>& args)
//{
//	// need arg check
//	past->Sig.evoke_getval(sinf);
//}

complex<float> aux_cexp(complex<float> base, complex<float> exponent)
{
	return pow(base, exponent);
}

float aux_mod(float numer, float denom)
{
	return fmod(numer, denom);
}

float aux_passthru(float number)
{
	return number;
}

float aux_angle(float number)
{
	return number;
}

float aux_angle_4_real(float number)
{
	return number > 0.f ? acosf(1.f) : acosf(-1.f);
}

static inline complex<float> r2c_sqrt(complex<float> x) { return sqrt(x); }
static inline complex<float> r2c_log(complex<float> x) { return log(x); }
static inline complex<float> r2c_log10(complex<float> x) { return log10(x); }
static inline complex<float> r2c_pow(complex<float> x, complex<float> y) { return pow(x, y); }


void skope::HandleMathFunc(string& fname, const body& arg)
{
	float(*fn1)(float) = NULL;
	float(*fn2)(float, float) = NULL;
	float(*cfn0)(complex<float>) = NULL;
	complex<float>(*cfn1)(complex<float>) = NULL;
	complex<float>(*cfn2)(complex<float>, complex<float>) = NULL;
	if (fname == "abs")
	{
		if (Sig.IsComplex())		cfn0 = cmpabs, Sig.each(cfn0);
		else						fn1 = fabs, Sig.each(fn1);
	}
	else if (fname == "conj") { if (Sig.IsComplex()) cfn1 = cmpconj; 	else	fn1 = fabs; }
	else if (fname == "real") { if (Sig.IsComplex()) cfn0 = cmpreal; 	else fn1 = aux_passthru; }
	else if (fname == "imag") {
		if (Sig.IsComplex()) cfn0 = cmpimag;
		else {
			Sig.SetReal();
			for (auto& val : Sig) val = 0.;
			fn1 = aux_passthru;
		}
	}
	else if (fname == "angle") { if (Sig.IsComplex()) cfn0 = cmpangle;	else fn1 = aux_angle_4_real; }
	else if (fname == "sin")	if (Sig.IsComplex())	cfn1 = cmpsin; else fn1 = sinf;
	else if (fname == "cos")	if (Sig.IsComplex())	cfn1 = cmpcos; else fn1 = cosf;
	else if (fname == "tan")	if (Sig.IsComplex())	cfn1 = cmptan; else fn1 = tanf;
	else if (fname == "log")
	{
		fn1 = logf, cfn1 = r2c_log;
	}
	else if (fname == "log10")
	{
		fn1 = log10f, cfn1 = r2c_log10;
	}
	else if (fname == "sqrt")
	{
		fn1 = sqrtf; cfn1 = r2c_sqrt;
	}
	else if (fname == "pow" || fname == "^")
	{
		fn2 = powf, cfn2 = r2c_pow;
	}
	else if (fname == "mod" || fname == "%")
	{
		fn2 = fmodf, cfn2 = NULL; // do this
	}
	else if (fname == "exp")	if (Sig.IsComplex())	cfn1 = cmpexp; else fn1 = expf;
	else if (fname == "db")		fn1 = aux_db;
	else if (fname == "sign")	fn1 = aux_sign;
	else if (fname == "asin")	fn1 = asinf;
	else if (fname == "acos")	fn1 = acosf;
	else if (fname == "atan")	fn1 = atanf;
	else if (fname == "round")	fn1 = aux_round;
	else if (fname == "fix")	fn1 = aux_fix;
	else if (fname == "ceil")	fn1 = ceilf;
	else if (fname == "floor")	fn1 = floorf;

	if (fname == "sqrt" || fname == "log10" || fname == "log")
	{
		if (Sig.IsComplex() || Sig._min() < 0)
		{
			Sig.SetComplex();
			Sig.each(cfn1);
		}
		else
			Sig.each(fn1);
	}
	else if (fname == "pow" || fname == "^" || fname == "mod" || fname == "%")
	{
		if (Sig.IsComplex() || Sig._min() < 0)
		{
			Sig.SetComplex();
			Sig.each(cfn2, arg);
		}
		else
			Sig.each(fn2, arg);
	}
	else
	{
		if (Sig.IsComplex() || Sig._min() < 0)
		{
			Sig.SetComplex();
			Sig.each(cfn1);
		}
		else
			Sig.each(fn1);
	}
}
