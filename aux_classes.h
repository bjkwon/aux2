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


#pragma once

using namespace std;

#include <string>
#include <complex>
#include <vector>
#include <map>
#include <functional>

typedef void (*fmodify) (float*, uint64_t, void*, void*);

// fs : 1 for non-temporal 
//      0 for relative timemarks tseq 
//      2 for string
//      3 for Handle array
//      4 for byte array
//      or actual sampling rate

// temporal means fs is meaningfully big, representing a sampling rate.

// nSamples is 1 ?  if (type() & 0x0001)
// nSamples > 1 ? if (type() & 0x0010)
// temporal ? if (type() & 0b1100) <== type() & TYPEBIT_TEMPO_CHAINS_SNAP
// audio ? if ( (type() & 0xFF00 == TYPEBIT_TEMPO_ONE || type() & 0xFF00 == TYPEBIT_TEMPO_ONE) && type() & 0x00FF > 1)
// tseq, no snap ? if ( (type() & 0xFF00 == TYPEBIT_TEMPO_ONE || type() & 0xFF00 == TYPEBIT_TEMPO_ONE) && type() & 0x00FF == 1)
// tseq, snap ? if ( type() & 0xFF00 == TYPEBIT_TEMPO_CHAINS_SNAP && type() & 0x00FF > 1)

#define TYPEBIT_NULL		(uint16_t)0x0000 // 
// Don't do any logical operations between the following three, & | !
#define TYPEBIT_TEMPO_ONE			(uint16_t)0x0004 // 0b0100 ; temporal no chain, OR single-chain audio
#define TYPEBIT_TEMPO_CHAINS		(uint16_t)0x0008 // 0b1000 ; temporal, chain, tseq (nSamples must be always one), OR audio 
#define TYPEBIT_TEMPO_CHAINS_SNAP	(uint16_t)0x000C // 0b1100 ; temporal, chain, snap (e.g., FFT)
// 
// The difference between audio and TYPEBIT_TEMPO_CHAINS_SNAP:
// one object of audio is represented in grid of 1/fs
// chains of TYPEBIT_TEMPO_CHAINS_SNAP are stacked on the same tmark.
#define TYPEBIT_REAL		(uint16_t)0x0000 // 4 bytes -- sizeof(float)
#define TYPEBIT_SIZE1		(uint16_t)0x0010 // use this for boolean  0b001
#define TYPEBIT_STRING 		(uint16_t)0x0030 // 0x0020 + 0x0010  0b011
#define TYPEBIT_BYTE 		(uint16_t)0x0050 // 0x0040 + 0x0010  0b101
#define TYPEBIT_COMPLEX		(uint16_t)0x0060 // 0b110 8 bytes -- sizeof(float) times 2
 // last three binary digits of HIWORD: number of channels minus one
#define TYPEBIT_MULTICHANS	(uint16_t)0x0100
#define TYPEBIT_CELL		(uint16_t)0x1000
#define TYPEBIT_STRUT		(uint16_t)0x2000
#define TYPEBIT_STRUTS		(uint16_t)0x4000

#define TYPEBIT_LOGICAL		TYPEBIT_SIZE1	
// type & TYPEBIT_TEMPORAL ==> type | TYPEBIT_TEMPO_CHAINS_SNAP 

#define ISNULL(X)           ((X) == TYPEBIT_NULL)
#define ISNULLG(X)          (!((X) & 3))
#define ISSCALAR(X)         ( ((X) & 0b1111) == 1) // An actual number, including a complex number; 0b01111111 is  compliment of TYPEBIT_COMPLEX 0b10000000 
#define ISSCALARG(X)        (((((X) & 0b1111) & 0xF003)) == 1) // struct or cell with a surface CSignals object
#define ISVECTOR(X)         (((X) & 0b1111) == 2) // An actual number array
#define ISVECTORG(X)        (((((X) & 0b1111) & 0xF003)) == 2) // struct or cell with a surface CSignals object
#define IS2D(X)             (((X) & 0xFF0F) == 3)
#define IS2DG(X)            (((X) & 0x000F) == 3)
#define ISSIZE1(X)          (((X) & 0xFFF0) & TYPEBIT_SIZE1) // An actual logical array, regardless of length 
#define ISSIZE1G(X)         (((X) & 0x00F0) & TYPEBIT_SIZE1) // struct or cell with a surface CSignals object
#define ISSTRING(X)         (((X) & 0xFFF0) == TYPEBIT_STRING) // An actual number array, regardless of length 
#define ISSTRINGG(X)        (((X) & 0x00F0) == TYPEBIT_STRING) // struct or cell with a surface CSignals object
#define ISBYTE(X)           (((X) & 0xFFF0) == TYPEBIT_BYTE) // An actual byte array, regardless of length 
#define ISBYTEG(X)          (((X) & 0x00F0) == TYPEBIT_BYTE) // struct or cell with a surface CSignals object
#define ISBOOL(X)           (((X) & 0xFFF0) == TYPEBIT_SIZE1) // An actual logical array, regardless of length 
#define ISBOOLG(X)          (((X) & 0x00F0) == TYPEBIT_SIZE1) // struct or cell with a surface CSignals object
#define ISCOMPLEX(X)        (((X) & 0b01100000) == TYPEBIT_COMPLEX) // An actual complex array, regardless of length 
#define ISCOMPLEXG(X)       (((X) & (0xFF00+0b01100000)) == TYPEBIT_COMPLEX) // struct or cell with a surface CSignals object
#define ISAUDIO(X)          (((X) & 0xF0FF) == TYPEBIT_TEMPO_ONE + 2 || ((X) & 0xF0FF) == TYPEBIT_TEMPO_CHAINS + 2 || ((X) & 0xF0FF) == TYPEBIT_TEMPO_ONE + 3 || ((X) & 0xF0FF) == TYPEBIT_TEMPO_CHAINS + 3) // An actual audio array
#define ISAUDIOG(X)         (((X) & 0x00FF) == TYPEBIT_TEMPO_ONE + 2 || ((X) & 0x00FF) == TYPEBIT_TEMPO_CHAINS + 2 || ((X) & 0x00FF) == TYPEBIT_TEMPO_ONE + 3 || ((X) & 0x00FF) == TYPEBIT_TEMPO_CHAINS + 3) // An actual audio array
// tseq means chained scalars, including an unchanged timed value (customarily a value timed at zero is not considered a tseq, but it is not prohibited)
#define ISTSEQ(X)           (((X) & 0xF0FF) == TYPEBIT_TEMPO_ONE + 1 || ((X) & 0xF0FF) == TYPEBIT_TEMPO_CHAINS + 1) // An actual audio array
#define ISTSEQG(X)          (((X) & 0x00FF) == TYPEBIT_TEMPO_ONE + 1 || ((X) & 0x00FF) == TYPEBIT_TEMPO_CHAINS + 1) // An actual audio array
// tshot means chained vectors or string including an unchanged one (customarily a vector/string timed at zero is not considered a tseq, but it is not prohibited)
#define ISTSHOT(X)          (((X) & 0xF0FF) == TYPEBIT_TEMPO_ONE + 2 || ((X) & 0xF0FF) == TYPEBIT_TEMPO_CHAINS + 2) // An actual audio array
#define ISTSHOTG(X)         (((X) & 0x00FF) == TYPEBIT_TEMPO_ONE + 2 || ((X) & 0x00FF) == TYPEBIT_TEMPO_CHAINS + 2)
// Stereo applies to any temporal object, regardless of audio, tseq or tshot. If you need a stereo audio, then ISSTEREO and ISAUDIO
#define ISSTEREO(X)         ((X) & (TYPEBIT_MULTICHANS + 0xF000))
#define ISSTEREOG(X)        ((X) & TYPEBIT_MULTICHANS)

// Temporal applies to any temporal object, regardless of audio, tseq or tshot.
#define ISTEMPORAL(X)       (((X) & 0xFF0C) == TYPEBIT_TEMPO_ONE || ((X) & 0xFF0C) == TYPEBIT_TEMPO_CHAINS || ((X) & 0xFF0C) == TYPEBIT_TEMPO_CHAINS_SNAP)
#define ISTEMPORALG(X)      (((X) & 0x000C) == TYPEBIT_TEMPO_ONE || ((X) & 0x000C) == TYPEBIT_TEMPO_CHAINS || ((X) & 0x000C) == TYPEBIT_TEMPO_CHAINS_SNAP)


#define ALL_AUDIO_TYPES AUDIO_TYPES_1D, AUDIO_TYPES_2D

#define AUDIO_TYPES_1D \
TYPEBIT_TEMPO_ONE + 2,  TYPEBIT_TEMPO_CHAINS + 2, \
TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_ONE + 2, TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS + 2

#define AUDIO_TYPES_2D \
TYPEBIT_TEMPO_ONE + 3,  TYPEBIT_TEMPO_CHAINS + 3, TYPEBIT_TEMPO_CHAINS_SNAP + 3, \
TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_ONE + 3, TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS + 3, TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS_SNAP + 3

#define FFT_RESULTS_TYPES FFT_RESULTS_TYPES_NONAUDIO, FFT_RESULTS_TYPES_ONECHANNEL, FFT_RESULTS_TYPES_MULTICHANNELS

#define FFT_RESULTS_TYPES_NONAUDIO 2, 3

#define FFT_RESULTS_TYPES_ONECHANNEL \
TYPEBIT_COMPLEX + 2, TYPEBIT_COMPLEX + 3, \
TYPEBIT_TEMPO_ONE + 2, TYPEBIT_TEMPO_CHAINS_SNAP + 2, TYPEBIT_TEMPO_ONE + 3, TYPEBIT_TEMPO_CHAINS_SNAP + 3, \
TYPEBIT_COMPLEX + TYPEBIT_TEMPO_ONE + 2, TYPEBIT_COMPLEX + TYPEBIT_TEMPO_CHAINS_SNAP + 2, \
TYPEBIT_COMPLEX + TYPEBIT_TEMPO_ONE + 3, TYPEBIT_COMPLEX + TYPEBIT_TEMPO_CHAINS_SNAP + 3 \

#define FFT_RESULTS_TYPES_MULTICHANNELS \
TYPEBIT_MULTICHANS + TYPEBIT_COMPLEX + 2, TYPEBIT_MULTICHANS + TYPEBIT_COMPLEX + 3, \
TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_ONE + 2, TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS_SNAP + 2, TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_ONE + 3, TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS_SNAP + 3, \
TYPEBIT_MULTICHANS + TYPEBIT_COMPLEX + TYPEBIT_TEMPO_ONE + 2, TYPEBIT_MULTICHANS + TYPEBIT_COMPLEX + TYPEBIT_TEMPO_CHAINS_SNAP + 2, \
TYPEBIT_MULTICHANS + TYPEBIT_COMPLEX + TYPEBIT_TEMPO_ONE + 3, TYPEBIT_MULTICHANS + TYPEBIT_COMPLEX + TYPEBIT_TEMPO_CHAINS_SNAP + 3 \

#define NONAUDIO_TEMPORAL_COMPLEX_TYPES \
TYPEBIT_COMPLEX + TYPEBIT_TEMPO_ONE + 1, TYPEBIT_COMPLEX + TYPEBIT_TEMPO_CHAINS + 1, \
TYPEBIT_COMPLEX + TYPEBIT_TEMPO_ONE + 2, TYPEBIT_COMPLEX + TYPEBIT_TEMPO_CHAINS + 2, \
TYPEBIT_COMPLEX + TYPEBIT_TEMPO_ONE + 3, TYPEBIT_COMPLEX + TYPEBIT_TEMPO_CHAINS + 3, \
TYPEBIT_COMPLEX + TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_ONE + 1, TYPEBIT_COMPLEX + TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS + 1, \
TYPEBIT_COMPLEX + TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_ONE + 2, TYPEBIT_COMPLEX + TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS + 2, \
TYPEBIT_COMPLEX + TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_ONE + 3, TYPEBIT_COMPLEX + TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS + 3

#define FARGS (unsigned int id0=0, unsigned int len=0, void *p = NULL)

#define DECL_GATE(FUNCTION_NAME) \
void FUNCTION_NAME(skope* past, const AstNode* pnode, const vector<CVar>& args);\
Cfunction set_builtin_function##FUNCTION_NAME(fGate fp);

// bufType Null Byte String Logic Real Complex  
#ifndef AUX_CLASSES
#define AUX_CLASSES
class body
{
public:
	uint64_t nSamples;
	uint64_t nGroups;
	union
	{
		char *strbuf;
		complex<float> *cbuf;
		float *buf;
		bool *logbuf;
	};
	char bufType;
	unsigned char bufBlockSize;
	bool ghost;

	// Constructors
	body();
	body(const body& src);
	body(float value);
	body(complex<float> value);
	body(float *y, int len);
	body(bool *y, int len);
	body(const vector<float> & src);
	virtual ~body();

	body& operator=(const body& rhs);
	body& operator=(const vector<float> & rhs);
	body& operator<=(const body& rhs);

	body& operator+=(float con);
	body& operator*=(float con);
	body& operator/=(float con);
	bool operator==(const body & rhs);

	body& UpdateBuffer(uint64_t length, uint64_t offset = 0);
	void Reset();

	float value() const;
	string valuestr(int digits = 14) const;
	complex<float> cvalue() const;
	void SetValue(float v);
	void SetValue(complex<float> v);
	void SetComplex();
	void SetByte();
	void SetReal();
	bool IsComplex() const {return bufBlockSize == 2 * sizeof(float);	};
	bool IsBool() const { return bufBlockSize == 1; };

	void SwapContents1node(body &sec);
	vector<float> ToVector() const;	
	
	body &addmult(char op, body &arg, unsigned int id0 = 0, uint64_t len = 0);
	body& each(float (*fn)(float));
	body& each_sym(float (*fn)(float));
	body& each_sym2(float (*fn)(float, float), const body& arg);
	body &each(float (*fn)(complex<float>));
	body &each(complex<float>(*fn)(float));
	body &each(complex<float> (*fn)(complex<float>));
	body &each(float (*fn)(float, float), const body &arg2);
	body &each(complex<float> (*fn)(complex<float>, complex<float>), const body &arg2);

	float* begin() { return nSamples>0 ? &buf[0] : nullptr; }
	float* end() { return nSamples>0 ? &buf[nSamples] : nullptr; }

	body &transpose();
	body& MakeLogical();
	body& LogOp(body &rhs, int op);
	body &insert(body &sec, int id);

	body& replacebyindex(vector<unsigned int>::iterator idBegin, vector<unsigned int>::iterator idEnd, const body& RHS);
	body& replacebyindex(unsigned int id0, unsigned int len, const body& RHS);
	float _max FARGS const;
	float _min FARGS const;
	float sum FARGS const;
	float mean FARGS const;
	float stdev FARGS const;
	bool operator < (const body &rhs) const;

protected:
		body& operator<=(body * rhs);
};

/* Use evoke_modsig when no size resizing is done, e.g., filter, ramp
* Use evoke_modsig2 when size is resized, e.g., resample
*/
class CSignal : public body
{
public:
	int fs;
	float tmark;
	short snap; // 0 for regular; 1 for time seq or an object where data stack up on the same tmark (vertically), like FFT
	uint64_t Len() const { if (fs == 2) return (nSamples-1) / nGroups; else  return nSamples / nGroups; }
	bool operator < (const CSignal &rhs) const;

	CSignal evoke_getsig2(CSignal(*fp) (float*, unsigned int, void*, void*), void* popt1 = NULL, void* popt = NULL);
	CSignal& evoke_modsig(fmodify, void* popt1 = NULL, void* popt2 = NULL);
	CSignal evoke_modsig2(CSignal(*fp) (const CSignal&, void*, void*), void* popt1 = NULL, void* popt2 = NULL);

	CSignal &matrixmult(CSignal *arg);
	CSignal & Hilbert FARGS;

	CSignal & movespec FARGS;

	CSignal & Reset(int  fs2set=0);
	void ReverseTime();

	CSignal & reciprocal(void);	// Multiplicative inverse
	CSignal & operator=(const body& rhs);
	CSignal & operator=(const CSignal & rhs);
	CSignal & operator<=(const CSignal & rhs);
	bool operator== (const CSignal &rhs);
	bool operator == (float rhs);
	bool operator == (string rhstr);
	CSignal & operator+=(CSignal *yy); // Concatenation
	CSignal & operator>>=(float delta);
	CSignal & operator-(void);	// Unary minus

	// Retrieve signal characteristics (single channel ONLY)
	int GetType() const;
	int GetFs() const {return fs;};
	void SetFs(int  newfs);
	float* GetBuffer() {return buf;}
	float length FARGS const;
	float dur FARGS const;
	float durc FARGS const;
	float begint FARGS const;
	float endt FARGS const;
	float RMS FARGS const;

	CSignal &_atmost FARGS;
	CSignal &_atleast FARGS;

	CSignal & resample FARGS;
	CSignal& _filter(const vector<float>& num, const vector<float>& den, vector<float>& initialfinal, unsigned int id0 = 0, unsigned int len = 0);

	inline bool IsEmpty() const { return fs == 1 && nSamples == 0 && tmark == 0.; }
	/*Don't use these Is___() functions. Backward compatible only*/
	inline bool IsScalar() const { return nSamples == 1; }
	inline bool IsVector() const { return nSamples > 1; }
	inline bool IsAudio() const {
		if (fs < 500) return false; 
		uint16_t tp = type();
		return (( (tp & 0xFF00) == TYPEBIT_TEMPO_ONE || (tp & 0xFF00) == TYPEBIT_TEMPO_CHAINS) && (tp & 0x00FF) > 1);
	}
	bool IsString() const { return bufType == 'S'; }

	// Constructors
	CSignal();
	CSignal(int sampleRate); // construct with a specified sample rate.
	CSignal(int sampleRate, unsigned int len); // construct with a specified sample rate and buffer size
	CSignal(float value); // construct a scala with the specified value
	CSignal(const CSignal & src); // copy constructor
	CSignal(float *y, int  len);
	CSignal(vector<float> vv);
	CSignal(string str); // make a string CSignal

	virtual ~CSignal();

	string str() const;
	char *getString(char *str, const int size);
	CSignal &SetString(const char *str);
	CSignal &SetString(const char c);

	bool IsSingle() const;
	bool IsLogical() const { return bufBlockSize == 1 && fs != 2; } // this doesn't differentiate "logical" audio

	uint16_t type() const
	{
		uint16_t out = TYPEBIT_NULL;
		if (IsEmpty())			return out;
		else if (fs == 1)		out = 0;
		if (nSamples > 0)
		{
			out++;
			if (nSamples > 1)
			{
				out++;
				if (nGroups > 1)
					out++;
			}
		}
		// Data type based on data alignment
		if (bufType == 'S') out += TYPEBIT_STRING;
		else if (bufType == 'B') out += TYPEBIT_BYTE;
		else if (bufBlockSize == 1) out += TYPEBIT_SIZE1;
		else if (bufBlockSize == sizeof(float)) out += TYPEBIT_REAL;
		else if (bufBlockSize == sizeof(float) *2) out += TYPEBIT_COMPLEX;
		if (tmark > 0 || (fs == 0 || fs > 500) && nSamples > 2) out += TYPEBIT_TEMPO_ONE;
		return out;
	};

protected:
	float _dur() { return (float)nSamples * 1000.f / fs; }// for backward compatibility 5/18. No reason to get rid of it. 10/18/2019
	CSignal & operator<=(CSignal * prhs);
	CSignal & operator%(const CSignal & v); // scale operator (absolute)
	CSignal & operator%(float v); // scale operator (absolute)
	CSignal & operator|(float v); // scale operator (relative)
	CSignal & operator|(const CSignal & RMS2adjust);
	CSignal & operator*(pair<vector<float>, vector<float>> coef);
	pair<unsigned int, uint64_t> grid() const {	return make_pair((unsigned int)round(tmark*fs/1000.), nSamples-1+ (uint64_t)round(tmark*fs / 1000.));	};
	bool overlap(const CSignal &sec);
	function<float(float)> op;
	function<float(float)> op1(float me) { return [me](float you) {return me + you; }; };
	function<float(float)> op2(float me) { return [me](float you) {return me - you; }; };
	function<float(float)> op3(float me) { return [me](float you) {return me * you; }; };
	function<float(float)> op4(float me) { return [me](float you) {return me / you; }; };
	bool operate(const CSignal & sec, char op);

private:
	int operator_prep(const CSignal & sec, uint64_t &idx4op1, uint64_t &idx4op2, uint64_t &offset);

};

class CTimeSeries : public CSignal
{
public:
	CTimeSeries *chain;
	int WriteAXL(FILE* fp);

	bool operator < (const CTimeSeries & rhs) const;

	CTimeSeries evoke_getval(float (CSignal::*)(unsigned int, unsigned int, void *) const, void *popt = NULL);
	CTimeSeries & evoke_modsig(fmodify, void* popt1 = NULL, void* popt2 = NULL);
	CTimeSeries evoke_modsig2(CSignal(*fp) (const CSignal&, void*, void*), void* popt1 = NULL, void* popt2 = NULL);

	CTimeSeries evoke_getsig(CTimeSeries(*fgetCSignals) (const CTimeSeries&, void*), void* popt = NULL);
	CTimeSeries evoke_getsig2(CSignal(*fp) (float*, unsigned int, void*, void*), void *popt1 = NULL, void *popt = NULL);

	CTimeSeries & Reset(int fs2set = 0);
	CTimeSeries & AddChain(const CTimeSeries &sec);
	CTimeSeries * GetDeepestChain();
	uint64_t  CountChains(uint64_t *maxlength=NULL) const;
	void AddMultChain(char type, CTimeSeries *forthis);
	CTimeSeries & MergeChains();
	CTimeSeries & ConnectChains();
	CTimeSeries & reciprocal(void);	// Multiplicative inverse
	CTimeSeries & timeshift(float tp_ms);
	CTimeSeries & removeafter(float timems);
	CTimeSeries & Squeeze();
	CTimeSeries & Crop(float begin_ms, float end_ms);
	CTimeSeries & ReplaceBetweenTPs(CTimeSeries &newsig, float t1, float t2);
	CTimeSeries & NullIn(float tpoint);

	CTimeSeries & operator=(const CSignal & rhs);
	CTimeSeries & operator=(const CTimeSeries & rhs);
	CTimeSeries & operator<=(const CTimeSeries & rhs);
	CTimeSeries & operator+=(float con);
	CTimeSeries & operator*=(float con);
	CTimeSeries & operator/=(float con);
	CTimeSeries & operator-(void);	// Unary minus
	CTimeSeries & operator*=(CTimeSeries &scaleArray);
	CTimeSeries & operator+=(CTimeSeries *yy); // Concatenation
	CTimeSeries & operator/=(CTimeSeries &scaleArray);
	CTimeSeries & operator>>=(float delta);
	CTimeSeries & operator%(float v);
	CTimeSeries & operator|(float v);
	CTimeSeries & operator|(CTimeSeries * RMS2adjust);
	bool operator==(const CTimeSeries & rhs);

	void setsnap(int set=1) {	for (CTimeSeries*p = this; p; p = p->chain)		p->snap = set;	};
	bool IsAudioOnAt(float timept);
	bool chained_scalar() const;
	void SwapContents1node(CTimeSeries &sec);
	CTimeSeries & LogOp(CTimeSeries &rhs, int type);
	void ReverseTime();

	CTimeSeries & each(float(*fn)(float));
	CTimeSeries& each_allownegative(float(*fn)(float));
	CTimeSeries & each(float(*fn)(complex<float>));
	CTimeSeries & each(float(*fn)(float, float), const CSignal &arg2);
	CTimeSeries & each(complex<float>(*fn)(complex<float>));
	CTimeSeries & each(complex<float>(*fn)(complex<float>, complex<float>), const CSignal&arg2);
	CTimeSeries & transpose1();

	float MakeChainless();
 	CTimeSeries * AtTimePoint(float timept);

	// Constructors
	CTimeSeries();
	CTimeSeries(int sampleRate); // construct with a specified sample rate.
	CTimeSeries(int sampleRate, unsigned int len); // construct with a specified sample rate and buffer size
	CTimeSeries(float value); // construct a scalar with the specified value ---is it necessary? 5/19/2018
	CTimeSeries(const CSignal & src);
	CTimeSeries(const CTimeSeries & src);
	virtual ~CTimeSeries();

	float alldur() const;
	bool IsEmpty() const { return chain == nullptr && CSignal::IsEmpty(); }

	bool operate(const CTimeSeries & sec, char op);
protected:
	CTimeSeries & operator<=(CTimeSeries * prhs);
	CTimeSeries & operator%(CTimeSeries * v);
	CTimeSeries & operator+(CTimeSeries * sec);
	CTimeSeries & operator-(CTimeSeries * sec);
	CSignal TSeries2CSignal();

private:
	void sort_by_tmark();
};

#define MAX_24BIT		(double)0x007fffff
static inline int _float_to_24bit(float x)
{
	// This maps a double variable raning -1 to 1, to a short variable ranging -16388608 to 16388607.
	return (int)(max(min(x, 1.f), -1.f) * MAX_24BIT - .5);
}

class CSignals : public CTimeSeries
{
public:
	CSignals *next;

	bool operator < (const CSignals & rhs) const;

	CSignals evoke_getval(float(*)(float), void* popt = NULL);
	CSignals evoke_getval(float (CSignal::*)(unsigned int, unsigned int, void* p) const, void *popt = NULL) ;
	CSignals& evoke_modsig(fmodify, void* popt1 = NULL, void* popt2 = NULL);
	CSignals evoke_modsig2(CSignal(*fp) (const CSignal&, void*, void*), void* popt1 = NULL, void* popt2 = NULL);

	CSignals evoke_getsig(CTimeSeries(*fgetCSignals) (const CTimeSeries&, void*), void* popt = NULL);
	CSignals evoke_getsig2(CSignal(*fp) (float*, unsigned int, void*, void*), void* popt1 = NULL, void* popt = NULL);

	// Constructors
	CSignals();
	CSignals(int sampleRate);
	CSignals(float value);
	CSignals(float *y, int len);
	CSignals(const CTimeSeries & src);
	CSignals(const CSignals & src);
	CSignals(std::string str); // make a string CSignals
	CSignals(bool *b, unsigned int len);
	virtual ~CSignals();

	bool operator==(const CSignals & rhs);
	bool operator==(float rhs);
	bool operator==(std::string rhstr);
	CSignals & operator=(const CTimeSeries & rhs);
	CSignals & operator=(const CSignals & rhs);
	CSignals & operator+=(float con);
	const CSignals & operator+=(CSignals *yy);
	CSignals& operator/=(float con);
	CSignals & operator*=(float con);
	CSignals & operator%(const CSignals &targetRMS);
	CSignals & operator%(float v);
	CSignals & operator|(float v);
	CSignals & operator|(const CSignals & RMS2adjust);

	int IsStereo() const { return 0 + (next!=NULL); }
	void SetFs(int newfs);

	inline bool IsEmpty() const { return next == nullptr && CTimeSeries::IsEmpty(); }

	float MakeChainless();
//	void SetValue(float v);

	int ReadAXL(FILE* fp, bool logical, char *errstr);
	int WriteAXL(FILE* fp);

	bool stereo_scalar() const;

	CSignals& RMS(); //to calculate the overall RMS; different from CSignal::RMS()

	void SetNextChan(const CSignals& second, bool need2makeghost = false);
	CTimeSeries *DetachNextChan() {CTimeSeries *p=next;next=NULL;return p;}
	CSignals & Reset(int fs2set=0);
	CSignals & reciprocal(void);
	CSignals & operator-(void);
	CSignals & operator<=(const CSignals & rhs); // ghost assignment operator2
	CSignals & operator<=(CSignals * prhs); // ghost assignment operator2
	CSignals & operator>>=(float delta);
	CSignals & Crop(float begin_ms, float end_ms);

	CSignals & ReplaceBetweenTPs(CSignals &newsig, float t1, float t2);
	CSignals & LogOp(CSignals &rhs, int type);

	void setsnap(int set=1) {	CTimeSeries::setsnap(set); if (next) next->CTimeSeries::setsnap(set);	};

	CSignals & NullIn(float tpoint);
	float alldur() const;

	CSignals & each(float (*fn)(float));
	CSignals& each_allownegative(float(*fn)(float));
	CSignals & each(float (*fn)(complex<float>));
	CSignals & each(complex<float> (*fn)(complex<float>));
	CSignals & transpose1();
	int getBufferLength(float & lasttp, float & lasttp_with_silence, float blockDur) const;
	void nextCSignals(float lasttp, float lasttp_with_silence, CSignals &ghcopy);
	template<typename T>
	int makebuffer(T * outbuffer, int length, float lasttp, float lasttp_with_silence, CSignals &ghcopy)
	{
		int nChan = next == nullptr ? 1 : 2;
		memset(outbuffer, 0, sizeof(T) * length * nChan);
		ghcopy <= *this;
		CSignals * p = &ghcopy;
		if (lasttp == 0. && lasttp_with_silence == 0.)
		{ // BEGINNING WITH A NULL PORTION
			// leave outbuffer alone, but update lasttp and lasttp_with_silence
			lasttp = lasttp_with_silence = tmark;
			if (nChan == 2)
			{
				if (next->tmark < lasttp)
					lasttp = lasttp_with_silence = next->tmark;
			}
		}
		else
		{
			for (int ch = 0; p && ch < 2; ch++, p = (CSignals*)p->next)
			{
				for (CTimeSeries *q = p; q; q = q->chain)
				{
					if (q->tmark > lasttp) break;
					int offset = (int)(q->tmark / 1000. * fs + .5) * nChan;
					if (is_same<T, short>::value)
						for (unsigned int m = 0; m < q->nSamples; m++)
							outbuffer[offset + m * nChan + ch] = (short)(_float_to_24bit(q->buf[m]) >> 8);
					else if (is_same<T, float>::value)
						for (unsigned int m = 0; m < q->nSamples; m++)
							outbuffer[offset + m * nChan + ch] = (T)q->buf[m];
				}
			}
		}
		nextCSignals(lasttp, lasttp_with_silence, ghcopy);
		return length;
	}
	int Wavwrite(const char *wavname, char *errstr, std::string wavformat = "");
	uint16_t type() const
	{
		uint16_t out = CSignal::type();
		//why is this ugly? 
		//because TYPEBIT_TEMPO_ONE was added in CSignal::type()
		//    why? because CSignal::type() needs to return temporal type for a simple(single) temporal object
		// another reason for the ugly minus/plus is, the type-bit scheme: 
		//    TYPEBIT_TEMPO_ONE TYPEBIT_TEMPO_CHAINS TYPEBIT_TEMPO_CHAINS_SNAP are not mutually exclusive.
		// Don't try to simplify this unless you know what to do. 12/29/2021
		if (chain)
		{
			if (out & TYPEBIT_TEMPO_ONE)
				out -= TYPEBIT_TEMPO_ONE;
			if (snap)
				out += TYPEBIT_TEMPO_CHAINS_SNAP;
			else
				out += TYPEBIT_TEMPO_CHAINS;
		}
		else if (fs == 0 || fs > 500)
		{
			if (snap) {
				out -= TYPEBIT_TEMPO_ONE;
				out += TYPEBIT_TEMPO_CHAINS_SNAP;
			}
			else if (chain) {
				out -= TYPEBIT_TEMPO_ONE;
				out += TYPEBIT_TEMPO_CHAINS;
			}
		}
		if (next)
			out += TYPEBIT_MULTICHANS;
		return out;
	}

protected:
	bool operate(const CSignals & sec, char op);
};

class CVar : public CSignals
{
public:
	vector<CVar> cell;
	map<std::string, CVar> strut;
	map<std::string, vector<CVar *>> struts;

	bool functionEvalRes;

	bool operator < (const CVar & rhs) const;
	bool IsStruct() const { return (!strut.empty() || !struts.empty()); }
	bool IsEmpty() const { return (!chain && !next && nSamples == 0 && cell.empty() && strut.empty() && struts.empty()); }
	CVar& Reset(int fs2set = 0);
	bool IsGO() const;

	CSignals evoke_getsig2(CSignal(*fp) (float*, unsigned int, void*, void*), void* popt1 = NULL, void* popt = NULL);

	CVar & bringnext();

	CVar & operator=(const CSignals & rhs);
	CVar & operator=(const CVar& rhs);
	CVar & operator=(CVar * rhs);

	bool operator==(const CVar& rhs);
	bool operator==(string rhstr);
	bool operator==(float val);

	CVar & initcell(CVar &sec);
	CVar & appendcell(CVar &sec);
	CVar & setcell(unsigned int id, CVar &sec);
	CVar & length();

	// Constructors
	CVar(const CSignals & src);
	CVar(const CVar& src);
	CVar(CVar * src);
	CVar();
	virtual ~CVar();

	CVar & operator+(const CVar & sec);
	CVar & operator-(const CVar & sec);
	CVar & operator*(const CVar & sec);
	CVar & operator/(const CVar & sec);
	CVar & operator-();
	CVar & operator+=(CVar * psec);
	CVar & operator+=(const CVar & sec);
	CVar & operator-=(const CVar & sec);
	CVar & operator*=(const CVar & sec);
	CVar & operator/=(const CVar & sec);

	uint16_t type() const
	{
		uint16_t out = CSignals::type();
		if (!cell.empty())		out += TYPEBIT_CELL;
		else if (!strut.empty())
		{
			out += TYPEBIT_STRUT;
			if (!struts.empty()) out += TYPEBIT_STRUTS;
		}
		else if (fs == 3)		out += TYPEBIT_STRUT + TYPEBIT_STRUTS;
		return out;
	}
};



#endif // AUX_CLASSES