#include "functions_common.h"
#include "portaudio.h"
#include <thread>

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
	vector<string> desc_arg_opt = { "devID", "repeat", };
	vector<CVar> default_arg = { CVar(0.), CVar(1.), };
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
	vector<string> desc_arg_opt = { "devID", "repeat", };
	vector<CVar> default_arg = { CVar(0.), CVar(1.), };
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


Cfunction set_builtin_function_stop_pause(fGate fp)
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

#define FRAMES_PER_BUFFER 4096


class playmod {
public:
	const CVar* pvar;
	double timeblock; // in sec; determined in the constructor and constant
	double currenttime; // in sec; constantly changing
	int currentID; // constantly changing
	int repeatCount;
	CVar* phandle;
	PaStream* stream;
	// In the constructor, CVar object and the framebuffer size are specified by the user
	// timeblock is derived.
	playmod(const CVar& obj, int _lenblock, int _repCount) {
		pvar = &obj;
		lenblock = _lenblock;
		timeblock = (double)lenblock / pvar->GetFs();
		currenttime = 0;
		currentID = 0;
		repeatCount = _repCount;
	};
	virtual ~playmod() {};
	int get_lenblock() { return lenblock; };
private:
	int lenblock; // not changing after the constructor
};

#include <iostream>

static int playCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	playmod* data = (playmod*)userData;
	CVar* phandle = data->phandle;
	CVar* pobj = (CVar*)data->pvar;
	float* out = (float*)outputBuffer;
	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;
	(void)inputBuffer;
	vector<auxtype> out1, out2;
	auto nomore = pobj->bufDataAt(data->currenttime, framesPerBuffer, out1, out2);
	//for now just look at out1
	for (unsigned long k = 0; k < min(out1.size(), framesPerBuffer); k++) {
		*out++ = (float)out1[k];  /* left */
		*out++ = (float)out2[k];  /* right */
	}
	if (nomore) {
		data->repeatCount--;
		std::cout << "repeat down to " << data->repeatCount << endl;
		if (data->repeatCount > 0) {
			data->currentID = 0;
			data->currenttime = (double)data->currentID / pobj->GetFs();
			std::cout << "new call bufDataAt with 0, " << framesPerBuffer - out1.size() << endl;
			nomore = pobj->bufDataAt(0, framesPerBuffer - out1.size(), out1, out2);
			auto endpt = out1.size();
			for (unsigned long k = 0; k < endpt; k++) {
				*out++ = (float)out1[k];  /* left */
				*out++ = (float)out2[k];  /* right */
			}
			data->currentID += endpt;
			data->currenttime = (double)data->currentID / pobj->GetFs();
			phandle->strut["dur_prog"].SetValue(data->currenttime);
			return paContinue;
		}
		else
		{
			std::cout << "Returning paComplete" << endl;
			return paComplete;
		}
	}
	else {
		data->currentID += out1.size();
		data->currenttime = (double)data->currentID / pobj->GetFs();
		phandle->strut["dur_prog"].SetValue(data->currenttime);
	}
	return paContinue;
}

map<CVar*, PaStream*> streams;

void playfinishcb(void *p)
{
	playmod* data = (playmod*)p;
	CVar* phandle = data->phandle;
	phandle->strut["active"] = CVar((double)false);
}

void playthread(const CVar &obj, const CVar& handle, const PaStreamParameters& outputParameters)
{
	PaStream* stream;
	PaError err;
	auto rep = handle.strut.find("repeat");
	playmod pm(obj, FRAMES_PER_BUFFER, (int)(*rep).second.value());
	err = Pa_OpenStream(&stream, NULL /*no input*/, &outputParameters, obj.GetFs(), FRAMES_PER_BUFFER, paClipOff, playCallback, &pm);
	pm.phandle = (CVar*)&handle;
	pm.stream = stream;
	streams[pm.phandle] = stream;
	Pa_SetStreamFinishedCallback(stream, playfinishcb);
	err = Pa_StartStream(stream);
	// sleep time: segment duration plus a margin of ~ 500 ms
	Pa_Sleep(obj.alldur() + 500* (int)(*rep).second.value());
	err = Pa_StopStream(stream);
	err = Pa_CloseStream(stream);
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

static CVar make_audio_handle(double fs, double dur, double repeat, const PaStreamParameters& outputParameters)
{
	CVar out;
	out.strut["fs"] = CVar(fs);
	out.strut["repeat"] = CVar(repeat);
	out.strut["channels"] = CVar((double)outputParameters.channelCount);
	out.strut["format"] = CVar((double)outputParameters.channelCount);
	out.strut["dev"] = CVar((double)outputParameters.device);
	out.strut["dur"] = CVar(dur);
	out.strut["dur_prog"] = CVar(0.);
	out.SetValue((double)(uintptr_t)&out);
	return out;
}

void _play(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	cleanup_streams();
	string fname = pnode->str;
	if (ISSCALAR(past->Sig.type())) {
		auto stream = streams[&past->Sig];
		const PaStreamInfo* info = Pa_GetStreamInfo(stream);
		if (!info)
			past->Sig.SetValue(0);
		else {
			PaError err = Pa_StartStream(stream);
			past->Sig.SetValue(1.);
		}
	}
	else {
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

		auto devid = args[0].value();
		auto repeat = args[1].value();
		auto handle = make_audio_handle(past->Sig.GetFs(), past->Sig.alldur(), repeat, outputParameters);
		thread t1(playthread, past->Sig, handle, outputParameters);
		t1.detach();
		past->Sig = handle;
	}
}

void _stop_pause(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	cleanup_streams();
	string fname = pnode->str;
	auto stream = streams[&past->Sig];
	const PaStreamInfo* info = Pa_GetStreamInfo(stream);
	if (!info)
		past->Sig.SetValue(0);
	else {
		PaError err = Pa_StopStream(stream);
		if (fname=="stop")
			err = Pa_CloseStream(stream);
		if (err != paNoError) {
			printf("error stop()\n");
		}
		past->Sig.SetValue(1.);
	}
}
