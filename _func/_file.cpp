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

map<float, FILE *> file_ids;

#include <algorithm>
#include <cassert>
//#include "bjcommon.h"
//#include "audstr.h"
//#include "lame_bj.h"
//#include "samplerate.h"
#include "sndfile.h"

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
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING + 2, };
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
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING + 2, };
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

static void EnumAudioVariables(skope* past, vector<string>& var)
{
	//var.clear();
	//for (map<string, CVar>::iterator it = past->Vars.begin(); it != past->Vars.end(); it++)
	//	if (it->second.GetType() == CSIG_AUDIO) var.push_back(it->first);
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
	if (!past->Sig.Wavwrite(fullfilename.c_str(), errStr, option))
		throw exception_etc(*past, pnode, errStr).raise();
}

static void resample_if_fs_different(skope* past, const AstNode* p)
{ // call this function only if past->Sig still has the active signal
	vector<string> audiovars;
	EnumAudioVariables(past, audiovars);
	if (audiovars.size() > 0)
	{
		if (past->Sig.GetFs() != past->GetFs())
		{
			//int oldFs = past->GetFs();
			//CVar ratio(1);
			//ratio.SetValue(past->Sig.GetFs() / (double)oldFs);
			//past->Sig.fp_mod(&CSignal::resample, &ratio);
			//if (ratio.IsString()) // this means there was an error during resample
			//	throw CAstException(FUNC_SYNTAX, *past, p).proc(ratio.string().c_str());
			//sformat(past->statusMsg, "(NOTE)File fs=%d Hz. The audio data resampled to %d Hz.", past->Sig.GetFs(), oldFs);
			//past->Sig.SetFs(oldFs);
			//if (past->Sig.next)
			//	past->Sig.next->SetFs(oldFs);
		}
	}
	else
	{
		past->pEnv->Fs = past->Sig.GetFs();
		past->statusMsg = "(NOTE)Sample Rate of AUXLAB Environment is now set to ";
		char temp[16];
		sprintf_s(temp, "%d Hz.", past->pEnv->Fs);
		past->statusMsg += temp;
	}
}

void _wave(skope *past, const AstNode *pnode, const vector<CVar>& args)
{
	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
	/*! \brief wave(filename)
	*         Open .wav file
	*
	*  Input: filename: string
	*  Output: audio_signal
	*/
	string estr;
//	string filename = past->makefullfile(past->Sig.str(), ".wav");
	string filename = past->Sig.str();
	float beginMs = args[0].value();
	float durMs = args[1].value();
	SF_INFO sfinfo; 
	SNDFILE* wavefileID = read_wav_info(filename, sfinfo, estr);
	if (!wavefileID)
		throw exception_etc(past, pnode, estr).raise();
	
	sf_count_t count;
	if (wavefileID)
		exception_etc(*past, p, estr).raise();
	past->Sig.Reset(sfinfo.samplerate);
//	past->Sig.SetNextChan(NULL); // Cleans up existing next // CHECK!!!!! 12/16/2021
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
		past->Sig.UpdateBuffer((int)sfinfo.frames);
		for (unsigned int k = 0; k < sfinfo.frames; k++)			past->Sig.buf[k] = buf3[k][0];
		buf3 = (float(*)[2]) & buffer[m++];
		past->Sig.next->UpdateBuffer((int)sfinfo.frames);
		for (unsigned int k = 0; k < sfinfo.frames; k++)			past->Sig.next->buf[k] = buf3[k][0];
		delete[] buffer;
	}
	sf_close(wavefileID);
//	resample_if_fs_different(past, p);
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
	while (!nextblock.IsEmpty())
	{
		float* buffer;
		float tp1, tp2;
		length = nextblock.getBufferLength(tp1, tp2, 300/*CAstSig::play_block_ms*/);
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
