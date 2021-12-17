// AUXLAB
//
// Copyright (c) 2009-2019 Bomjun Kwon (bjkwon at gmail)
// Licensed under the Academic Free License version 3.0
//
// Project: sigproc
// Signal Generation and Processing Library
// Platform-independent (hopefully)
//
// Version: 1.7
// Date: 5/17/2020
//
//#include <math.h>
//#include <stdlib.h>
//#include <string.h> // aux_file
//#include <time.h>
#include <sstream>
//#include <vector>
//#include <map>
//#include <unordered_map>
#include <stdbool.h>

#include "aux_classes.h"
#include "skope.h"
#include "skope_exception.h"

// 10/1/2020 TO DO-----separate these graffy function somehow to add its own features
// such as blockNULL etc.

#include "builtin_functions.h"

bool CAstSigEnv::IsValidBuiltin(const string& funcname)
{
	if (pseudo_vars.find(funcname) != pseudo_vars.end())
		return false;
	return builtin.find(funcname) != builtin.end();
}

//void _nullin(skope *past, const AstNode *pnode)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	past->checkAudioSig(pnode, past->Sig);
//	CVar sig = past->Sig;
//	past->Compute(p);
//	past->checkScalar(p, past->Sig);
//	sig.NullIn(past->Sig.value());
//	past->Sig = sig;
//}

void _contig(skope *past, const AstNode *pnode)
{
	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
	past->Sig.MakeChainless();
}

void _tictoc(skope *past, const AstNode *pnode)
{
	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
	string fname = pnode->str;
	if (fname == "tic")
	{
		past->Sig.SetValue((float)past->tic()); // Internal timer value--not really useful for the user other than showing it was working
	}
	else
	{
		past->Sig.SetValue((float)past->toc(pnode)); // milliseconds since tic was called.
	}
}

void _time_freq_manipulate(skope *past, const AstNode *pnode)
{
	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
	//check qualifers
//	past->checkAudioSig(pnode, past->Sig);
	CVar param, paramopt;
	string fname;
	try {
		skope tp(past);
		param = tp.Compute(p);
		if (p->next)
		{
			paramopt = tp.Compute(p->next);
			if (paramopt.strut.empty())
				throw exception_func(*past, p, "must be a struct variable", "", 3).raise();
		}
		int type = param.GetType();
		//if (type!= CSIG_TSERIES && type != CSIG_SCALAR)
		//	throw CAstException(FUNC_SYNTAX, *past, p).proc("parameter must be either a scalar or a time sequence.");
		// Think about this 11/19/2021
//		if (param.GetType() == CSIG_TSERIES)
		{
			float audioDur = past->Sig.dur();
			if (param.GetFs() == 0) // relative
				for (CTimeSeries *p = &param; p; p = p->chain)
				{
					p->tmark *= audioDur;
					p->SetFs(past->Sig.GetFs());
				}
			//If the first tmark is not 0, make one with 0 tmark and bring the value at zero
			if (param.tmark != 0.)
			{
				CTimeSeries newParam(past->Sig.GetFs());
				newParam.tmark = 0.;
				newParam.SetValue(param.value());
				newParam.chain = new CTimeSeries;
				*newParam.chain = param; // this way the copied version goes to chain
				param = newParam;
			}
			//If the last tmark is not the end of the signal, make one with 0 tmark and bring the value at zero
			CTimeSeries *pLast = NULL;
			for (CTimeSeries *p = &param; p; p = p->chain)
				if (!p->chain)
					pLast = p;
			if (pLast && pLast->tmark != past->Sig.dur())
			{
				CTimeSeries newParam(past->Sig.GetFs());
				newParam.tmark = past->Sig.dur();
				newParam.SetValue(pLast->value());
				pLast->chain = new CTimeSeries;
				*pLast->chain = newParam; // this way the copied version goes to chain
			}
			//IF tsequence goes beyond the audio duration, cut it out.
			for (CTimeSeries *p = &param; p; p = p->chain)
				if (p->chain && p->chain->tmark > audioDur)
				{
					delete p->chain;
					p->chain = NULL;
				}
		}
		fname = pnode->str;
		for (auto it = paramopt.strut.begin(); it != paramopt.strut.end(); it++)
			param.strut[(*it).first] = (*it).second;
		//if (fname == "respeed")
		//	past->Sig.evoke_modsig(&CSignal::resample, &param);
		//else if (fname == "movespec")
		//	past->Sig.evoke_modsig(&CSignal::movespec, &param);
		//if (param.IsString())
		//	throw exception_etc(*this, pnode, string("Error in respeed:") + param.string())).raise();
	}
	catch (const skope_exception &e) {
		throw exception_etc(*past, pnode, e.getErrMsg()).raise(); }
	if (fname == "respeed")
	{ // Take care of overlapping chains after processing
		past->Sig.MergeChains();
	}
}

void _colon(skope *past, const AstNode *pnode)
{
	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
	CVar first = past->Sig;
	if (!first.IsScalar()) throw exception_etc(*past, pnode, "Colon: All arguments must be scalars (check 1st arg).").raise();
	float val1, val2, step;
	CVar third, second = past->Compute(p->next);
	if (!second.IsScalar()) throw exception_etc(*past, pnode, "Colon: All arguments must be scalars (check 2nd arg).").raise();
	val1 = first.value();
	val2 = second.value();
	if (p->next->next) {
		third = past->Compute(p->next->next);
		if (!third.IsScalar()) throw exception_etc(*past, pnode, "Colon: All arguments must be scalars (check 3rd arg).").raise();
		step = third.value();
	}
	else
		step = (val1>val2) ? -1.f : 1.f;
	past->Sig.Reset(1);
	int nItems = max(1, (int)((val2-val1)/step)+1);
	past->Sig.UpdateBuffer(nItems);
	for (int i=0; i<nItems; i++)
		past->Sig.buf[i] = val1 + step*i;
}

//void _clear(skope *past, const AstNode *pnode)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	past->ClearVar((AstNode *)past->xtree, &past->Sig);
//	past->Sig.Reset();
//}

void _squeeze(skope *past, const AstNode *pnode)
{
	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
	past->Sig.Squeeze();
}

AstNode *searchstr(AstNode *p, int findthis)
{ // if there's a node with "type" in the tree, return that node
	if (p)
	{
		if (p->type == findthis) return p;
		if (p->child)
		{
			if (p->child->type == findthis) return p->child;
			else return searchstr(p->child, findthis);
		}
		else if (p->next)
		{
			if (p->next->type == findthis) return p->next;
			else return searchstr(p->next, findthis);
		}
	}
	return NULL;
}

AstNode *searchstr(AstNode *p, const char* pstr)
{ // if there's a node with "type" in the tree, return that node
	if (p)
	{
		if (p->str==pstr) return p;
		if (p->child)
		{
			if (p->child->str == pstr) return p->child;
			else return searchstr(p->child, pstr);
		}
		else if (p->next)
		{
			if (p->next->str == pstr) return p->next;
			else return searchstr(p->next, pstr);
		}
	}
	return NULL;
}

int findcol(AstNode *past, const char* pstr, int line)
{
	for (AstNode *pn=past; pn; pn = pn->next)
	{
		if (pn->line<line) continue;
		AstNode *pm = searchstr(pn->child, pstr);
		if (pm) return pm->col;
		else	return -1;
	}
	return -1;
}

//vector<string> CAstSigEnv::InitBuiltInFunctionsExt(const vector<string>& externalModules)
//{
//	// If there's an error (including file not found), it's reported back to the caller.
//	vector<string> out;
//	char ext[256];
//	for (auto libname : externalModules)
//	{
//		string fname = libname;
//		_splitpath(libname.c_str(), NULL, NULL, NULL, ext);
//		if (!strcmp(ext,""))
//			fname += ".dll";
//		HANDLE hLib = LoadLibrary(fname.c_str());
//		if (hLib)
//		{
//			auto pt = (map<string, Cfunction>(_cdecl*)()) GetProcAddress((HMODULE)hLib, (LPCSTR)MAKELONG(1, 0)); // Init()
//			if (pt)
//			{
//				map<string, Cfunction> res = pt();
//				for (auto it = res.begin(); it != res.end(); it++)
//				{
//					builtin[(*it).first] = (*it).second;
//				}
//			}
//			else
//				out.push_back(fname + string(" does not have Init()."));
//		}
//		else
//			out.push_back(fname + string(" not found."));
//	}
//	return out;
//}

string Cfunction::make_funcsignature()
{
	string out;
	return out;
}

Cfunction set_builtin_function_tone(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;

	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "f", "dur", };
	vector<string> desc_arg_opt = { "phase", };
	vector<CVar> default_arg = { CVar(0.f), };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { 1, 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);

	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_noise(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;

	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "dur", };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes1);

	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_wave(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;

	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "filename", };
	vector<string> desc_arg_opt = { "start_time", "end_time", };
	vector<CVar> default_arg = { CVar(0.f), CVar(-1.f) };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { 50, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);

	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_ramp(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj", "ramp_dur"};
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_AUDIO + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_pow(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "value_or_array", "value_or_array" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 1, 2, 10, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}


Cfunction set_builtin_function_mod(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "value_or_array", "value_or_array" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 1, 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_wavwrite(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_signal", "filename" };
	vector<string> desc_arg_opt = { "option" };
	vector<CVar> default_arg = { CVar(string(""))};
	set<uint16_t> allowedTypes1 = { TYPEBIT_AUDIO + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 50, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void CAstSigEnv::InitBuiltInFunctions()
{
	//Think about this 11/19/2021
//	srand((unsigned)time(0) ^ (unsigned int)GetCurrentThreadId());

	string name;
	Cfunction ft;
	set<uint16_t> allowedTypes;
	vector<string> arg_desc;

	builtin["tone"] = set_builtin_function_tone(&_tone);
	builtin["noise"] = set_builtin_function_noise(&_tparamonly);
	builtin["gnoise"] = set_builtin_function_noise(&_tparamonly);
	builtin["dc"] = set_builtin_function_noise(&_tparamonly);
	builtin["silence"] = set_builtin_function_noise(&_tparamonly);
	builtin["wave"] = set_builtin_function_wave(&_wave);
	builtin["ramp"] = set_builtin_function_ramp(&_ramp);
	builtin["pow"] = set_builtin_function_pow(&_pow);
	builtin["^"] = set_builtin_function_pow(&_pow);
	builtin["mod"] = set_builtin_function_mod(&_mod);
	builtin["%"] = set_builtin_function_mod(&_mod);
	
	builtin["wavwrite"] = set_builtin_function_wavwrite(&_wavwrite);

//	name = "sam";
//	ft.alwaysstatic = false;
//	ft.funcsignature = "(signal, AM_rate[, AM_depth_m = 1, initial_phase_between_0 & 1 = 0])";
//	ft.narg1 = 2;	ft.narg2 = 4;
//	ft.func =  &_sam;
//	builtin[name] = ft;
//

//	ft.narg1 = 2;	ft.narg2 = 3;
//	ft.alwaysstatic = true;
//	name = ":";
//	ft.funcsignature = "(scalar:scalar) or (scalar:scalar:scalar)";
//	ft.func =  &_colon;
//	builtin[name] = ft;
//
//	name = "wavwrite";
//	ft.alwaysstatic = false;
//	ft.funcsignature = "(audio_signal, filename[, option])";
//	ft.narg1 = 2;	ft.narg2 = 3;
//	ft.func =  &_wavwrite;
//	builtin[name] = ft;
//
//	name = "write";
//	ft.alwaysstatic = false;
//	ft.funcsignature = "(audio_signal, filename[, option])";
//	ft.narg1 = 2;	ft.narg2 = 3;
//	ft.func = &_write;
//	builtin[name] = ft;
//
//	name = "setfs"; // check this... is narg1 one correct?
//	ft.alwaysstatic = true;
//	ft.funcsignature = "(filename)";
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.func =  &_setfs;
//	builtin[name] = ft;
//
//	name = "audio";
//	ft.alwaysstatic = false;
//	ft.funcsignature = "(non_audio_vector)";
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.func =  &_audio;
//	builtin[name] = ft;
//
//	name = "squeeze";
//	ft.alwaysstatic = false;
//	ft.funcsignature = "() ... to remove the null interval";
//	ft.narg1 = 0;	ft.narg2 = 0;
//	ft.func =  &_squeeze; // do this
//	builtin[name] = ft;
//
//	// begin narg 2 and 2
//	ft.alwaysstatic = false;
//	ft.narg1 = 2;
//	ft.narg2 = 2;
//
//	name = "ramp";
//	ft.funcsignature = "(signal, ramp_duration)";
//	ft.func =  &_ramp;
//	builtin[name] = ft;
//
//	name = "isaudioat";
//	ft.funcsignature = "(audio_signal, time_pt)";
//	ft.func =  &_isaudioat;
//	builtin[name] = ft;
//
//	// end narg 2 and 2
//
//	ft.narg1 = 1;	ft.narg2 = 1;
//	name = "hamming";
//	ft.funcsignature = "(audio_signal)";
//	ft.func = &_hamming;
//	builtin[name] = ft;
//	name = "hann";
//	ft.func = &_blackman;
//	builtin[name] = ft;
//	ft.narg1 = 1;	ft.narg2 = 2;
//	name = "blackman";
//	ft.funcsignature = "(audio_signal, [alpha=0.16])";
//	ft.func = &_blackman;
//	builtin[name] = ft;
//
//#ifndef NO_IIR
//	ft.narg1 = 2;	ft.narg2 = 6;
//	const char *f2[] = { "lpf", "hpf", 0 };
//	ft.funcsignature = "(signal, freq, [order=4], [kind=1], [dB_passband_ripple=0.5], [dB_stopband_atten=-40])\n  --- kind: 1 for Butterworth, 2 for Chebyshev, 3 for Elliptic";
//	for (int k = 0; f2[k]; k++)
//	{
//		name = f2[k];
//		ft.func =  &_iir;
//	builtin[name] = ft;
//	}
//	ft.narg1 = 3;	ft.narg2 = 7;
//	const char *f3[] = { "bpf", "bsf", 0 };
//	ft.funcsignature = "(signal, freq, [order=4], [kind=1], [dB_passband_ripple=0.5], [dB_stopband_atten=-40])\n  --- kind: 1 for Butterworth, 2 for Chebyshev, 3 for Elliptic";
//	for (int k = 0; f3[k]; k++)
//	{
//		name = f3[k];
//		ft.func =  &_iir;
//	builtin[name] = ft;
//	}
//#endif //NO_IIR
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(stereo_signal)";
//	name = "left";
//	ft.func =  &_left; // check
//	builtin[name] = ft;
//	name = "right";
//	ft.func =  &_right; // check
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 2;
//	name = "std";
//	ft.funcsignature = "(length, [, bias])";
//	ft.func =  &_std;
//	builtin[name] = ft;
//
//	ft.narg1 = 2;	ft.narg2 = 2;
//	name = "setnextchan";
//	ft.func = &_setnextchan;
//	builtin[name] = ft;
//	const char *f4[] = { "atleast", "atmost", 0 };
//	ft.funcsignature = "(array_or_value, array_or_value_of_limit)";
//	for (int k = 0; f4[k]; k++)
//	{
//		name = f4[k];
//		ft.func = &_mostleast;
//	builtin[name] = ft;
//	}
//	ft.funcsignature = "(obj, member_variable_string)";
//	name = "erase";
//	ft.func = &_erase;
//	builtin[name] = ft;
//	ft.funcsignature = "(obj, member_variable_string)";
//	name = "ismember";
//	ft.func = &_ismember;
//	builtin[name] = ft;
//	ft.funcsignature = "(obj, any_non-cell_non-class_expression)";
//	name = "head";
//	ft.func = &_head;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 1;
//	const char *f5[] = { "min", "max", 0 };
//	ft.funcsignature = "(vector) or (scalar1, scalar2, ...)";
//	for (int k = 0; f5[k]; k++)
//	{
//		name = f5[k];
//		ft.func =  &_minmax;
//	builtin[name] = ft;
//	}
//
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(array)";
//	const char *f6[] = { "sum", "mean", "length", 0 };
//	for (int k = 0; f6[k]; k++)
//	{
//		name = f6[k];
//		ft.func = _arraybasic;
//	builtin[name] = ft;
//	}
//
//	ft.narg1 = 1;	ft.narg2 = 2;
//	ft.funcsignature = "(array [, dimension_either_1_or_2])";
//	name = "size";
//	ft.func = &_size;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(graphic_handle)";
//	name = "replicate";
//	ft.func = &_replicate;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 2;
//	ft.funcsignature = "(array_or_signal[, order=1])";
//	name = "diff";
//	ft.func =  &_diff;
//	builtin[name] = ft;
//
//	ft.narg1 = 2;	ft.narg2 = 2;
//	ft.funcsignature = "(audio_signal)";
////	name = "nullin";
////	ft.func = &_nullin;
////	builtin[name] = ft;
//	ft.narg1 = 1;	ft.narg2 = 1;
//	name = "contig";
//	ft.func = &_contig;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(array_or_signal)";
//	name = "cumsum";
//	ft.func = &_cumsum;
//	builtin[name] = ft;
//
//	ft.funcsignature = "(value)";
//	name = "rand";
//	ft.func =  &_rand;
//	builtin[name] = ft;
//	name = "irand";
//	ft.func =  &_irand;
//	builtin[name] = ft;
//	name = "randperm";
//	ft.func =  &_randperm;
//	builtin[name] = ft;
//	name = "zeros";
//	ft.func =  &_zeros;
//	builtin[name] = ft;
//	name = "ones";
//	ft.func =  &_ones;
//	builtin[name] = ft;
//	name = "cell";
//	ft.func =  &_cell;
//	builtin[name] = ft;
////	name = "clear";
////	ft.func = &_clear;
////	builtin[name] = ft;
//
//	ft.narg1 = 2;	ft.narg2 = 2;
//	name = "group";
//	ft.funcsignature = "(array, nGroups)";
//	ft.func =  &_group;
//	builtin[name] = ft;
//	name = "matrix"; // kept for backward compatibility
//	builtin[name] = ft; // kept for backward compatibility
//	name = "ungroup";
//	ft.funcsignature = "(array, overlap)";
//	ft.func = &_ungroup;
//	builtin[name] = ft;
//	ft.narg1 = 2;	ft.narg2 = 3;
//	name = "buffer";
//	ft.funcsignature = "(array, blocklen[, overlap=0])";
//	ft.func = &_buffer;
//	builtin[name] = ft;
//
//
//	ft.narg1 = 2;	ft.narg2 = 2;
//	name = "respeed";
//	ft.funcsignature = "(audio_signal, playback_rate_change_ratio)";
//	ft.func = &_time_freq_manipulate;
//	builtin[name] = ft;
//
//	ft.narg1 = 3;	ft.narg2 = 3;
//	name = "interp";
//	ft.funcsignature = "(refX, refY, query_x_points)";
//	ft.func = &_interp1;
//	builtin[name] = ft;
//
//	ft.alwaysstatic = true;
//	ft.narg1 = 2;	ft.narg2 = 0;
//	ft.funcsignature = "(format_string, ...)";
//	name = "sprintf";
//	ft.func =  &_sprintf;
//	builtin[name] = ft;
//	name = "fprintf";
//	ft.alwaysstatic = false;
//	ft.func =  &_fprintf;
//	builtin[name] = ft;
//
//	ft.narg1 = 2;	ft.narg2 = 3;
//	name = "fread";
//	ft.func = &_fread;
//	builtin[name] = ft;
//	ft.narg1 = 3;	ft.narg2 = 3;
//	name = "fwrite";
//	ft.func = &_fwrite;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(file_ID)";
//	name = "fclose";
//	ft.func =  &_fclose;
//	builtin[name] = ft;
//
//	ft.narg1 = 2;	ft.narg2 = 2;
//	name = "fopen";
//	ft.alwaysstatic = true;
//	ft.funcsignature = "(filename, mode)";
//	ft.func = &_fopen; // check
//	builtin[name] = ft;
//
//	ft.narg1 = 0;	ft.narg2 = 0;
//	name = "getfs";
//	ft.funcsignature = "()";
//	ft.func =  &_getfs;
//	builtin[name] = ft;
//	name = "tic";
//	ft.func = &_tictoc;
//	builtin[name] = ft;
//	name = "toc";
//	ft.func = &_tictoc;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(filename)";
//	name = "file";
//	ft.func =  &_file;
//	builtin[name] = ft;
//	name = "include";
//	ft.func =  &_include; // check
//	builtin[name] = ft;
//	name = "fdelete";
//	ft.func =  &_fdelete; // check
//	builtin[name] = ft;
//
//	ft.narg1 = 0;	ft.narg2 = 1;
//	ft.funcsignature = "(directory_name)";
//	name = "dir";
//	ft.func =  &_dir;
//	builtin[name] = ft;
//
//	ft.alwaysstatic = false;
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(object)";
//	const char *f7[] = { "isempty", "isaudio", "isvector", "isstring", "isstereo", "isbool", "iscell", "isclass", "istseq", 0 };
//	for (int k = 0; f7[k]; k++)
//	{
//		name = f7[k];
//		ft.func = _varcheck;
//		builtin[name] = ft;
//	}
//	ft.narg1 = 1;	ft.narg2 = 1;
//	name = "otype";
//	ft.func = _datatype;
//	builtin[name] = ft;
//	ft.alwaysstatic = true;
//	ft.narg1 = 2;	ft.narg2 = 2;
//	name = "issame";
//	ft.func = _veq;
//	builtin[name] = ft;
//
//	ft.alwaysstatic = false;
//	ft.narg1 = 2;	ft.narg2 = 4;
//	ft.funcsignature = "(signal, Numerator_array [, Denominator_array=1 for FIR])";
//	const char *f8[] = { "filt", "filtfilt", 0 };
//	for (int k = 0; f8[k]; k++)
//	{
//		name = f8[k];
//		ft.func = _filt;
//		builtin[name] = ft;
//	}
//#ifndef NO_FFTW
//
//	ft.narg1 = 1;	ft.narg2 = 2;
//	name = "fft";
//	ft.funcsignature = "(array, [Num_of_FFT_pts=length_of_the_input])";
//	ft.func =  &_fft; // check
//	builtin[name] = ft;
//	name = "ifft";
//	ft.func = &_ifft;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(signal_or_vector)";
//	name = "envelope";
//	ft.func =  &_envelope;
//	builtin[name] = ft;
//	name = "hilbert";
//	ft.func =  &_hilbert;
//	builtin[name] = ft;
//
//	ft.narg1 = 2;	ft.narg2 = 2;
//	name = "movespec";
//	ft.funcsignature = "(audio_signal, frequency_to_shift)";
//	ft.func = &_time_freq_manipulate;
//	builtin[name] = ft;
//
//	name = "conv";
//	ft.funcsignature = "(array1, array2)";
//	ft.func = &_conv;
//	builtin[name] = ft;
//#endif
//
//	ft.narg1 = 1;	ft.narg2 = 2;
//	ft.funcsignature = "(signal_or_vector [, positive_for_acending_negative_for_descending = 1])";
//	name = "sort";
//	ft.func =  &_sort;
//	builtin[name] = ft;
//
//#ifndef NO_PLAYSND
//	ft.alwaysstatic = false;
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(audio_handle)";
//	name = "pause";
//	ft.func = &_pause_resume;
//	builtin[name] = ft;
//	name = "resume";
//	ft.func = &_pause_resume;
//	builtin[name] = ft;
//	name = "stop";
//	ft.func = &_stop;
//	builtin[name] = ft;
//	name = "qstop";
//	ft.func = &_stop;
//	builtin[name] = ft;
//
//	ft.funcsignature = "(audio_signal [, repeat=1]) or (audio_handle, audio_signal [, repeat=1])";
//	ft.narg1 = 1;	ft.narg2 = 3;
//	name = "play";
//	ft.func =  &_play;
//	builtin[name] = ft;
//	ft.funcsignature = "(deviceID, recording_duration [=-1;indefinite], mono1_or_stereo2 [=1], callback_duration_ms [=setting in ini])";
//	ft.narg1 = 1;	ft.narg2 = 4;
//	name = "record";
//	ft.func = &_record;
//#endif
//	builtin[name] = ft;
//	ft.funcsignature = "(audio_signal)";
//	ft.alwaysstatic = false;
//	ft.narg1 = 1;	ft.narg2 = 1;
//	name = "vector";
//	ft.func =  &_vector;
//	builtin[name] = ft;
//	name = "dur";
//	ft.func = &_arraybasic;
//	builtin[name] = ft;
//	name = "begint";
//	ft.func = &_arraybasic;
//	builtin[name] = ft;
//	name = "endt";
//	ft.func = &_arraybasic;
//	builtin[name] = ft;
//	name = "rms";
//	ft.func = &_arraybasic;
//	builtin[name] = ft;
//	name = "rmsall";
//	ft.func = &_arraybasic;
//	builtin[name] = ft;
//
//	ft.alwaysstatic = false;
//	ft.narg1 = 1;	ft.narg2 = 1;
//	name = "tsq_isrel";
//	ft.funcsignature = "(tseq)";
//	ft.func = &_tsq_isrel;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 2;
//	name = "tsq_gettimes";
//	ft.funcsignature = "(tseq [, audio_signal])";
//	ft.func = &_tsq_gettimes;//
//	builtin[name] = ft;
//
//	ft.narg1 = 2;	ft.narg2 = 2;
//	name = "tsq_settimes";
//	ft.funcsignature = "(tseq, times_vector_or_tseq)";
//	ft.func = &_tsq_settimes;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 1;
//	name = "tsq_getvalues";
//	ft.funcsignature = "(tseq)";
//	ft.func = &_tsq_getvalues;
//	builtin[name] = ft;
//
//	ft.narg1 = 2;	ft.narg2 = 2;
//	name = "tsq_setvalues";
//	ft.funcsignature = "(tseq, values_matrix)";
//	ft.func = &_tsq_setvalues;
//	builtin[name] = ft;
//
//	ft.alwaysstatic = true;
//	name = "msgbox";
//	ft.narg1 = 1;	ft.narg2 = 0;
//	ft.funcsignature = "(msg_body_with_printf_style)";
//	ft.func =  &_msgbox;
//	builtin[name] = ft;
//
//	ft.alwaysstatic = false;
//	name = "inputdlg";
//	ft.narg1 = 2;	ft.narg2 = 99;
//	ft.funcsignature = "(title, msg_body)";
//	ft.func =  &_inputdlg;
//	builtin[name] = ft;
//
//	ft.alwaysstatic = false;
//	ft.narg1 = 1;	ft.narg2 = 2;
//	ft.funcsignature = "(logical_variable) or (logical_variable1, logical_variable2) ";
//	name = "and";
//	ft.func =  &_and;
//	builtin[name] = ft;
//	name = "or";
//	ft.func =  &_or;
//	builtin[name] = ft;
//
//	ft.alwaysstatic = false;
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(string)";
//	name = "str2num";
//	ft.func =  &_str2num;
//	builtin[name] = ft;
//	ft.alwaysstatic = true;
//	name = "eval";
//	ft.func =  &_eval;
//	builtin[name] = ft;
//

	ft.narg1 = 1;	ft.narg2 = 1;
	ft.funcsignature = "(value_or_array)";
	const char *fmath1[] = { "abs", "sqrt", "conj", "real", "imag", "angle", "sign", "sin", "cos", "tan", "asin", "acos", "atan", "log", "log10", "exp", "round", "fix", "ceil", "floor", 0 };
	for (int k = 0; fmath1[k]; k++)
	{
		name = fmath1[k];
		ft.func =  NULL;
		builtin[name] = ft;
	}

//#ifdef _WINDOWS
//	ft.alwaysstatic = true;
//	ft.narg1 = 0;	ft.narg2 = 1;
//	name = "figure";
//	ft.funcsignature = "([position(screen_coordinate)]) or (existing_Figure_ID)";
//	ft.func =  &_figure;
//	builtin[name] = ft;
//
//	ft.alwaysstatic = false;
//	ft.narg1 = 1;	ft.narg2 = 2;
//	name = "axes";
//	ft.funcsignature = "([position]) or (existing_axes_ID)";
//	ft.func =  &_axes;
//	builtin[name] = ft;
//
//	ft.narg1 = 3;	ft.narg2 = 4;
//	ft.funcsignature = "([graphic_handle], x, y, string)";
//	name = "text";
//	ft.func = &_text;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 4;
//	ft.funcsignature = "([graphic_handle], variable [, options]) or (vector_x, vector_y [, options])";
//	name = "plot";
//	ft.func = &_plot;
//	builtin[name] = ft;
//	name = "line";
//	ft.func = &_line;
//	builtin[name] = ft;
//
//	name = "delete";
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.alwaysstatic = false;
//	ft.funcsignature = "(graphic_handle)";
//	ft.func = &_delete_graffy;
//	builtin[name] = ft;
//	name = "repaint";
//	ft.funcsignature = "(graphic_handle)";
//	ft.func = &_repaint;
//	builtin[name] = ft;
//	name = "showrms";
//	ft.funcsignature = "(graphic_handle)";
//	ft.func = &_showrms;
//	builtin[name] = ft;
//#endif // _WINDOWS

	//name = "input";
	//ft.alwaysstatic = false;
	//ft.funcsignature = "(prompt_message)";
	//ft.func = &aux_input;
	//builtin[name] = ft;

	//name = "error";
	//ft.alwaysstatic = false;
	//ft.narg1 = ft.narg2 = 1;
	//ft.funcsignature = "(message)";
	//ft.func = &udf_error;
	//builtin[name] = ft;
	//name = "warning";
	//ft.func = &udf_warning;
	//builtin[name] = ft;
	//name = "throw";
	//ft.func = &udf_rethrow;
	//builtin[name] = ft;
	//name = "esc";
	//ft.funcsignature = "(string)";
	//ft.func = &_esc;
	//builtin[name] = ft;


	//name = "i";
	//ft.alwaysstatic = false;
	//ft.narg1 = ft.narg2 = 0;
	//ft.funcsignature = "";
	//ft.func = &_imaginary_unit;
	//pseudo_vars[name] = ft;
	//name = "e";
	//ft.func = &_natural_log_base;
	//pseudo_vars[name] = ft;
	//name = "pi";
	//ft.func = &_pi;
	//pseudo_vars[name] = ft;

}

void firstparamtrim(string &str)
{
	size_t parenth1 = str.find('(');
	size_t parenth2 = str.find(')');
	size_t comma = str.find(',', parenth1);
	string tocopy(")");
	if (comma != string::npos)
		tocopy = str.substr(comma + 2);
	str.replace(str.begin() + parenth1 + 1, str.end(), tocopy);
	str.replace(str.begin() + 1, str.end(), str);
	str[0] = '.';
}

static inline complex<float> r2c_sqrt(complex<float> x) { return sqrt(x); }
static inline complex<float> r2c_log(complex<float> x) { return log(x); }
static inline complex<float> r2c_log10(complex<float> x) { return log10(x); }
static inline complex<float> r2c_pow(complex<float> x, complex<float> y) { return pow(x, y); }

bool isgraphicfunc(string fname)
{
	if (fname == "figure") return true;
	if (fname == "text") return true;
	if (fname == "delete") return true;
	if (fname == "plot") return true;
	if (fname == "axes") return true;
	return false;
}

string dotstring(const AstNode *pnode, AstNode *pRoot)
{
	string out;
	AstNode *p = pRoot;
	while (p)
	{
		if (!out.empty()) out += '.';
		out += p->str;
		p = p->alt;
		if (p == pnode) break;
	}
	return out;
}

void check(const skope& ths, const AstNode* pnode, const vector<CVar>& args, const Cfunction& fn, const string& fname)
{
	int ind = 0;
	auto allowedset = fn.allowed_arg_types.begin();
	for (auto arg : args)
	{
		auto thistype = arg.type();
		bool pass = false;
		for (auto it = allowedset->begin(); it!= allowedset->end(); it++)
		{
			pass = thistype == *it;
			if (pass) break;
		}
		ind++;
		if (!pass)
		{
			ostringstream emsgstr;
			emsgstr << "type " << thistype;
			throw exception_func(ths, pnode, emsgstr.str(), fname, ind).raise();
		}
		allowedset++;
	}
}

vector<CVar> skope::make_check_args(const AstNode* pnode, const Cfunction& func)
{ // make arg vector for the function gate
  // check the arg types; if a type of a given arg is not one of allowed ones, throw.
  // check minimum, maximum number of args; if the number of given args is outside the range, throw. 
  // fill default arguments in the arg vector if not specified
	vector<CVar> out;
	ostringstream ostr;

	// if pnode is N_STRUCT, Sig was already computed, no need to redo it.
	// Just get the 

	bool struct_call = pnode->type == N_STRUCT;
	const AstNode* p2 = get_second_arg(pnode, struct_call);
	string fname = pnode->str;
	uint16_t thistype;
	int count = 1;
	auto allowedset = func.allowed_arg_types.begin();
	thistype = Sig.type();
	bool pass0 = false;
	for (auto it = allowedset->begin(); it != allowedset->end(); it++)
	{
		pass0 = thistype == *it;
		if (pass0)
			break;
	}
	if (!pass0)
	{
		ostr << "type " << thistype;
		throw exception_func(*this, pnode, ostr.str(), fname, count).raise();
	}
	allowedset++;
	count++;
	for (const AstNode* pn = p2; pn; pn = pn->next, count++)
	{
		if (count > func.narg2)
		{
			ostr << fname << "(): too many args; maximum number of args is " << func.narg2 << ".";
			throw exception_etc(*this, pnode, ostr.str()).raise();
		}
		skope smallskope(this);
		smallskope.Compute(pn);
		thistype = smallskope.Sig.type();
		bool pass = false;
		for (auto it = allowedset->begin(); it != allowedset->end(); it++)
		{
			pass = thistype == *it;
			if (pass)
			{
				out.push_back(smallskope.Sig);
				break;
			}
		}
		if (!pass)
		{
			ostr << "type " << thistype;
			throw exception_func(*this, pnode, ostr.str(), fname, count).raise();
		}
		allowedset++;
	}
	count--;
	if (count > func.narg2)
	{
		ostr << fname << "(): minimum number of args " << func.narg1 << "; only " << count << " given.";
		throw exception_etc(*this, pnode, ostr.str()).raise();
	}
	for (; count < func.narg2; count++)
		out.push_back(CVar(func.defaultarg[count - func.narg1]));
	return out;
}

void skope::HandleAuxFunctions(const AstNode *pnode, AstNode *pRoot)
{
	string fnsigs;
	int res, nArgs;
	string fname = pnode->str;
	auto ft = pEnv->builtin.find(fname);
	bool structCall = pnode->type == N_STRUCT;
	AstNode * p(NULL);
	if (pnode->alt)
	{
		if (pnode->alt->type == N_ARGS)
			p = pnode->alt->child;
		else if (pnode->alt->type == N_HOOK)
			p = pnode->alt;
	}
	if (!structCall)
		Compute(p);
	if ((*ft).second.func)
	{
		vector<CVar> args = make_check_args(pnode, (*ft).second);

		/* IMPORTANT--
		* When the gate function is called, Sig is always the first argument (this is to avoid going through a copy constructor when used inside a gate function)
		* Just make sure to avoid calling Compute() inside the gate function before Sig is used for actual builtin function operation
		* If you still need to call Compute(); make a copy of Sig and do so and accept that a tiny performance degradation may occur
		*/
		(*ft).second.func(this, pnode, args);
	}
	else
	{
		if (structCall)
		{
			if (p && p->type != N_STRUCT) throw exception_etc(*this, pnode, string("Too many arg : " + fname)).raise();
		}
		if (Sig.IsStruct() || !Sig.cell.empty() || Sig.GetFs() == 3)
		{
			if (structCall)
			{
				string dotnotation = dotstring(pnode, pRoot);
				throw exception_etc(*this, pnode, string("Cannot take cell, class or handle object " + dotnotation)).raise();
			}
			else
				throw exception_etc(*this, p, string("Cannot take cell, class or handle object " + fname)).raise();
		}
		const AstNode* p2 = get_second_arg(pnode, pnode->type == N_STRUCT);
		if (p2)
		{ // only for those math functions taking two args, e.g., mod, pow
			skope smallskope(this);
			smallskope.Compute(p2);
			HandleMathFunc(fname, smallskope.Sig);
		}
		else
			HandleMathFunc(fname, CVar()); // just a dummy arg
	}
	Sig.functionEvalRes = true;
	return;
}
