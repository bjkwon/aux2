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


// GetType() == CSIG_TSERIES was used many times in the old code
// This is a poor code that doesn't represent something solid,
// but it worked (mostly) well, probably not a bullet-proof way, but
// other code was written around to keep big problems from emerging.
// Long-term, this should be completely re-done, but for now,
// it is renamed to chained_scalar or stereo_scalar show what it is.
// 11/14/2021

#pragma once

using namespace std;

#include <string>
#include <complex>
#include <vector>
#include <map>
#include <functional>

typedef void (*fmodify) (float*, uint64_t, uint64_t, void*);

// To check if nSamples is 1, type() & 0x0001

#define TYPEBIT_NULL		(uint16_t)0x0000
#define TYPEBIT_SNAP		(uint16_t)0x0004
#define TYPEBIT_TEMPORAL	(uint16_t)0x0008
// The difference between TYPEBIT_AUDIO and TYPEBIT_TSEQ:
// data in TYPEBIT_AUDIO aligned with time points in grid of 1/fs,
// data in TYPEBIT_TSEQ are stacked on the same tmark.
#define TYPEBIT_AUDIO		(TYPEBIT_TEMPORAL + TYPEBIT_NULL)
#define TYPEBIT_TSEQ		(TYPEBIT_TEMPORAL + TYPEBIT_SNAP)
#define TYPEBIT_SIZE8		(uint16_t)0x0000 // 4 bytes -- sizeof(float)
#define TYPEBIT_SIZE1		(uint16_t)0x0010
#define TYPEBIT_FS2 		(uint16_t)0x0020
#define TYPEBIT_SIZE16		(uint16_t)0x0040 // 8 bytes -- 4 bytes times 2
#define TYPEBIT_LOGICAL		(TYPEBIT_SIZE1 + TYPEBIT_NULL) // 0x0010
// DO NOT MASK THIS WITH type() to detect string; Use IsString() instead.
#define TYPEBIT_STRING		(TYPEBIT_SIZE1 + TYPEBIT_FS2)
#define TYPEBIT_COMPLEX		TYPEBIT_SIZE16
#define TYPEBIT_GO	    	(uint16_t)0x0800
#define TYPEBIT_CELL		(uint16_t)0x1000
#define TYPEBIT_STRUT		(uint16_t)0x2000
#define TYPEBIT_STRUTS		(uint16_t)0x4000


// IsString()
// ()

#define FARGS (unsigned int id0=0, unsigned int len=0, void *p = NULL)

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
	void SetReal();
	bool IsComplex() const {return bufBlockSize == 2 * sizeof(float);	};
	bool IsBool() const { return bufBlockSize == 1; };

	void SwapContents1node(body &sec);
	vector<float> ToVector() const;	
	
	body &addmult(char type, body &arg, unsigned int id0 = 0, uint64_t len = 0);
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

	body & mathFunForbidNegative(float(*fn)(float), complex<float>(*cfn)(complex<float>));
	body & mathFunForbidNegative(float(*fn)(float, float), complex<float>(*cfn)(complex<float>, complex<float>), const body& param);

	body &transpose();
	body& MakeLogical();
	body& LogOp(body &rhs, int type);
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

class CSignal : public body
{
public:
	int fs;
	float tmark;
	short snap; // 0 for regular; 1 for time seq or an object where data stack up on the same tmark (vertically), like FFT
	uint64_t Len() const { if (fs == 2) return (nSamples-1) / nGroups; else  return nSamples / nGroups; }
	bool operator < (const CSignal &rhs) const;


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

	inline bool IsEmpty() const { return nSamples == 0 && tmark == 0.; }
	inline bool IsScalar() const { return nSamples == 1; }
	inline bool IsVector() const { return nSamples > 1; }
	inline bool IsAudio() const {
		if (fs < 500) return false; 
		uint16_t tp = type();
		return (tp & (TYPEBIT_TEMPORAL + 0x0002)) && !(tp & TYPEBIT_SNAP); // nSamples must be > 2
	}
	bool IsString() const { return bufBlockSize == 1 && fs == 2; }

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
		else if (fs == 0 || fs > 500)		out = TYPEBIT_TEMPORAL;
		if (snap) out += TYPEBIT_SNAP;
		if (nSamples > 0) out++;
		if (nSamples > 1) out++;
		// Data type based on data alignment
		if (IsString())		out += TYPEBIT_STRING;
		else if (IsBool()) out += TYPEBIT_SIZE1;
		else if (bufBlockSize == sizeof(float)) out += TYPEBIT_SIZE8;
		else if (bufBlockSize == sizeof(float) *2) out += TYPEBIT_SIZE16;
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
	CTimeSeries & evoke_modsig(fmodify, void *popt = NULL);
	CTimeSeries evoke_getsig(CSignal (*)(CSignal&, unsigned int, unsigned int, void* ), void *popt = NULL);

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
	CTimeSeries & each(float(*fn)(complex<float>));
	CTimeSeries & each(float(*fn)(float, float), const body &arg2);
	CTimeSeries & each(complex<float>(*fn)(complex<float>));
	CTimeSeries & each(complex<float>(*fn)(complex<float>, complex<float>), const body &arg2);
	CTimeSeries & transpose1();

	CTimeSeries & each(float(*fn)(float, float), complex<float>(*cfn)(complex<float>, complex<float>), const CTimeSeries &param);

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
	CSignals & evoke_modsig(fmodify, void *popt = NULL);
	CSignals evoke_getsig(CSignal(*)(CSignal&, unsigned int, unsigned int, void* p), void *popt = NULL);

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
	CSignals & operator*=(float con);
	CSignals & operator%(const CSignals &targetRMS);
	CSignals & operator%(float v);
	CSignals & operator|(float v);
	CSignals & operator|(const CSignals & RMS2adjust);

	int IsStereo() const { return 0 + (next!=NULL); }

	inline bool IsEmpty() const { return next == nullptr && CTimeSeries::IsEmpty(); }

	float MakeChainless();
	void SetValue(float v);

	int ReadAXL(FILE* fp, bool logical, char *errstr);
	int WriteAXL(FILE* fp);

	bool stereo_scalar() const;

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
	CSignals & each(float (*fn)(complex<float>));
	CSignals & each(float (*fn)(float, float), const body &arg2);
	CSignals & each(complex<float> (*fn)(complex<float>));
	CSignals & each(complex<float> (*fn)(complex<float>, complex<float>), const body &arg2);
	CSignals & transpose1();
	CSignals & each(float(*fn)(float, float), complex<float>(*cfn)(complex<float>, complex<float>), const CSignals& param);
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

	CVar & bringnext();

	CVar & operator=(const CSignals & rhs);
	CVar & operator=(const CVar& rhs);
	CVar & operator=(CVar * rhs);

	bool operator==(const CVar& rhs);
	bool operator==(std::string rhstr);
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
		uint16_t out = CSignal::type();
		if (out == TYPEBIT_NULL && next) return ((CVar*)next)->CSignal::type();
		if (!cell.empty())		out += TYPEBIT_CELL;
		else if (!strut.empty())
		{
			out += TYPEBIT_STRUT;
			if (!struts.empty()) out += TYPEBIT_STRUTS;
		}
		else if (fs == 3)		out += TYPEBIT_STRUT + TYPEBIT_STRUTS;
		// fs zero means tseq with relative time, treat it as TYPEBIT_AUDIO
		else if (out == 1 && fs == 0)	out += TYPEBIT_AUDIO;
		if (IsGO()) out += TYPEBIT_GO;
		return out;
	}
};



#endif // AUX_CLASSES