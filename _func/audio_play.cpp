#include "functions_common.h"
#include "portaudio.h"
#include <thread>
#include <mutex>
#include <iostream>

/* An audio handle has the following members:
*	fs: sampling rate
*	chan: the number of channels
*	format: audio format (char, int16, float32, double, etc)
*	dur: duration in sec
*	dur_prog: duration played/recorded in sec
*	repeat: repeat count (default 1)
*	dev: device id's for the audio event, can be an array
*	block: the length of the recording/playing buffer per channel
*	buffer; a CSignals object (valid while active--a short life cycle
*	active: bool (true, or false).
*/

map<PaStream*, bool> playdone;

Cfunction set_builtin_function_play(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj", };
	vector<string> desc_arg_opt = { "repeat", "devID", };
	vector<CVar> default_arg = { CVar(1.), CVar(0.), };
	set<uint16_t> allowedTypes1 = { 1, ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, 2 };
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

Cfunction set_builtin_function_play2(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_handle", "audio_object", };
	vector<string> desc_arg_opt = { "repeat", };
	vector<CVar> default_arg = { CVar(1.), CVar(0.), };
	set<uint16_t> allowedTypes1 = { TYPEBIT_NULL }; // not used
	ft.allowed_arg_types.push_back(allowedTypes1); // not used (but need this line)
	set<pfunc_typecheck> allowedCheckFunc1 = { Cfunction::IsSTRUT };
	ft.qualify.push_back(allowedCheckFunc1);
	set<pfunc_typecheck> allowedCheckFunc2 = { Cfunction::IsAUDIOG };
	ft.qualify.push_back(allowedCheckFunc2);
	set<pfunc_typecheck> allowedCheckFunc3 = { Cfunction::IsScalar };
	ft.qualify.push_back(allowedCheckFunc3);
	set<pfunc_typecheck> prohibitFunc1 = { Cfunction::IsNotScalarG, };
	ft.reject.push_back(prohibitFunc1);
	set<pfunc_typecheck> prohibitFunc2 = { };
	ft.reject.push_back(prohibitFunc2);
	ft.reject.push_back(prohibitFunc2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_stop_pause_resume(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_handle", };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = {};
	set<uint16_t> allowedTypes1 = { 0xFFFF, }; // accepting all
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

#define FRAMES_PER_BUFFER 256 // With fs=44100 Hz, tone(500,1000).ramp(100) sounds OK with this value. If this was 4096, it sounds choppy at the end


class playmod {
public:
	const CVar* pvar;
	const CVar* pvarNext;
	double timeblock; // in sec; determined in the constructor and constant
	double currenttime; // in sec; constantly changing
	int currentID; // constantly changing
	int repeatCount;
	auxtype* pdur_prog;
	auxtype* pactive;
	PaStream* stream;
	// In the constructor, CVar object and the framebuffer size are specified by the user
	// timeblock is derived.
	playmod(const CVar& obj, int _lenblock, int _repCount, auxtype* _pdur_prog, auxtype* _pactive) {
		pvar = &obj;
		lenblock = _lenblock;
		timeblock = (double)lenblock / pvar->GetFs();
		currenttime = 0;
		currentID = 0;
		repeatCount = _repCount;
		pdur_prog = _pdur_prog;
		pactive = _pactive;
		pvarNext = NULL;
	};
	virtual ~playmod() {};
	int get_lenblock() { return lenblock; };
private:
	int lenblock; // not changing after the constructor
};

static int playCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	playmod* data = (playmod*)userData;
	CVar* pobj = (CVar*)data->pvar;
	auxtype* pdur_prog = data->pdur_prog;
	auxtype* pactive = data->pactive;
	float* out = (float*)outputBuffer;
	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;
	(void)inputBuffer;
	vector<auxtype> out1, out2;
	auto nomore = pobj->bufDataAt(data->currenttime, framesPerBuffer, out1, out2);
	//for now just look at out1
	for (unsigned long k = 0; k < min(out1.size(), framesPerBuffer); k++) {
		*out++ = (float)out1[k];  /* left */
		if (out2.size()>0)
			*out++ = (float)out2[k];  /* right */
	}
	if (nomore) {
		data->repeatCount--;
		if (data->repeatCount > 0) {
			data->currentID = 0;
			data->currenttime = (double)data->currentID / pobj->GetFs();
			nomore = pobj->bufDataAt(0, framesPerBuffer - out1.size(), out1, out2);
			for (unsigned long k = 0; k < out1.size(); k++) {
				*out++ = (float)out1[k];  /* left */
				if (out2.size()>0)
					*out++ = (float)out2[k];  /* right */
			}
			data->currentID += out1.size();
			data->currenttime = (double)data->currentID / pobj->GetFs();
			*pdur_prog = data->currenttime;
			return paContinue;
		}
		else
		{
			*pactive = (auxtype)0.;
			playdone[data->stream] = true;
			return paComplete;
		}
	}
	else {
		data->currentID += out1.size();
		data->currenttime = (double)data->currentID / pobj->GetFs();
		*pdur_prog = data->currenttime;
		*pactive = (auxtype)1.;
	}
	return paContinue;
}

map<uintptr_t, PaStream*> streams;

void playfinishcb(void *p)
{
	playmod* data = (playmod*)p;
//	*data->pactive = 0; //settings "active" false
}

/* 1/3/2023
*  h=x.play plays x; while x is played
* h.play(y) should put y on deck and when playing x is completed, y should be played
* That means, h.play(y) should invoke Pa_OpenStream AFTER the stream for x is stopped and closed
* therefore, lock1 holds on mutex m for playing x and m is released when x playing is done and lock2 takes over the mutex m
* To do---
* 1. Create mutex object as many as playing handle is generated
* (so that h=x.play, h.play(y) are subject to one mutex, but at the same time h2=x.play, h2.play(y) etc can generate another playing handle and play them independently.
* 2. h.play(z) or h.play(a) etc while x or y is played... can subsequent calls to h.play(new_sig) properly put new signals on deck and played them properly?
* 
*  a=wave("") b=wave("")
* h=a.play; j=0, for k=1:100000, j+=k.sqrt; end, h.play(b)
*/

void playthread2(const CVar& obj, uintptr_t handle, int rep, auxtype* pdur_prog, auxtype* pactive, const PaStreamParameters& outputParameters)
{
	PaStream* stream;
	PaError err;
	playmod pm(obj, FRAMES_PER_BUFFER, rep, pdur_prog, pactive);

	/* Do not open a new stream until the previous has completed playing*/
	//Hold here
//	unique_lock<mutex> lock2(mtx);
	err = Pa_OpenStream(&stream, NULL /*no input*/, &outputParameters, obj.GetFs(), FRAMES_PER_BUFFER, paClipOff, playCallback, &pm);
	pm.stream = stream;
	streams[handle] = stream;
	Pa_SetStreamFinishedCallback(stream, playfinishcb);
	err = Pa_StartStream(stream);
	playdone[stream] = false;
	while (!playdone[stream]) {
		Pa_Sleep(100);
	};
	err = Pa_StopStream(stream);
	err = Pa_CloseStream(stream);
}

void playthread(const CVar &obj, uintptr_t handle, int rep, auxtype* pdur_prog, auxtype* pactive, void* petc, const PaStreamParameters& outputParameters)
{
	PaStream* stream;
	PaError err;
	playmod pm(obj, FRAMES_PER_BUFFER, rep, pdur_prog, pactive);
	mutex p;
//	unique_lock<mutex> lock1(mtx);
	err = Pa_OpenStream(&stream, NULL /*no input*/, &outputParameters, obj.GetFs(), FRAMES_PER_BUFFER, paClipOff, playCallback, &pm);
	if (err == paNoError) {
		streams[handle] = stream;
		playdone[stream] = false;
		pm.stream = stream;
		Pa_SetStreamFinishedCallback(stream, playfinishcb);
		err = Pa_StartStream(stream);
		while (!playdone[stream]) {
			Pa_Sleep(100);
		};
		err = Pa_StopStream(stream);
		err = Pa_CloseStream(stream);
	}
	else {
		strcpy((char*)petc, "error");
	}
}

static void cleanup_streams()
{
	for (auto it = streams.begin(); it != streams.end(); it++)
	{
		const PaStreamInfo* info = Pa_GetStreamInfo((*it).second);
		if (!info)
		{
			streams.erase((*it).first);
			return;
		}
	}
}

static unique_ptr<CVar> make_audio_handle(double fs, double dur, double repeat, const PaStreamParameters& outputParameters)
{
	CVar *tp = new CVar(0.);
	tp->strut["type"] = CVar(string("audio"));
	tp->strut["fs"] = CVar(fs);
	tp->strut["repeat"] = CVar(repeat);
	tp->strut["channels"] = CVar((double)outputParameters.channelCount);
	tp->strut["format"] = CVar((double)outputParameters.channelCount);
	tp->strut["dev"] = CVar((double)outputParameters.device);
	tp->strut["dur"] = CVar(dur);
	tp->strut["dur_prog"] = CVar(0.);
	tp->strut["active"] = CVar(0.);
	*tp->buf = (double)(uintptr_t)tp->buf;
	return unique_ptr<CVar>(tp);
}

void _play2(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	cleanup_streams();
	PaStream* stream = NULL;
	if (streams.find((uintptr_t)past->Sig.value())!=streams.end())
		stream = streams[(uintptr_t)past->Sig.value()];
	else {
		past->pgo = nullptr;
		past->Sig.Reset();
		past->Sig.SetValue(0);
		return;
	}
	const PaStreamInfo* info = Pa_GetStreamInfo(stream);
	auto tp = past->Sig.type();
	if (!info) {
		past->Sig.SetValue(0);
		past->pgo = nullptr;
	}
	else {
		if (past->pgo != nullptr && ISSCALARG(tp) && ISSTRUT(tp) && past->pgo.get()->value() == past->Sig.value())
		{
			PaStreamParameters outputParameters;
			outputParameters.device = Pa_GetDefaultOutputDevice();
			if (outputParameters.device == paNoDevice) {
				fprintf(stderr, "Error: No default output device.\n");
				return;
			}
			outputParameters.channelCount = 2;       /* stereo output */
			outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
			outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
			outputParameters.hostApiSpecificStreamInfo = NULL;
			auto repeat = args[1].value();
			unique_ptr<CVar> pres = make_unique<CVar>(args[0]);
			auto phandle = move(past->pgo);
			*phandle->strut["dur"].buf = args[0].alldur();
			auxtype* pdur_prog = phandle->strut["dur_prog"].buf;
			auxtype* pactive = phandle->strut["active"].buf;
			thread playingthread(playthread2, *pres, (uintptr_t)phandle->value(), (int)repeat, pdur_prog, pactive, outputParameters);
			playingthread.detach();
			past->Sig = *(past->pgo = move(phandle));
		}
		else {
			past->Sig.SetValue(0);
			past->pgo = nullptr;
		}
	}
}

void _play(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	cleanup_streams();
	string fname = pnode->str;
	PaStreamParameters outputParameters;
	outputParameters.device = Pa_GetDefaultOutputDevice();
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		return;
	}
	outputParameters.channelCount = past->Sig.next ? 2 : 1;       /* stereo or mono */
	outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	auto repeat = args[0].value();
	auto devid = args[1].value();
	auto phandle = make_audio_handle(past->Sig.GetFs(), past->Sig.alldur(), repeat, outputParameters);
	auxtype* pdur_prog = phandle->strut["dur_prog"].buf;
	auxtype* pactive = phandle->strut["active"].buf;
	char etc[256] = {};
	thread t1(playthread, past->Sig, (uintptr_t)phandle->value(), (int)repeat, pdur_prog, pactive, etc, outputParameters);
	t1.detach();
	past->Sig = *(past->pgo = move(phandle));
	cout << "playing " << (uintptr_t)past->Sig.value() << endl;

}

void _stop_pause_resume(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	cleanup_streams();
	string fname = pnode->str;
	cout << "stopping " << (uintptr_t)past->Sig.value() << endl;
	auto stream = streams[(uintptr_t)past->Sig.value()];
	cout << "stopping stream " << stream << endl;
	past->Sig.Reset();
	const PaStreamInfo* info = Pa_GetStreamInfo(stream);
	if (!info) {
		past->Sig.SetValue(0);
	}
	else {
		if (fname == "resume") {
			PaError err = Pa_StartStream(stream);
			if (err == paNoError) {
				playdone[stream] = false;
				past->Sig.SetValue(1.);
			}
			else {
				printf("Can't resume; PortAudio error: Pa_StartStream()\n");
				past->Sig.SetValue(0.);
			}
		}
		else {
			auto tp = past->Sig.type();
			PaError err = Pa_StopStream(stream);
			if (err == paNoError) {
				if (fname == "stop") {
					PaError err = Pa_CloseStream(stream);
					if (err == paNoError) {
						playdone[stream] = true;
						// If not the same, find the pgo that corresponds to past->Sig
						if (past->pgo != nullptr && ISSCALARG(tp) && ISSTRUT(tp) && past->pgo.get()->value() == past->Sig.value())
						{
							auto p = past->pgo.get();
							*(p->strut["active"].buf) = 0;
							past->Sig.SetValue(0.);
						}
					}
					else {
						printf("Can't Stop/Pause; PortAudio error: Pa_CloseStream()\n");
						past->Sig.SetValue(0.);
					}
				}
				else {
					cout << "Paused" << endl;
				}
			}
			else {
				printf("Can't Stop/Pause; PortAudio error: Pa_StopStream()\n");
				past->Sig.SetValue(0.);
			}
		}
	}
}
