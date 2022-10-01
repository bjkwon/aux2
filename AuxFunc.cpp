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

string CAstSigEnv::AppPath = "";


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

Cfunction set_builtin_function_colon(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "(scalar:scalar) or (scalar:scalar:scalar)", "(scalar:scalar) or (scalar:scalar:scalar)" };
	vector<string> desc_arg_opt = { "(scalar:scalar) or (scalar:scalar:scalar)" };
	vector<CVar> default_arg = { CVar(string("doesn't matter")), /*CVar(default_value), CVar(string("default_str")), same number of desc_arg_opt*/};
	set<uint16_t> allowedTypes1 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _colon(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	float val1 = past->Sig.value();
	float val2 = args[0].value();
	float step;
	if (ISSCALAR(args.back().type()))
		step = args[1].value();
	else
		step = (val1 > val2) ? -1.f : 1.f;
	int nItems = max(1, (int)((val2 - val1) / step) + 1);
	past->Sig.UpdateBuffer(nItems);
	for (int k = 0; k < nItems; k++)
		past->Sig.buf[k] = val1 + step * k;
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


#define SET_BUILTIN_FUNC(AUXNAME,GATENAME) builtin.emplace(AUXNAME, set_builtin_function_##GATENAME(&_##GATENAME));
#define SET_PSEUDO_VARS(AUXNAME,GATENAME) pseudo_vars.emplace(AUXNAME, set_builtin_function_constant(&_##GATENAME));

void CAstSigEnv::InitBuiltInFunctions()
{
	//Think about this 11/19/2021
//	srand((unsigned)time(0) ^ (unsigned int)GetCurrentThreadId());

	string name;
	Cfunction ft;
	set<uint16_t> allowedTypes;
	vector<string> arg_desc;

	SET_BUILTIN_FUNC("tone", tone);
	SET_BUILTIN_FUNC("noise", tparamonly);
	SET_BUILTIN_FUNC("gnoise", tparamonly);
	SET_BUILTIN_FUNC("dc", tparamonly);
	SET_BUILTIN_FUNC("silence", tparamonly);
	SET_BUILTIN_FUNC("wave", wave);
	SET_BUILTIN_FUNC("wavwrite", wavwrite);
	SET_BUILTIN_FUNC("ramp", ramp);
	SET_BUILTIN_FUNC("sam", sam);
	SET_BUILTIN_FUNC("pow", pow);
	SET_BUILTIN_FUNC("^", pow);
	SET_BUILTIN_FUNC("mod", mod);
	SET_BUILTIN_FUNC("%", mod);
	SET_BUILTIN_FUNC("group", group);
	SET_BUILTIN_FUNC("ungroup", group);
	SET_BUILTIN_FUNC("matrix", group);
	SET_BUILTIN_FUNC("fft", fft);
	SET_BUILTIN_FUNC("ifft", ifft);
	SET_BUILTIN_FUNC(":", colon);
	SET_BUILTIN_FUNC("min", minmax);
	SET_BUILTIN_FUNC("max", minmax);
	SET_BUILTIN_FUNC("sum", sums);
	SET_BUILTIN_FUNC("mean", sums);
	SET_BUILTIN_FUNC("stdev", sums); // to do---support unnormalized stdev (break out of sums and make one for stdev)
	SET_BUILTIN_FUNC("length", lens);
	SET_BUILTIN_FUNC("size", lens);
	SET_BUILTIN_FUNC("rmsall", rmsetc);
	SET_BUILTIN_FUNC("rms", rmsetc);
	SET_BUILTIN_FUNC("begint", rmsetc);
	SET_BUILTIN_FUNC("endt", rmsetc);
	SET_BUILTIN_FUNC("dur", rmsetc);
	SET_BUILTIN_FUNC("ones", onezero);
	SET_BUILTIN_FUNC("zeros", onezero);
	SET_BUILTIN_FUNC("and", andor);
	SET_BUILTIN_FUNC("and", andor2);
	SET_BUILTIN_FUNC("or", andor);
	SET_BUILTIN_FUNC("or", andor2);
	SET_BUILTIN_FUNC("sort", sort);
	SET_BUILTIN_FUNC("atmost", mostleast);
	SET_BUILTIN_FUNC("atleast", mostleast);
	SET_BUILTIN_FUNC("hamming", hamming);
	SET_BUILTIN_FUNC("blackman", blackman);
	SET_BUILTIN_FUNC("filthann", blackman);
	SET_BUILTIN_FUNC("", filt);
	SET_BUILTIN_FUNC("filtfilt", filt);
	SET_BUILTIN_FUNC("conv", conv);
	SET_BUILTIN_FUNC("audio", audio);
	SET_BUILTIN_FUNC("vector", vector);
	SET_BUILTIN_FUNC("left", leftright);
	SET_BUILTIN_FUNC("right", leftright);
	SET_BUILTIN_FUNC("hilbert", hilbenvlope);
	SET_BUILTIN_FUNC("envlope", hilbenvlope);
	SET_BUILTIN_FUNC("fopen", fopen);
	SET_BUILTIN_FUNC("fclose", fclose);
	SET_BUILTIN_FUNC("printf", printf);
	SET_BUILTIN_FUNC("fprintf", fprintf);
	SET_BUILTIN_FUNC("fread", fread);
	SET_BUILTIN_FUNC("fwrite", fwrite);
	SET_BUILTIN_FUNC("write", write);
	SET_BUILTIN_FUNC("file", file);
	SET_BUILTIN_FUNC("json", json);

	SET_BUILTIN_FUNC("clear", clear);

	SET_BUILTIN_FUNC("resample", resample);
	SET_BUILTIN_FUNC("cell", rand);
	SET_BUILTIN_FUNC("rand", rand);
	SET_BUILTIN_FUNC("irand", irand);
	SET_BUILTIN_FUNC("randperm", randperm);
	SET_BUILTIN_FUNC("dir", dir);
	SET_BUILTIN_FUNC("issame", veq);
	SET_BUILTIN_FUNC("otype", datatype);
	SET_BUILTIN_FUNC("eval", eval);
	SET_BUILTIN_FUNC("diff", diff);
	SET_BUILTIN_FUNC("cumsum", cumsum);
	
	// Functions with no argument
	SET_BUILTIN_FUNC("getfs", noargs);
	SET_BUILTIN_FUNC("tic", noargs);
	SET_BUILTIN_FUNC("toc", noargs);

	SET_BUILTIN_FUNC("head", cellstruct);
	SET_BUILTIN_FUNC("ismember", cellstruct);
	SET_BUILTIN_FUNC("erase", cellstruct);

	//SET_BUILTIN_FUNC("", );

	SET_PSEUDO_VARS("i", imaginary_unit);
	SET_PSEUDO_VARS("e", natural_log_base);
	SET_PSEUDO_VARS("pi", pi);

//	name = "setfs"; // check this... is narg1 one correct?
//	ft.alwaysstatic = true;
//	ft.funcsignature = "(filename)";
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.func =  &_setfs;
//	builtin[name] = ft;
//
//	name = "squeeze";
//	ft.alwaysstatic = false;
//	ft.funcsignature = "() ... to remove the null interval";
//	ft.narg1 = 0;	ft.narg2 = 0;
//	ft.func =  &_squeeze; // do this
//	builtin[name] = ft;
//
//
//	name = "isaudioat";
//	ft.funcsignature = "(audio_signal, time_pt)";
//	ft.func =  &_isaudioat;
//	builtin[name] = ft;
//
//	// end narg 2 and 2
//
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
//	ft.funcsignature = "(graphic_handle)";
//	name = "replicate";
//	ft.func = &_replicate;
//	builtin[name] = ft;
//
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
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(filename)";
//	name = "include";
//	ft.func =  &_include; // check
//	builtin[name] = ft;
//	name = "fdelete";
//	ft.func =  &_fdelete; // check
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
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(string)";
//	name = "str2num";
//	ft.func =  &_str2num;
//	builtin[name] = ft;
//	ft.alwaysstatic = true;
//

	ft.narg1 = 1;	ft.narg2 = 1;
	ft.funcsignature = "(value_or_array)";
	const char *fmath1[] = { "abs", "sqrt", "conj", "real", "imag", "angle", "sign", "sin", "cos", "tan", "asin", "acos", "atan", "log", "log10", "exp", "round", "fix", "ceil", "floor", 0 };
	for (int k = 0; fmath1[k]; k++)
	{
		name = fmath1[k];
		ft.func =  NULL;
		builtin.emplace(name, ft);
	}

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

static bool this_is_one_of_allowedset(uint16_t thistype, const vector<set<uint16_t>>::const_iterator& allowed)
{
	for (auto it = allowed->begin(); it != allowed->end(); it++)
	{
		if (thistype == *it) return true;
	}
	return false;
}

void skope::make_check_args_math(const AstNode* pnode)
{
	ostringstream ostr;
	set<uint16_t> allowed = { 1, 2, 3, ALL_AUDIO_TYPES };
	vector<set<uint16_t>> allowedvector;
	allowedvector.push_back(allowed);
	auto it = allowedvector.begin();
	if (!this_is_one_of_allowedset(Sig.type(), it)) {
		ostr << "type " << Sig.type();
		throw exception_func(*this, pnode, ostr.str(), pnode->str).raise();
	}
}

vector<CVar> skope::make_check_args(const AstNode* pnode, const Cfunction& func, void* pexc )
{ // Goal: make args vector for the function gate
  // the first arg is always Sig. The second arg is the first output vector.
  // check the arg types; if a type of a given arg is not one of allowed ones, throw.
  // check minimum, maximum number of args; if the number of given args is outside the range, throw. 
  // fill default arguments in the arg vector if not specified
	vector<CVar> out;
	ostringstream ostr;

	bool struct_call = pnode->type == N_STRUCT;
	string fname = pnode->str;
	if (func.allowed_arg_types.empty() && !pnode->alt && !pnode->child)
		return out;
	skope_exception* exc = (skope_exception*)pexc;
	if (func.alwaysstatic && struct_call)
	{
		ostr << "function " << fname << " does not allow . (dot) notation call.";
		*exc = exception_func(*this, pnode, ostr.str(), fname);
		return out;
	}
	if (Sig.type() == TYPEBIT_NULL && func.narg1 == 0 && func.narg2 > 0)
		Sig = func.defaultarg.front();
	vector<set<uint16_t>>::const_iterator allowedset = func.allowed_arg_types.begin();
	// 0xFFFF accepts all type; i.e., 0xFFFF bypasses checking
	if (*allowedset->begin() != 0xFFFF && !this_is_one_of_allowedset(Sig.type(), allowedset))
	{
		ostr << "type " << Sig.type();
		*exc = exception_func(*this, pnode, ostr.str(), fname, 1);
		return out;
	}
	allowedset++;
	int count = 2;
	const AstNode* p2 = get_second_arg(pnode, struct_call);
	for (auto pn = p2; pn; pn = pn->next, count++)
	{
		if (func.narg2 >= 0 && count > func.narg2)
		{
			ostr << fname << "(): too many args; maximum number of args is " << func.narg2 << ".";
			*exc = exception_etc(*this, pnode, ostr.str());
			return out;
		}
		skope smallskope(this);
		try {
			smallskope.Compute(pn);
			if (func.narg2 < 0) // unspecified max arg count, no type checking
				out.push_back(smallskope.Sig);
			else
			{
				// for arguments with any type (e.g., fwrite), bypass checking with 0xFFFF and add the result of Compute to the out vector
				if (*allowedset->begin() == 0xFFFF || this_is_one_of_allowedset(smallskope.Sig.type(), allowedset))
				{
					out.push_back(smallskope.Sig);
				}
				else
				{
					ostr << "type " << smallskope.Sig.type();
					*exc = exception_func(*this, pnode, ostr.str(), fname, count);
					return out;
				}
				allowedset++;
			}
		}
		catch (skope_exception e) {
			throw e;
		}
	}
	count--;
	if (count < func.narg1)
	{
		ostr << fname << "(): the number of arg should be at least " << func.narg1 << "; Only " << count << " given.";
		*exc = exception_etc(*this, pnode, ostr.str());
		return out;
	}
	for (; count < func.narg2; count++)
		out.push_back(CVar(func.defaultarg[count - func.narg1]));
	return out;
}

const AstNode* find_parentnode_alt(const AstNode* pnode, const AstNode* pRoot0)
{
	// determine if pnode is part of N_VECTOR nodes from pRoot0
	for (auto p = pRoot0; p; p = p->alt)
	{
		if (p->type==N_VECTOR && p->alt== pnode)
			return p;
		if (p->alt == pnode)
			return p;
	}
	return pnode;
}

const AstNode* arg0node(const AstNode* pnode, const AstNode* pRoot0)
{
	if (pnode->type == N_STRUCT)
	{
		return find_parentnode_alt(pnode, pRoot0);
	}
	else
	{
		if (pnode->alt)
		{
			if (pnode->alt->type == N_ARGS)
				return pnode->alt->child;
			else if (pnode->alt->type == N_HOOK)
				return pnode->alt;
		}
		return NULL;
	}
}

#include "skope_exception.h"

void skope::HandleAuxFunctions(const AstNode *pnode, AstNode *pRoot)
{
	string fnsigs;
	string fname = pnode->str;
	auto ftlist = pEnv->builtin.find(fname);
	bool structCall = pnode->type == N_STRUCT;
	const AstNode* arg0 = arg0node(pnode, node);
	if (!structCall)
	{
		Compute(arg0); // Update Sig with arg0; For strucCall, it was already done in read_node()
		if (!arg0)
			Sig.Reset();
	}
	if ((*ftlist).second.func)
	{
		exception_func exc(*this, pnode, "");
		exc.msgonly.clear();
		vector<CVar> args = make_check_args(pnode, (*ftlist).second, &exc);
		if (!exc.msgonly.empty()) {
			ftlist++;
			if (ftlist != pEnv->builtin.end()) {
				exc.msgonly.clear();
				args = make_check_args(pnode, (*ftlist).second, &exc);
				if (!exc.msgonly.empty()) throw exc.raise();
			}
			else
				throw exc.raise();
		}
		/* IMPORTANT--
		* When the gate function is called, Sig is always the first argument (this is to avoid going through a copy constructor when used inside a gate function)
		* Just make sure to avoid calling Compute() inside the gate function before Sig is used for actual builtin function operation
		* If you still need to call Compute(); make a copy of Sig and do so and accept that a tiny performance degradation may occur
		*/

		// if arg0 is the only arg and is null, bypass everything and the output is null
		if (args.size()!=0 || 
			Sig.type()!=TYPEBIT_NULL ||
			((*ftlist).second.narg1==0 && args.empty()) ||
			(*(*ftlist).second.allowed_arg_types.front().begin())==0xFFFF)
			(*ftlist).second.func(this, pnode, args);
	}
	else
	{
		if (Sig.IsStruct() || !Sig.cell.empty() || Sig.GetFs() == 3)
		{
			if (structCall)
			{
				string dotnotation = dotstring(pnode, pRoot);
				throw exception_etc(*this, pnode, string("Cannot take cell, class or handle object " + dotnotation)).raise();
			}
			else
				throw exception_etc(*this, arg0, string("Cannot take cell, class or handle object " + fname)).raise();
		}
		make_check_args_math(pnode);
		const AstNode* p2 = get_second_arg(pnode, pnode->type == N_STRUCT);
		if (p2)
		{ // only for those math functions taking two args, e.g., mod, pow
			skope smallskope(this);
			smallskope.Compute(p2);
			smallskope.make_check_args_math(p2);
			HandleMathFunc(fname, smallskope.Sig);
		}
		else
			HandleMathFunc(fname, CVar()); // just a dummy arg
	}
	Sig.functionEvalRes = true;
	return;
}

Cfunction set_builtin_function_clear(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "object"  };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 0xFFFF};
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _clear(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	auto arg0 = arg0node(pnode, past->node);
	if (arg0->type==T_ID)
		past->ClearVar(arg0->str);
	else
		throw exception_etc(*past, pnode, "Only variables can be cleared.").raise();
	past->Sig.Reset();

}

