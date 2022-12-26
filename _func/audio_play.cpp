#include "functions_common.h"
#include "portaudio.h"

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
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
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

#define FRAMES_PER_BUFFER 2048


class playstr {
public:
	const CVar* pvar;
	int offset;
	playstr(const CVar& obj = NULL) { pvar = &obj;  offset = 0; };
	virtual ~playstr() {};
};

static int patestCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	playstr* data = (playstr*)userData;
	CVar* pobj = (CVar*)data->pvar;
	float* out = (float*)outputBuffer;
	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;
	(void)inputBuffer;
	for (unsigned long i = 0; i < framesPerBuffer; i++) {
		if (data->offset + i > pobj->nSamples) {
			*out++ = 0;
			*out++ = 0;
		}
		else {
			*out++ = (float)pobj->buf[data->offset + i];  /* left */
			*out++ = (float)pobj->buf[data->offset + i];  /* right */
		}
	}
	data->offset += framesPerBuffer;
	if (data->offset >= pobj->nSamples)
		return paComplete;
	return paContinue;
}

static void StreamFinished(void* userData)
{
	printf("Stream Completed.\n");
}

void _play(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	PaStreamParameters outputParameters;
	PaStream * stream;
	PaError err;

	err = Pa_Initialize();
	if (err != paNoError) {
		printf("error play()\n");
		return;
	}
	outputParameters.device = Pa_GetDefaultOutputDevice();
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		return;
	}
	outputParameters.channelCount = 2;       /* stereo output */
	outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	playstr ps(past->Sig);
	err = Pa_OpenStream(&stream, NULL /*no input*/, &outputParameters, past->GetFs(), FRAMES_PER_BUFFER, paClipOff, patestCallback, &ps);
	err = Pa_SetStreamFinishedCallback(stream, &StreamFinished);
	err = Pa_StartStream(stream);
	Pa_Sleep(past->Sig.alldur()*1.2);
//	err = Pa_StopStream(stream);
	err = Pa_CloseStream(stream);
	Pa_Terminate();

}