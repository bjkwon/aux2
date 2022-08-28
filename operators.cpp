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

#include <cstring> // for g++
#include "aux_classes.h"
#include <algorithm>

bool CSignal::overlap(const CSignal &sec)
{
	if (sec.grid().first > grid().second || sec.grid().second < grid().first)
		return false;
	return true;
}

int CSignal::operator_prep(const CSignal& sec, uint64_t &idx4op1, uint64_t &idx4op2, uint64_t &offset)
{//The case of scalar sec is handled separately. Here we only separately allow the case of *this scalar.
	if (nSamples > 1 && fs != sec.fs) throw "Both operands must have the same fs.";
	// if this and sec are do not overlap, return here
	if (sec.grid().first > grid().second || sec.grid().second < grid().first)
		return 0;
	auto i1 = grid().first;
	auto i2 = sec.grid().first;
	auto f1 = grid().second;
	auto f2 = sec.grid().second;
	uint64_t offset2copy = 0, count2add = 0;
	if (f1 < f2)
		count2add += f2 - f1;
	if (i2 < i1)
	{
		count2add += (offset2copy = i1 - i2);
		tmark = sec.tmark;
	}
	UpdateBuffer(nSamples + count2add, offset2copy);
	nGroups = sec.nGroups;
	if (i1 == 0 && i2 == 0 && f2 == 0)
	{
		float val = buf[0];
		for_each(buf + 1, buf + nSamples, [val](float &v) { v = val; });
	}
	if (i2 < i1)
	{
		memcpy(buf, sec.buf, (i1 - i2)*bufBlockSize);
		offset = (idx4op1 = i1 - i2) ;
	}
	else
	{
		offset = 0;
		idx4op1 = i2 - i1;
	}
	idx4op2 = min(f1, f2) - min(i1, i2) + 1;
	if (f1 < f2)
		memcpy(buf + idx4op2, sec.buf + f1 - i2 + 1, (f2 - f1)*bufBlockSize);
	return 1;
}

void CTimeSeries::sort_by_tmark()
{
	if (!chain) return;
	CTimeSeries temp;
	temp <= *this;
	vector<const CTimeSeries*> chains;
	chains.push_back((const CTimeSeries*)&temp);
	for (const CTimeSeries *p = chain; p; p = p->chain)
		chains.push_back(p);

	std::sort(chains.begin(), chains.end(), [](const CTimeSeries *p, const CTimeSeries *q) 
	{return p->tmark < q->tmark; });

	auto it = chains.begin();
	if (this != *it)
	{
		buf = (*it)->buf;
		nSamples = (*it)->nSamples;
		nGroups = (*it)->nGroups;
		tmark = (*it)->tmark;
	}
	for (CTimeSeries *p = this; p; p = p->chain)
	{
		++it;
		if (it == chains.end())
			p->chain = nullptr;
		else
		{
			if (*it!=&temp)
				p->chain = (CTimeSeries*)*it;
			else
			{
				p->chain->tmark = temp.tmark;
				p->chain->buf = temp.buf;
				p->chain->nSamples = temp.nSamples;
				p->chain->nGroups = temp.nGroups;
			}
		}
	}
}

bool CSignal::operate(const CSignal& sec, char op)
{
	// Exception handling is yet to be done 3/8/2019
	// If fs for one is 1 and for the other is >3 (such as 44100)
	// make fs for this the big number
	if (fs == 1 && sec.fs > 3) fs = sec.fs;
	if (sec.IsScalar())
	{
		if (sec.IsComplex())
		{
			complex<float > val = sec.cvalue();
			if (!IsComplex()) SetComplex();
			if (op == '+')
				for_each(cbuf, cbuf + nSamples, [val](complex<float >& v) { v += val; });
			else if (op == '-')
				for_each(cbuf, cbuf + nSamples, [val](complex<float >& v) { v -= val; });
			else if (op == '*')
				for_each(cbuf, cbuf + nSamples, [val](complex<float >& v) { v *= val; });
			else if (op == '/')
				for_each(cbuf, cbuf + nSamples, [val](complex<float >& v) { v /= val; });
		}
		else
		{
			float  val = sec.value();
			if (IsComplex())
			{
				if (op == '+')
					for_each(cbuf, cbuf + nSamples, [val](complex<float >& v) { v += val; });
				else if (op == '-')
					for_each(cbuf, cbuf + nSamples, [val](complex<float >& v) { v -= val; });
				else if (op == '*')
					for_each(cbuf, cbuf + nSamples, [val](complex<float >& v) { v *= val; });
				else if (op == '/')
					for_each(cbuf, cbuf + nSamples, [val](complex<float >& v) { v /= val; });
			}
			else
			{
				if (op == '+')
					for_each(buf, buf + nSamples, [val](float & v) { v += val; });
				else if (op == '-')
					for_each(buf, buf + nSamples, [val](float & v) { v -= val; });
				else if (op == '*')
					for_each(buf, buf + nSamples, [val](float & v) { v *= val; });
				else if (op == '/')
					for_each(buf, buf + nSamples, [val](float & v) { v /= val; });
			}
		}
		snap += sec.snap;
		if (snap) snap = 1;
	}
	else
	{
		if (IsScalar()) 
		{
			snap += sec.snap;
			if (snap) snap = 1;
		}
		else
		{
			if (snap & sec.snap)
				throw "Operation between two TSEQ not allowed.";
		}
		uint64_t offset, idBegin, idEnd;
		if (!operator_prep(sec, idBegin, idEnd, offset)) // if not overlapping, just skip
			return false;
		int k = 0;
		if (sec.IsComplex())
		{
			complex<float>* cbuf2 = sec.cbuf + offset;
			if (op == '+')
				for_each(cbuf + idBegin, cbuf + idEnd, [cbuf2, &k](complex<float>& v) { v += cbuf2[k++]; });
			else if (op == '-')
				for_each(cbuf + idBegin, cbuf + idEnd, [cbuf2, &k](complex<float>& v) { v -= cbuf2[k++]; });
			else if (op == '*')
				for_each(cbuf + idBegin, cbuf + idEnd, [cbuf2, &k](complex<float>& v) { v *= cbuf2[k++]; });
			else if (op == '/')
				for_each(cbuf + idBegin, cbuf + idEnd, [cbuf2, &k](complex<float>& v) { v /= cbuf2[k++]; });
		}
		else
		{
			float* buf2 = sec.buf + offset;
			if (IsComplex())
			{
				if (op == '+')
					for_each(cbuf + idBegin, cbuf + idEnd, [buf2, &k](complex<float>& v) { v += buf2[k++]; });
				else if (op == '-')
					for_each(cbuf + idBegin, cbuf + idEnd, [buf2, &k](complex<float>& v) { v -= buf2[k++]; });
				else if (op == '*')
					for_each(cbuf + idBegin, cbuf + idEnd, [buf2, &k](complex<float>& v) { v *= buf2[k++]; });
				else if (op == '/')
					for_each(cbuf + idBegin, cbuf + idEnd, [buf2, &k](complex<float>& v) { v /= buf2[k++]; });
			}
			else
			{
				if (op == '+')
					for_each(buf + idBegin, buf + idEnd, [buf2, &k](float& v) { v += buf2[k++]; });
				else if (op == '-')
					for_each(buf + idBegin, buf + idEnd, [buf2, &k](float& v) { v -= buf2[k++]; });
				else if (op == '*')
					for_each(buf + idBegin, buf + idEnd, [buf2, &k](float& v) { v *= buf2[k++]; });
				else if (op == '/')
					for_each(buf + idBegin, buf + idEnd, [buf2, &k](float& v) { v /= buf2[k++]; });
			}
		}
	}
	return true;
}

bool CTimeSeries::operate(const CTimeSeries& sec, char op)
{
	if (sec.nSamples == 1 && !sec.chain)
	{
		for (CTimeSeries *p = this; p; p = p->chain)
			p->CSignal::operate(sec, op);
		return true;
	}
	else if (nSamples == 1 && !chain)
	{
		CTimeSeries tp = *this;
		*this = sec;
		return operate(tp, op);
	}
	vector<const CTimeSeries*> sec_chains;
	for (const CTimeSeries *q = &sec; q; q = q->chain)
		sec_chains.push_back(q);
	for (auto &q : sec_chains)
		((CTimeSeries*)q)->chain = nullptr;
	for (auto it_chain = sec_chains.begin(); it_chain != sec_chains.end(); )
	{
		bool need2add = false;
		bool checker = true;
		for (CTimeSeries *p = this; p && it_chain != sec_chains.end(); p = p->chain)
		{
			if (p->CSignal::operate(**it_chain, op))
			{
				checker &= false;
				it_chain = sec_chains.erase(it_chain);
			}
			else
				checker &= true;
		}
		if (checker)
			it_chain++;
		checker = false;
	}
	for (auto q : sec_chains)
		AddChain(*q);
	// Unite overlapping chains
	for (CTimeSeries *q = this; q; /*q = q->chain; -->shouldn't be here because p can be removed from the chain*/)
	{
		bool autoupdate = true;
		for (CTimeSeries *p = q; p && p->chain; p = p->chain)
		{
			if (q->overlap(*(p->chain)))
			{
				q->CSignal::operate(*(p->chain), op);
				p->chain = p->chain->chain; //			remove p from the chain
				autoupdate = false;
			}
		}
		if (autoupdate)
			q = q->chain;
	}
	//at this point, no overlapping chains
	sort_by_tmark();
	return true;
}
bool CSignals::operate(const CSignals& sec, char op)
{
	CSignals copy;
	if (!next && sec.next)
		copy = *this;
	CTimeSeries::operate(sec, op); // here sec is used as CTimeSeries
	if (next)
	{
		if (sec.next)
			((CSignals*)next)->CTimeSeries::operate(*sec.next, op);
		else
			((CSignals*)next)->CTimeSeries::operate(sec, op);
	}
	else if (sec.next)
	{
		//if copy is constant or vector
		if (copy.type() <= 2)
			copy.SetFs(sec.next->GetFs());
		SetNextChan(copy);
		next->operate(*sec.next, op);
	}

	return true;
}

CVar & CVar::operator+(const CVar & sec)
{
	if (!cell.empty() || !strut.empty() || !struts.empty())
		throw "Invalid operation for a class or cell variable.";
	CSignals::operate(sec, '+');
	return *this;
}
CVar & CVar::operator+=(const CVar & sec)
{
	return operator+(sec);
}
CVar & CVar::operator-()
{
	if (!cell.empty() || !strut.empty() || !struts.empty())
		throw "Invalid operation for a class or cell variable.";
	CSignals::operator-();
	return *this;
}
CVar & CVar::operator-(const CVar & sec)
{
	if (!cell.empty() || !strut.empty() || !struts.empty())
		throw "Invalid operation for a class or cell variable.";
	CSignals::operate(sec, '-');
	return *this;
}
CVar & CVar::operator-=(const CVar & sec)
{
	return operator-(sec);
}
CVar & CVar::operator*(const CVar & sec)
{
	if (!cell.empty() || !strut.empty() || !struts.empty())
		throw "Invalid operation for a class or cell variable.";
	CSignals::operate(sec, '*');
	return *this;
}
CVar & CVar::operator*=(const CVar & sec)
{
	return operator*(sec);
}
CVar & CVar::operator/(const CVar & sec)
{
	if (!cell.empty() || !strut.empty() || !struts.empty())
		throw "Invalid operation for a class or cell variable.";
	CSignals::operate(sec, '/');
	return *this;
}
CVar & CVar::operator/=(const CVar & sec)
{
	return operator/(sec);
}

bool CVar::operator==(const CVar & rhs)
{
	bool singleItem = type() & 1;
	if (IsGO())
	{ // For GO's, check only the ID's
		if (nSamples != rhs.nSamples) return false;
		if (type() & TYPEBIT_STRING ^ rhs.type() & TYPEBIT_STRING) return false;
		if (type() & TYPEBIT_STRING) return str() == rhs.str();
		return value() == rhs.value();
	}
	if (!singleItem && (!cell.empty() || !strut.empty() || !struts.empty()))
		throw "For now no cell, strut or GO objects.";
	return CSignals::operator==(rhs);
}

bool CSignals::operator==(const CSignals & rhs)
{
	if (CTimeSeries::operator==((CTimeSeries)rhs))
	{
		if (next)	return CTimeSeries::operator==(*(rhs.next));
		return true;
	}
	else
		return false;
}
bool CTimeSeries::operator==(const CTimeSeries & rhs)
{
	CTimeSeries *p = this, *q = (CTimeSeries *)&rhs;
	bool res;
	for (; p && q ; p = p->chain, q = q->chain)
	{
		CSignal *pp = p, *qq = q;
		res = *pp == *qq;
		if (!res) return false;
	}
	// if this and rhs have different chain structure, return false
	if (p || q) return false;
	return true;
}
bool CSignal::operator==(const CSignal & rhs)
{
	if (fs != rhs.fs) return false;
	if (snap != rhs.snap) return false;
	if (tmark != rhs.tmark) return false;
	return body::operator==((body)rhs);

}
bool body::operator==(const body & rhs)
{
	if (nSamples != rhs.nSamples) return false;
	if (nGroups != rhs.nGroups) return false;
	if (bufBlockSize != rhs.bufBlockSize) return false;
	if (bufBlockSize == 1)
		for (unsigned int k = 0; k < nSamples; k++)
		{
			if (logbuf[k] != rhs.logbuf[k]) return false;
		}
	else if (bufBlockSize == sizeof(float))
		for (unsigned int k = 0; k < nSamples; k++)
		{
			if (buf[k] != rhs.buf[k]) return false;
		}
	else //if (bufBlockSize == 2*sizeof(float))
		for (unsigned int k = 0; k < nSamples; k++)
		{
			if (cbuf[k] != rhs.cbuf[k]) return false;
		}
	return true;
}

CTimeSeries& CTimeSeries::operator-(CTimeSeries* sec)
{
	throw "You shouldn't come here---";
	AddMultChain('+', &sec->operator-());
	return *this;
}

CVar & CVar::operator+=(CVar * psec)
{
	if (!cell.empty() || !strut.empty() || !struts.empty())
		throw "Invalid operation for a class or cell variable.";
	CSignals::operator+=(psec);
	return *this;
}

CSignal& CSignal::operator-(void) 	// Unary minus
{
	if (bufBlockSize == sizeof(float))
		for (unsigned int k = 0; k < nSamples; k++) buf[k] = -buf[k];
	if (bufBlockSize == 2 * sizeof(float))
		for (unsigned int k = 0; k < nSamples; k++) cbuf[k] = -cbuf[k];
	return *this;
}
CTimeSeries& CTimeSeries::operator-(void) 	// Unary minus
{
	for (CTimeSeries *p = this; p; p = p->chain)
		p->CSignal::operator-();
	return *this;
}
CSignals& CSignals::operator-(void)	// Unary minus
{
	CTimeSeries::operator-();
	if (next)	-*next;
	return *this;
}

CSignal & CSignal::operator*(pair<vector<float>, vector<float>> coef)
{ // modulate this with coef
	vector<float> tpoints = coef.first;
	vector<float> vals = coef.second;
	//assumption: tpoints is increasing 
	auto itval = vals.begin();
	for (auto it = tpoints.begin(); it != tpoints.end() - 1; it++)
	{
		if (*it > endt()) continue;
		float t1 = *it + tmark;
		float t2 = *(it + 1) + tmark;
		float slope = (*(itval + 1) - *itval) / (t2 - t1);
		float slopePerSample = (*(itval + 1) - *itval) / (t2 - t1) / fs * 1000.;
		int beginID = max(0, (int)round((t1 - tmark)*fs / 1000.));
		int endID = min((int)nSamples, (int)round((t2 - tmark)*fs / 1000.));
		for (int k = beginID; k < endID; k++)
			buf[k] *= *itval + slopePerSample * (k - beginID);
		itval++;
	}
	return *this;
}

CSignal & CSignal::operator%(float v)
{ // "at" operator; set the RMS at ____
	float rms = RMS();
	float factor = v - rms;
	*this *= pow(10, factor / 20.);
	return *this;
}
CTimeSeries & CTimeSeries::operator%(float v)
{
	CSignal::operator%(v);
	for (CTimeSeries *p = this; p; p = p->chain)
		p->CSignal::operator%(v);
	return *this;
}
CSignals & CSignals::operator%(float v)
{
	CTimeSeries::operator%(v);
	if (next)
		next->operator%(v);
	return *this;
}
//No exception handling when "PRESUMPTION" is noted.
CSignal & CSignal::operator%(const CSignal &targetRMS)
{ // PRESUMPTION for targetRMS: nSamples>0, bufBlockSize>=8--reject Empty, reject bool & string
	operator%(targetRMS.buf[0]);
	return *this;
}
CTimeSeries & CTimeSeries::operator%(CTimeSeries * targetRMS)
{ // PRESUMPTION for targetRMS: nSamples>0, bufBlockSize>=8--reject Empty, reject bool & string
	// for each chain, if chained, or a single chain satisfying that condition
	// if chained, it must be the same time-course; i.e., same tmarks
	// Or, scalar time sequence, where nSamples=1 for all chains
	bool timescalar = targetRMS->nSamples==1;
	for (CTimeSeries *p = targetRMS->chain; p; p = p->chain)
	{
		if (timescalar && p->nSamples > 1)
			throw "all operand chains must be scalar.";
		if (!timescalar && p->nSamples <= 1)
			throw "all operand chains must be vector or audio.";
	}
	if (timescalar)
	{ // time scalars 
		vector<float> _tpoints, vals;
		bool relTime = targetRMS->fs == 0;
		for (CTimeSeries *p = targetRMS; p; p = p->chain)
		{
			_tpoints.push_back(p->tmark);
			vals.push_back(p->value());
		}
		vector<float> tpoints(_tpoints);
		if (relTime)
		{
			for (auto &tp : tpoints)
				tp *= _dur();
		}
		CSignal::operator*(make_pair(tpoints, vals));
		for (CTimeSeries *p = chain, *q = targetRMS->chain; p; p = p->chain)
		{
			tpoints = _tpoints;
			if (relTime)
			{
				for (auto &tp : tpoints)
					tp *= p->_dur();
			}
			p->CSignal::operator*(make_pair(tpoints, vals));
		}
	}
	else
	{
		float v = targetRMS->buf[0];
		CSignal::operator%(v);
		for (CTimeSeries *p = chain, *q = targetRMS->chain; p; p = p->chain)
		{
			if (targetRMS->chain)
			{
				v = q->buf[0];
				q = q->chain;
			}
			p->CSignal::operator%(v);
		}
	}
	return *this;
}
CSignals & CSignals::operator%(const CSignals &targetRMS)
{ // PRESUMPTION for targetRMS: 
	// nSamples>0, bufBlockSize>=8--reject Empty, reject bool & string
	// for each chain, if chained, or a single chain satisfying that condition
	// if chained, it must be the same time-course; i.e., same tmarks
	// if targetRMS->next, it must have the same structure; if not, apply the same targetRMS for both channel (or one, if this->next is nullptr)
	//
	// OR
	// scalar time sequence, where nSamples=1 for all chains
	CTimeSeries::operator%((CTimeSeries*)&targetRMS);
	if (next)
	{
		if (targetRMS.next)
			((CSignals*)next)->CTimeSeries::operator%(targetRMS.next);
		else
			((CSignals*)next)->CTimeSeries::operator%((CTimeSeries *)&targetRMS);
	}
	return *this;
}

CSignal & CSignal::operator|(float v)
{ // Set the RMS at ____ (go up or down)
	*this *= pow(10, v / 20.);
	return *this;
}
CTimeSeries & CTimeSeries::operator|(float v)
{
	CSignal::operator|(v);
	for (CTimeSeries *p = this; p; p = p->chain)
		p->CSignal::operator|(v);
	return *this;
}
CSignals & CSignals::operator|(float v)
{
	CTimeSeries::operator|(v);
	if (next)
		next->operator|(v);
	return *this;
}

CSignal & CSignal::operator|(const CSignal & RMS2adjust)
{ // PRESUMPTION for RMS2adjust: nSamples>0, bufBlockSize>=8--reject Empty, reject bool & string
	operator|(RMS2adjust.buf[0]);
	return *this;
}
CTimeSeries & CTimeSeries::operator|(CTimeSeries * RMS2adjust)
{
	float v = RMS2adjust->buf[0];
	CSignal::operator|(v);
	for (CTimeSeries *p = chain, *q = RMS2adjust->chain; p; p = p->chain)
	{
		if (RMS2adjust->chain)
		{
			v = q->buf[0];
			q = q->chain;
		}
		p->CSignal::operator|(v);
	}
	return *this;
}
CSignals & CSignals::operator|(const CSignals & RMS2adjust)
{
	CTimeSeries::operator|((CTimeSeries*)&RMS2adjust);
	if (next)
	{
		if (RMS2adjust.next)
			((CSignals*)next)->CTimeSeries::operator|(RMS2adjust.next);
		else
			((CSignals*)next)->CTimeSeries::operator|((CTimeSeries *)&RMS2adjust);
	}
	return *this;
}
CTimeSeries& CTimeSeries::operator+(CTimeSeries* sec)
{
	AddMultChain('+', sec);
	return *this;
}

bool body::operator < (const body &rhs) const
{
	if (nSamples < rhs.nSamples) return true;
	if (nSamples > rhs.nSamples) return false;
	if (nGroups < rhs.nGroups) return true;
	if (nGroups > rhs.nGroups) return false;
	if (bufBlockSize < rhs.bufBlockSize) return true;
	if (bufBlockSize > rhs.bufBlockSize) return false;
	if (bufBlockSize== sizeof(float)) {
		for (unsigned int k = 0; k < nSamples; k++)
			if (buf[k] < rhs.buf[k]) return true;
	}
        else if (bufBlockSize == 2*sizeof(float)) {
		for (unsigned int k = 0; k < nSamples; k++)
			if (real(cbuf[k]) < real(rhs.cbuf[k]) && imag(cbuf[k]) < imag(rhs.cbuf[k])) return true;
	}
        else if (bufBlockSize == 1) {
		for (unsigned int k = 0; k < nSamples; k++)
			if (strbuf[k] < rhs.strbuf[k]) return true;
	}
        if (ghost && !rhs.ghost) return true;
	if (!ghost && rhs.ghost) return false;
	return false;
}

bool CSignal:: operator < (const CSignal &rhs) const
{
	if (fs < rhs.fs) return true;
	if (fs > rhs.fs) return false;
	if (tmark < rhs.tmark) return true;
	if (tmark > rhs.tmark) return false;
	if (snap < rhs.snap) return true;
	if (snap > rhs.snap) return false;
	body *p = (body *)this;
	body *q = (body*)&rhs;
	if (*p < *q) return true;
	return false;
}

bool CTimeSeries::operator < (const CTimeSeries & rhs) const
{
	for (const CTimeSeries *p = &rhs, *p0 = this; p; p = p->chain, p0 = p0->chain)
	{
		if (!p0) return true;
		if (*(CSignal*)p0 < *(CSignal*)p) return true;
		if (*(CSignal*)p < *(CSignal*)p0) return false;
	}
	//if (outarg.size() < rhs.outarg.size()) return true;
	//if (outarg.size() > rhs.outarg.size()) return false;
	//auto rm = rhs.outarg.begin();
	//for (auto m : outarg)
	//{
	//	if (m < *rm) return true;
	//	if (*rm < m) return false;
	//}
	return false;
}

bool CSignals::operator < (const CSignals & rhs) const
{
	if (!next && rhs.next) return true;
	if (next && !rhs.next) return false;
	CTimeSeries *p = (CTimeSeries *)this;
	if (*p < *(CTimeSeries *)&rhs) return true;
	if (*(CTimeSeries *)&rhs < *p) return false;
	if (next && rhs.next && *next < *(CTimeSeries *)rhs.next) return true;
	return false;
}

bool CVar::operator < (const CVar & rhs) const
{
	if (cell.size() < rhs.cell.size()) return true;
	if (cell.size() > rhs.cell.size()) return false;
	auto rm = rhs.cell.begin();
	for (auto m : cell)
	{
		if (m < *rm) return true;
	}
	auto rs = rhs.strut.begin();
	for (auto it = strut.begin(); it != strut.end(); it++, rs++)
	{
		if (rs == rhs.strut.end()) return false;
		if ((*it).first < (*rs).first) return true;
		if ((*it).first > (*rs).first) return false;
		return (*it).second < (*rs).second;
	}
	if (rs != rhs.strut.end()) return true;
	auto rss = rhs.struts.begin();
	for (auto it = struts.begin(); it != struts.end(); it++, rs++)
	{
		if (rss == rhs.struts.end()) return false;
		if ((*it).first < (*rss).first) return true;
		if ((*it).first > (*rss).first) return false;
		return (*it).second < (*rss).second;
	}
	if (rss != rhs.struts.end()) return true;
	CSignals *p = (CSignals *)this;
	CSignals *q = (CSignals*)&rhs;
	if (*p < *q) return true;
	return false;
}

body& body::operator<=(const body& rhs)
{ // Ghost assignment--reset existing and ghost copy from the RHS 
	Reset();
	ghost = true;
	buf = rhs.buf;
	nSamples = rhs.nSamples;
	nGroups = rhs.nGroups;
	bufBlockSize = rhs.bufBlockSize;
	//parg = rhs.parg;
//	resOutput = move(rhs.resOutput); // Cannot move because it is const... Then how? 11/29/2019
	return *this;
}
body& body::operator<=(body* rhs)
{ // Ghost assignment--reset existing and ghost copy from the RHS 
	Reset();
	ghost = true;
	buf = rhs->buf;
	nSamples = rhs->nSamples;
	nGroups = rhs->nGroups;
	bufBlockSize = rhs->bufBlockSize;
	//parg = rhs->parg;
	return *this;
}

CSignal& CSignal::operator<=(const CSignal& rhs)
{
	body::operator<=(rhs);
	tmark = rhs.tmark;
	fs = rhs.fs;
	snap = rhs.snap;
	return *this;
}
CSignal& CSignal::operator<=(CSignal* rhs)
{
	body::operator<=(rhs);
	tmark = rhs->tmark;
	fs = rhs->fs;
	snap = rhs->snap;
	return *this;
}

CTimeSeries& CTimeSeries::operator<=(const CTimeSeries& rhs)
{
	CSignal::operator<=(rhs);
	//outarg = rhs.outarg;
	if (rhs.chain)
	{
		chain = new CTimeSeries;
		*chain <= *rhs.chain;
	}
	else
	{
		if (!ghost) delete chain;
		chain = NULL;
	}
	return *this;
}
CTimeSeries& CTimeSeries::operator<=(CTimeSeries* rhs)
{
	CSignal::operator<=(rhs);
	//outarg = rhs->outarg;
	if (rhs->chain)
	{
		chain = new CTimeSeries;
		*chain <= *rhs->chain;
	}
	else
	{
		if (!ghost) delete chain;
		chain = NULL;
	}
	return *this;
}

CSignals& CSignals::operator<=(const CSignals& rhs)
{
	CTimeSeries::operator<=(rhs);
	if (rhs.next)
	{
		if (next)
			delete next;
		else
			next = new CSignals;
		*next <= rhs.next;
	}
	return *this;
}
CSignals& CSignals::operator<=(CSignals* rhs)
{
	CTimeSeries::operator<=(rhs);
	if (rhs->next)
	{
		if (next)
			delete next;
		else
			next = new CSignals;
		*next <= rhs->next;
	}
	return *this;
}

float CSignal::begint(unsigned int id0, unsigned int len, void* p) const
{
	return tmark + id0 * 1000.f / fs;
}

float CSignal::endt(unsigned int id0, unsigned int len, void* p) const
{
	return begint(id0, len) + dur(id0, len);
}

float CSignal::length(unsigned int id0, unsigned int len, void* p) const
{
	if (len == 0) len = nSamples;
	if (IsString())
		return (float)strlen(strbuf);
	else
		return (float)len;
}
float CSignal::dur(unsigned int id0, unsigned int len, void* p) const
{
	if (len == 0) len = nSamples;
	return 1000.f / fs * len;
}

inline static float _getdB(float x)
{
	// 3 dB is added to make rms of full scale sinusoid 0 dB
	return 20 * log10f(x) + 3.0103f;
}
// Left for Legacy use only, called by SetLevel()
// Upgrading to follow the new convention in  CSignal __rms(float* buf, unsigned int len, void* pargin, void* pargout)
// takes more work than desired. 1/6/2022
float CSignal::RMS(unsigned int id0, unsigned int len, void* p) const
{
	if (len == 0) len = nSamples;
	if (len == 0) return std::numeric_limits<float>::infinity();
	float val(0);
	for_each(buf + id0, buf + id0 + len, [&val](float& v) {val += v * v; });
	return _getdB(sqrt(val / len));
}
