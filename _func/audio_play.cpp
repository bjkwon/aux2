#include "functions_common.h"
#include "portaudio.h"
#include <thread>

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

#define FRAMES_PER_BUFFER 256


class playmod {
public:
	const CVar* pvar;
	double timeblock; // in sec; determined in the constructor and constant
	double currenttime; // in sec; constantly changing
	int currentID; // constantly changing
	int handle;
	PaStream* stream;
	// In the constructor, CVar object and the framebuffer size are specified by the user
	// timeblock is derived.
	playmod(const CVar& obj, int _lenblock) {
		pvar = &obj;
		lenblock = _lenblock;
		timeblock = (double)lenblock / pvar->GetFs();
		currenttime = 0;
		currentID = 0;
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
	float* out = (float*)outputBuffer;
	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;
	(void)inputBuffer;
	vector<auxtype> out1, out2;
	auto nomore = pobj->bufDataAt(data->currenttime, framesPerBuffer, out1, out2);
	//for now just look at out1
	for (unsigned long k = 0; k < framesPerBuffer; k++) {
		*out++ = (float)out1[k];  /* left */
		*out++ = (float)out1[k];  /* right */
	}
	if (nomore)
		return paComplete;
	else {
		data->currentID += out1.size();
		data->currenttime = (double)data->currentID / pobj->GetFs();
	}
	return paContinue;
}

map<int, PaStream*> streams;

void playfinishcb(void *p)
{
	playmod* data = (playmod*)p;
	//12/27/2022 
	//At this point, this callback is pretty useless for the implementation of pause() 
	// as long as pause() uses Pa_StopStream and h.play calls Pa_StartStream again
	// because Pa_StopStream() invokes this callback.
//	streams.erase(data->handle);
}

void playthread(const CVar &obj, int id)
{
	PaStreamParameters outputParameters;
	PaStream* stream;
	PaError err;

	outputParameters.device = Pa_GetDefaultOutputDevice();
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		return;
	}
	outputParameters.channelCount = 2;       /* stereo output */
	outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	playmod pm(obj, FRAMES_PER_BUFFER);
	err = Pa_OpenStream(&stream, NULL /*no input*/, &outputParameters, obj.GetFs(), FRAMES_PER_BUFFER, paClipOff, playCallback, &pm);
	pm.handle = id;
	pm.stream = stream;
	streams[id] = stream;
	Pa_SetStreamFinishedCallback(stream, playfinishcb);
	err = Pa_StartStream(stream);
	Pa_Sleep(obj.alldur() + 500);
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

void _play(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	cleanup_streams();
	string fname = pnode->str;
	if (ISSCALAR(past->Sig.type())) {
		auto handle = (int)past->Sig.value();
		auto stream = streams[handle];
		const PaStreamInfo* info = Pa_GetStreamInfo(stream);
		if (!info)
			past->Sig.SetValue(0);
		else {
			PaError err = Pa_StartStream(stream);
			past->Sig.SetValue(1.);
		}
	}
	else {
		int handle = rand();
		thread t1(playthread, past->Sig, handle);
		t1.detach();
		past->Sig.SetValue((auxtype)handle);
	}
}

void _stop_pause(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	cleanup_streams();
	string fname = pnode->str;
	auto handle = (int)past->Sig.value();
	auto stream = streams[handle];
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
