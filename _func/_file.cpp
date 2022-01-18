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
// Date: 5/24/2020

#include <string.h> // aux_file
#include "functions_common.h"

#include <algorithm>
#include <cassert>
#include "lame.h" // mp3data_struct
#include "lame_aux_api.h"
//#include "samplerate.h"
#include "sndfile.h"
//#include "utils.h"
int str2vector(vector<string>& out, const string& in, const string& delim_chars);


int GetFileText(const char* fname, const char* mod, string& strOut); // utils.cpp
int countDeliminators(const char* buf, const char* deliminators);
size_t str2vect(vector<string>& _out, const char* input, const char* delim, int maxSize_x);

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
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_file(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "filename", };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
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
	vector<CVar> default_arg = { CVar(string("")) };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
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

Cfunction set_builtin_function_fopen(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "filename", "mode"};
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_fclose(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "FILE_ID" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_BYTE + 2};
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_fread(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "FILE_ID", "precision", };
	vector<string> desc_arg_opt = { "additional_arg"};
	vector<CVar> default_arg = { CVar(string("")) };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_BYTE + 2 };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_fwrite(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "FILE_ID", "object", "precision", };
	vector<string> desc_arg_opt = { "additional_arg" };
	vector<CVar> default_arg = { CVar(string("")) };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_BYTE + 2 };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 0xFFFF }; // bypass
	ft.allowed_arg_types.push_back(allowedTypes2);
	set<uint16_t> allowedTypes3 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes3);
	ft.allowed_arg_types.push_back(allowedTypes3);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_write(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "object", "filename" };
	vector<string> desc_arg_opt = { "option" };
	vector<CVar> default_arg = { CVar(string("")) };
	set<uint16_t> allowedTypes1 = { 1, 2, 3, TYPEBIT_COMPLEX + 1, TYPEBIT_COMPLEX + 2, TYPEBIT_COMPLEX + 3, 
		TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, TYPEBIT_STRING + 3,
		TYPEBIT_SIZE1 + 1, TYPEBIT_SIZE1 + 2, TYPEBIT_SIZE1 + 3,
		ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
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

static inline bool isnumeric(const char* buf)
{
	for (size_t k = 0; k < strlen(buf); k++)
	{
		if (buf[k] <= 0 && buf[k] >= 9)
			continue;
		else
		{
			if (buf[k] != '\0' && buf[k] != '\t')
				return false;
		}
	}
	return true;
}

static void EnumAudioVariables(skope* past, vector<string>& var)
{
	var.clear();
	for (map<string, CVar>::iterator it = past->Vars.begin(); it != past->Vars.end(); it++)
		if (ISAUDIO(it->second.type())) var.push_back(it->first);
}

void _fopen(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string filename = past->Sig.str();
	string mode = args[0].str();
	FILE* fl;
	if (!(fl = fopen(filename.c_str(), mode.c_str())))
	{
		throw exception_etc(*past, pnode, "fopen failed.").raise();
	}
	else
	{
		past->Sig.Reset(2);
		past->Sig.strbuf = new char[sizeof(uint64_t)];
		memcpy(past->Sig.buf, &fl, sizeof(uint64_t));
		past->Sig.nSamples = sizeof(uint64_t);
		past->Sig.bufType = 'B';
	}
}

void _fclose(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
//	auto sz = sizeof(INT_PTR);
//	char* tempbuf = new char[sz];
//	memcpy(tempbuf, past->Sig.buf, sz);
	auto fp = (FILE*)*(uint64_t*)past->Sig.buf;
//	delete[] tempbuf;
	if (fclose(fp) == EOF)
	{
		past->Sig.SetValue(-1.);
	}
	else
	{
		past->Sig.SetValue(0);
	}
}

/* fwrite: audio signal --> rescale from -1 to 1 to each integer range corresponding to the format,
   e.g., -32768 to 32767 for int16, 0 to 65535 for uint16
   if it is stereo, writes the data in an interleaved manner for each channel
   nonaudio signal --> write as is.. don't care whether it is outside of the range.

   fread: reads the data according to the format, e.g., -2^31 to 2^31 for int32, makes a non-audio object
   if the last arg is "a" or "audio," it rescales in the range and makes the object audio (mono)
   if the last arg is "a2" or "audio2," it rescales in the range and makes the object audio (stereo)
*/

static FILE* prepare_freadwrite(skope* past, const AstNode* pnode, const vector<CVar>& args, int& bytes, string& prec, char* additional)
{
	//first arg is always file identifier
	//second arg is the signal to write to file
	//third arg is precision--one of the following: int8 int16 int32 uint8 uint16 uint32 char float float

	auto fp = (FILE*)*(uint64_t*)past->Sig.buf;
	string fname = pnode->str;
	auto arg = args.begin();
	if (fname == "fwrite") arg++;
	prec = (*arg).str();
	if (prec == "int8" || prec == "uint8" || prec == "char" || prec == "byte")
		bytes = 1;
	else if (prec == "int16" || prec == "uint16")
		bytes = 2;
	else if (prec == "int24")
		bytes = 3;
	else if (prec == "float" || prec == "int32" || prec == "uint32")
		bytes = 4;
	else if (prec == "double")
		bytes = 8;
	else
		throw exception_func(*past, pnode, "Second arg must be one of the following: char byte int16 uint16 float int32 uint32 double", fname).raise();
	if (fname == "fread")
		strcpy(additional, (*++arg).str().c_str());
	return fp;
}

template<typename T>
size_t fwrite_general_floating(T var, const CVar& sig, string prec, FILE* file)
{
	int k = 0;
	T* pvar = &var;
	if (sig.next)
	{
		float* buf2 = sig.next->buf;
		for_each(sig.buf, sig.buf + sig.nSamples,
			[buf2, pvar, file, &k](float v) {
				*pvar = (T)v; fwrite(pvar, sizeof(T), 1, file);
				*pvar = (T)buf2[k++]; fwrite(pvar, sizeof(T), 1, file); });
	}
	else
	{
		for_each(sig.buf, sig.buf + sig.nSamples,
			[pvar, file](float v) { *pvar = (T)v; fwrite(pvar, sizeof(T), 1, file); });
	}
	return sig.nSamples;
}

template<typename T>
size_t fwrite_general(T var, const CVar& sig, string prec, FILE* file, int bytes, uint64_t factor)
{
	auto tp = sig.type();
	if (!ISAUDIO(tp))
		factor = 1;
	int k = 0;
	T* pvar = &var;
	if (sig.next)
	{
		float* buf2 = sig.next->buf;
		for_each(sig.buf, sig.buf + sig.nSamples,
			[buf2, pvar, factor, bytes, file, &k](float v) {
				*pvar = (T)(factor * v - .5); fwrite(pvar, bytes, 1, file);
				*pvar = (T)(factor * buf2[k++] - .5); fwrite(pvar, bytes, 1, file); });
	}
	else
	{
		if (ISAUDIO(tp))
			for_each(sig.buf, sig.buf + sig.nSamples,
				[pvar, bytes, factor, file](float v) { *pvar = (T)(factor * v - .5); fwrite(pvar, bytes, 1, file); });
		else
			for_each(sig.buf, sig.buf + sig.nSamples,
				[pvar, bytes, factor, file](float v) { *pvar = (T)(v - .5); fwrite(pvar, bytes, 1, file); });
	}
	return sig.nSamples;
}

void _fwrite(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	CVar firstarg = args[0];
	int bytes;
	string prec;
	size_t res=-1;
	FILE* file = prepare_freadwrite(past, pnode, args, bytes, prec, NULL);
	const CVar* pobj = NULL;
	auto tp = args[0].type();
	if (ISAUDIO(tp))
	{
		if (tp & TYPEBIT_TEMPO_CHAINS || (tp & TYPEBIT_TEMPO_ONE && args[0].tmark > 0))
		{
			CVar copy = args[0];
			copy.MakeChainless();
			pobj = &copy;
		}
	}
	if (!pobj) pobj = &args[0];

	if (prec == "char")
	{
		if (pobj->IsString())
			res = fwrite(pobj->strbuf, 1, pobj->nSamples, file);
		else
		{
			char temp = 0;
			res = fwrite_general(temp, *pobj, prec, file, bytes, 0x100);
		}
	}
	else if (prec == "int8")
	{
		int8_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0x80);
	}
	else if (prec == "uint8")
	{
		uint8_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0x100);
	}
	else if (prec == "int16")
	{
		int16_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0x8000);
	}
	else if (prec == "uint16")
	{
		uint16_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0x10000);
	}
	//else if (prec == "int24")
	//{
	//	int32_t temp = 0; // in24_t doesn't exist
	//	res = fwrite_general(temp, *pobj, prec, file, bytes, 0x800000);
	//}
	else if (prec == "int32")
	{
		int32_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0x80000000);
	}
	else if (prec == "uint32")
	{
		uint32_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0xffffffff);
	}
	else if (prec == "float")
	{ // No automatic scaling
		float temp = 0;
		res = fwrite_general_floating(temp, *pobj, prec, file);
	}
	else if (prec == "double")
	{ // No automatic scaling
		double temp = 0;
		res = fwrite_general_floating(temp, *pobj, prec, file);
	}
	past->Sig.SetValue((float)res);
}

template<typename T>
void fread_general(T var, CVar& sig, FILE* file, int bytes, char* addarg, uint64_t factor)
{
	T* pvar = &var;
	if (!strcmp(addarg, "audio") || !strcmp(addarg, "a"))
		for_each(sig.buf, sig.buf + sig.nSamples,
			[pvar, bytes, file, factor](float& v) { fread(pvar, bytes, 1, file); v = *pvar / (float)factor; });
	else if (!strcmp(addarg, "audio2") || !strcmp(addarg, "a2"))
	{
		int k = 0;
		CSignals next(CSignal(sig.GetFs(), sig.nSamples));
		sig.SetNextChan(next);
		float* buf2 = sig.next->buf;
		for_each(sig.buf, sig.buf + sig.nSamples,
			[buf2, pvar, bytes, file, factor, &k](float& v) {
				fread(pvar, bytes, 1, file); v = *pvar / (float)factor;
				fread(pvar, bytes, 1, file); buf2[k++] = *pvar / (float)factor; });
	}
	else
		for_each(sig.buf, sig.buf + sig.nSamples,
			[pvar, bytes, file](float& v) { fread(pvar, bytes, 1, file); v = *pvar; });
}

void _fread(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	int bytes;
	string prec;
	char addarg[16] = {};
	FILE* file = prepare_freadwrite(past, pnode, args, bytes, prec, addarg);

	auto res = fseek(file, 0L, SEEK_END);
	size_t sz = ftell(file);
	res = fseek(file, 0L, SEEK_SET);

	size_t nItems = sz / bytes;
	if (prec == "char" || prec == "byte")
	{ // Treat it separately just to make the code neat.
		past->Sig.SetString('\0');
		past->Sig.UpdateBuffer((unsigned int)nItems);
		fread(past->Sig.strbuf, bytes, nItems, file);
		if (prec == "byte") past->Sig.bufType = 'B';
		return;
	}
	past->Sig.Reset(1); // always make it non-audio
	if (!strcmp(addarg, "audio2") || !strcmp(addarg, "a2"))
	{
		if (nItems / 2 * 2 != nItems)
			throw exception_etc( *past, pnode, "attempting to read stereo audio data but data count is not even.").raise();
		nItems /= 2;
	}
	past->Sig.UpdateBuffer((unsigned int)nItems);
	if (!strcmp(addarg, "audio") || !strcmp(addarg, "a") || !strcmp(addarg, "audio2") || !strcmp(addarg, "a2"))
		past->Sig.SetFs(past->pEnv->Fs);
	if (prec == "int8" || prec == "uint8")
	{
		int8_t temp = 0;
		fread_general(temp, past->Sig, file, bytes, addarg, 0x80);
	}
	else if (prec == "int16" || prec == "uint16")
	{
		int16_t temp = 0;
		fread_general(temp, past->Sig, file, bytes, addarg, 0x8000);
	}
	else if (prec == "int24")
	{
		int32_t temp = 0; // in24_t doesn't exist
		fread_general(temp, past->Sig, file, bytes, addarg, 0x80000000); // check
	}
	else if (prec == "int32" || prec == "uint32")
	{
		int32_t temp = 0;
		fread_general(temp, past->Sig, file, bytes, addarg, 0x80000000);
	}
	else if (prec == "float")
	{
		float temp = 0.;
		fread_general(temp, past->Sig, file, bytes, addarg, 1);
	}
	else if (prec == "float")
	{
		float temp = 0.;
		fread_general(temp, past->Sig, file, bytes, addarg, 1);
	}
	if (!strcmp(addarg, "audio") || !strcmp(addarg, "a") | !strcmp(addarg, "audio2") || !strcmp(addarg, "a2"))
		past->Sig.SetFs(past->pEnv->Fs);
}


static void write2textfile(FILE* fid, CVar* psig)
{
	if (psig->bufBlockSize == 1)
	{
		for (unsigned int k = 0; k < psig->nSamples; k++)
			fprintf(fid, "%c ", psig->logbuf[k]);
		fprintf(fid, "\n");
	}
	else if (ISAUDIO(psig->type())) // audio
	{
		for (unsigned int k = 0; k < psig->nSamples; k++)
			fprintf(fid, "%7.4f ", psig->buf[k]);
		if (psig->next)
		{
			fprintf(fid, "\n");
			for (unsigned int k = 0; k < psig->nSamples; k++)
				fprintf(fid, "%7.4f ", psig->next->buf[k]);
		}
		fprintf(fid, "\n");
	}
	else if (!psig->cell.empty())
	{
		for (auto cel : psig->cell)
			write2textfile(fid, &cel);
	}
	else
	{
		for (unsigned int k = 0; k < psig->nSamples; k++)
			fprintf(fid, "%g ", psig->buf[k]);
		fprintf(fid, "\n");
	}
}

static SNDFILE* read_wav_info(const string& wavname, SF_INFO& sfinfo, string& errstr)
{
	sfinfo = {};
	SNDFILE* wavefileID;
	if ((wavefileID = sf_open(wavname.c_str(), SFM_READ, &sfinfo)) == NULL)
	{
		auto errlibsndfile = sf_strerror(wavefileID);
		errstr = "Unable to open audio file ";
		errstr += wavname + " -- " + errlibsndfile;
	}
	else
		errstr = "";
	return wavefileID;
}

void _wavwrite(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string filename = args[0].str();
	string option = args[1].str();

	string fullfilename = past->makefullfile(filename, ".wav");
	char errStr[256];
	past->Sig.MakeChainless();
	if (!past->Sig.Wavwrite(fullfilename.c_str(), errStr, option))
		throw exception_etc(*past, pnode, errStr).raise();
}

void _write(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string filename = args[0].str();
	string option = args[1].str();
//	trim(filename, ' '); // is it necessary? 1/9/2022
	size_t pdot = filename.rfind('.');
	string extension = filename.substr(pdot + 1);
	csignals_mp3_aiff x;
	if (extension == "mp3")
	{
		/* For now, MakeChainless makes the whole audio data into one big piece.
		   Compared to wavwrite, short null portions in the middle should not be handled
		   separately, instead save into mp3 as the part of the audio data.
		   If the null portions in the middle are long, it would be better to
		   convert those null portions into separate silent portions and save each piece into
		   separate mp3 block, but as of today, I'm not sure how to do it.
		   Maybe call lame_encoder_loop in lame_main.c for each block while FILE * outf is open?
		   Let's figure it out later. 10/3/2019
		   */
		past->Sig.MakeChainless();
		char errStr[256] = { 0 };
		x.fs = past->Sig.GetFs();
		x.fbuf_l = past->Sig.buf;
		x.fbuf_r = past->Sig.next? past->Sig.next->buf : NULL;
		x.floatbuf = 1;
		x.length = past->Sig.nSamples;
		int res = write_mp3(&x, filename.c_str(), errStr);
		if (!res)
			throw exception_func(*past, pnode, errStr).raise();
	}
	else if (extension == "wav")
	{
		_wavwrite(past, pnode, args);
	}
	else if (extension == "txt")
	{
		FILE* fid = fopen(filename.c_str(), "wt");
		if (!fid)
			throw exception_etc(*past, pnode, "File creation error").raise();
		write2textfile(fid, &past->Sig);
		fclose(fid);
	}
	else
		throw exception_func(*past, pnode, "The extension must be specified .wav .mp3 or .txt").raise();
}

CSignal __resample(const CSignal& base, void* pargin, void* pargout); // from movespec.cpp; pargin is vector<CVar>

static void resample_if_fs_different(skope* past, const AstNode* p)
{ // call this function only if past->Sig still has the active signal
	vector<string> audiovars;
	ostringstream out;
	EnumAudioVariables(past, audiovars);
	if (audiovars.size() > 0)
	{
		if (past->Sig.GetFs() != past->GetFs())
		{
			int oldFs = past->GetFs();
			CVar ratio(1), argout(1);
			ratio.SetValue(past->Sig.GetFs() / (float)oldFs);
			vector<CVar> arg(1, ratio);
			past->Sig = past->Sig.evoke_modsig2(__resample, &arg, &argout);
			if (ISSTRING(argout.type())) // this means there was an error during resample
				throw exception_etc(*past, p, argout.str()).raise();
			out << "(NOTE)File fs=" << past->Sig.GetFs() << " Hz. The audio data resampled to " << oldFs << " Hz.";
			past->statusMsg = out.str();
			past->Sig.SetFs(oldFs);
		}
	}
	else
	{
		past->pEnv->Fs = past->Sig.GetFs();
		out << "(NOTE)Sample Rate of AUXLAB Environment is now set to " << past->pEnv->Fs << " Hz.";
		past->statusMsg = out.str();
	}
}

void _wave(skope *past, const AstNode *pnode, const vector<CVar>& args)
{
	string estr;
	string filename = past->Sig.str();
	float beginMs = args[0].value();
	float durMs = args[1].value();
	SF_INFO sfinfo; 
	SNDFILE* wavefileID = read_wav_info(filename, sfinfo, estr);
	if (!wavefileID)
		throw exception_etc(past, pnode, estr).raise();
	
	sf_count_t count;
	past->Sig.Reset(sfinfo.samplerate);
	sf_count_t id1 = (sf_count_t)(beginMs / 1000.f * sfinfo.samplerate + .5);
	sf_seek(wavefileID, id1, SEEK_SET);
	int _frames2read;
	if (durMs < 0)
		_frames2read = sfinfo.frames - id1;
	else
		_frames2read = (sf_count_t)(durMs / 1000.f * sfinfo.samplerate + .5) - id1;
	sf_count_t frames2read = max(_frames2read, 0);
	if (sfinfo.channels == 1)
	{
		past->Sig.UpdateBuffer((uint64_t)frames2read);
		count = sf_readf_float(wavefileID, past->Sig.buf, frames2read);  // sndfile.h
	}
	else
	{
		float* buffer = new float[sfinfo.channels * (int)frames2read];
		count = sf_readf_float(wavefileID, buffer, frames2read);  // sndfile.h
		float(*buf3)[2];
		past->Sig.next = new CSignals(sfinfo.samplerate);
		int m(0);
		buf3 = (float(*)[2]) & buffer[m++];
		past->Sig.UpdateBuffer((int)frames2read);
		for (unsigned int k = 0; k < frames2read; k++)			past->Sig.buf[k] = buf3[k][0];
		buf3 = (float(*)[2]) & buffer[m++];
		past->Sig.next->UpdateBuffer((int)frames2read);
		for (unsigned int k = 0; k < frames2read; k++)			past->Sig.next->buf[k] = buf3[k][0];
		delete[] buffer;
	}
	sf_close(wavefileID);
	resample_if_fs_different(past, pnode);
}

static int filetype(const string& fname, string& errstr)
{
	FILE* fp = fopen(fname.c_str(), "rb");
	if (!fp) { errstr = "File not found or cannot be opened.";  return 0; }
	char buffer[16];
	auto res = fread(buffer, 1, sizeof(buffer), fp);
	if (res < sizeof(buffer)) { errstr = "File type header is corrupt or cannot be read.";  fclose(fp); return 0; }
	errstr = "";
	auto ret = buffer[0] == (char)0xFF;
	auto ret2 = buffer[1] >= (char)0xE0;
	auto ret3 = buffer[1] <= (char)0xFF;
	if (!memcmp(buffer, "RIFF", 4) && !memcmp(buffer + 8, "WAVE", 4))
		res = 1; // WAV
	else if (!memcmp(buffer, "ID3", 3) || (buffer[0] == (char)0xFF && (char)0xE0 <= buffer[1] && buffer[1] <= (char)0xFF))
		res = 2; // MP3
	else if (!strncmp(buffer, "FORM", 5))
		res = 3; // AIFF
	else
		res = 4;
	fclose(fp);
	return res;
}

static uint64_t estimate_buffersize(const string& filename, int fs, mp3data_struct* mp3info) //int nBytes, int nChans, int bitrate, int framesize)
{
	uint64_t res=0;
	// get the file size
	FILE* fp = fopen(filename.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	long filesize = ftell(fp);
	fclose(fp);
	float factor = (fs / 44100.f) * (mp3info->stereo/ 2.f) * (mp3info->bitrate / 1411.2f);
	float dur_of_standard = filesize / 176400.f;
	float est_dur = dur_of_standard / factor;
	res = (uint64_t)(est_dur * 44100);
	return res;
}

void _file(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	char errStr[16];
	string filename = past->Sig.str();
	string errstr, content;
	int res = filetype(filename, errstr);
	int ret, fs, nChans;
	size_t len, nLines;
	mp3data_struct mp3info;
	csignals_mp3_aiff x;
	switch (res)
	{
	case 1:
		//as of now Jan 2022, args is empty
		// supplying default args for _wave()
		((vector<CVar>*) & args)->push_back(CVar(0.f));
		((vector<CVar>*) & args)->push_back(CVar(-1.f));
		_wave(past, pnode, args);
		resample_if_fs_different(past, pnode);
		break;
	case 2:
	case 3:
		read_mp3_header(filename.c_str(), &len, &nChans, &fs, &mp3info, errStr);
		past->Sig.Reset(fs);
		if ((int)len < 0)
			len = estimate_buffersize(filename.c_str(), fs, &mp3info);
		past->Sig.UpdateBuffer(len);
		if (nChans > 1)
		{
			past->Sig.SetNextChan(CSignals(fs));
			past->Sig.next->UpdateBuffer(len);
		}
		x.floatbuf = 1;
		x.fbuf_l = past->Sig.buf;
		x.fbuf_r = (nChans > 1) ? past->Sig.next->buf : NULL;
		x.length = len;
		if (!(ret = read_mp3(&x, filename.c_str(), errStr)))
			throw exception_func(*past, pnode, errStr).raise();
		past->Sig.nSamples = ret;
		past->Sig.buf = x.fbuf_l;
		if (nChans > 1) {
			past->Sig.next->nSamples = ret;
			past->Sig.next->buf = x.fbuf_r;
		}
		resample_if_fs_different(past, pnode);
		break;
	case 4:
		if (GetFileText(filename.c_str(), "rb", content))
		{
			past->Sig.Reset();
			vector<string> line;
			nLines = str2vector(line, content, "\r\n");
			char buf[256];
			for (size_t k = 0; k < nLines; k++)
			{
				//if there's at least one non-numeric character except for space and tab, treat the whole line as a string.
				//if (!isnumeric(line[k].c_str()))
				//{
				//	past->Sig.appendcell((CVar)line[k]);
				//}
				//else
				{
					vector<string> datavect;
					res = str2vector(datavect, line[k], " \t");
					float* data = new float[res];
					k = 0;
					for (auto str : datavect)
					{
						if (sscanf(str.c_str(), "%f", data + k++)==EOF)
							throw exception_func(*past, pnode, "Error in reading and converting to float data").raise();
					}
					CSignals tp(data, res);
					if (nLines == 1)
						past->Sig = tp;
					//else
					//	past->Sig.appendcell((CVar)tp);
					delete[] data;
				}
			}
		}
		else
			throw exception_func(*past, pnode, "Cannot read file").raise();
		break;
	}
}


int CSignals::Wavwrite(const char* wavname, char* errstr, std::string wavformat)
{
	SF_INFO sfinfo;
	SNDFILE* wavefileID;
	sfinfo.channels = (next) ? 2 : 1;
	if (wavformat.length() == 0)
		sfinfo.format = SF_FORMAT_WAV + SF_FORMAT_PCM_16; // default
	else if (wavformat == "8")
		sfinfo.format = SF_FORMAT_WAV + SF_FORMAT_PCM_U8;
	else if (wavformat == "16")
		sfinfo.format = SF_FORMAT_WAV + SF_FORMAT_PCM_16;
	else if (wavformat == "24")
		sfinfo.format = SF_FORMAT_WAV + SF_FORMAT_PCM_24;
	else if (wavformat == "32")
		sfinfo.format = SF_FORMAT_WAV + SF_FORMAT_PCM_32;
	else if (wavformat == "ulaw")
		sfinfo.format = SF_FORMAT_WAV + SF_FORMAT_ULAW;
	else if (wavformat == "alaw")
		sfinfo.format = SF_FORMAT_WAV + SF_FORMAT_ALAW;
	else if (wavformat == "adpcm1")
		sfinfo.format = SF_FORMAT_WAV + SF_FORMAT_IMA_ADPCM;
	else if (wavformat == "adpcm2")
		sfinfo.format = SF_FORMAT_WAV + SF_FORMAT_MS_ADPCM;
	//	else if (wavformat=="vorbis")
	//		sfinfo.format = SF_FORMAT_OGG + SF_FORMAT_VORBIS; // not available ...  ogg.c requires external lib which I don't have yet. bjkwon 03/19/2016
	else
	{
		sprintf(errstr, "Supported waveformat---8, 16, 24, 32, ulaw, alaw, adpcm1 or adpcm2.\n");
		return 0;
	}
	sfinfo.frames = nSamples;
	sfinfo.samplerate = fs;
	sfinfo.sections = sfinfo.seekable = 1;
	if ((wavefileID = sf_open(wavname, SFM_WRITE, &sfinfo)) == NULL)
	{
		sprintf(errstr, "Unable to open/write audio file to '%s'\n", wavname);
		sf_close(wavefileID);
		return 0;
	}
	float* dbuffer = nullptr;
	int lengthAllocated = -1, length = -1;
	CSignals nextblock, nextblock2;
	nextblock <= *this;
	int nChan = next == NULL ? 1 : 2;
	while (nextblock.nSamples != 0 || nextblock.tmark != 0.)
	{
		float* buffer;
		float tp1, tp2;
		length = nextblock.getBufferLength(tp1, tp2, 300/*skope::play_block_ms*/);
		if (tp1 == 0. && tp2 == 0. || nChan > 1)
		{
			if (length > lengthAllocated)
			{
				if (dbuffer) delete[] dbuffer;
				dbuffer = new float[length * nChan];
				lengthAllocated = length;
			}
			buffer = dbuffer;
			if (!nextblock.makebuffer<float>(dbuffer, length, tp1, tp2, nextblock2)) // sig is empty
				return 0;
		}
		else
		{
			buffer = nextblock.buf;
			nextblock2 <= nextblock;
			nextblock.nextCSignals(tp1, tp2, nextblock2);
		}
		sf_writef_float(wavefileID, buffer, length);
		nextblock = nextblock2;
	}
	if (dbuffer) delete[] dbuffer;
	sf_close(wavefileID);
	return 1;
}
