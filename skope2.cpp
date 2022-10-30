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

#include <sstream>
#include <list>
#include <algorithm>
#include <exception>
#include <math.h>
#include <time.h>
#include "aux_classes.h"
#include "skope.h"
#include "typecheck.h"
#include "deque"

#include <algorithm> // for lowercase
#include <assert.h>

#include "psycon.tab.h"
#include "skope_exception.h"

void skope::replica_prep(CVar* psig)
{// For GO's, prepping replica is not simple, because a copy of the psig doesn't represent the GO.
 // Therefore, CSIG_HDLARRAY data type is used to carry it forward,
 // because an CSIG_HDLARRAY object can be copied without worries.
 // Note: in this case CSIG_HDLARRAY with size of one can be created. Keep that in mind.
	//if (psig->IsGO())
	//{
	//	//if pgo is already a CSIG_HDLARRAY with multiple elements
	//	if (pgo->GetType() == CSIG_HDLARRAY)
	//		replica = *pgo;
	//	else
	//	{
	//		vector<CVar*> tp(1, pgo);
	//		replica = *MakeGOContainer(tp);
	//	}
	//}
	//else
		replica = *psig;
}

//void CNodeProbe::insertreplace(const AstNode *pnode, CVar &sec, CVar &indsig)
//{
//}

CVar * CNodeProbe::TID_indexing(const AstNode* pnode, AstNode *pLHS, AstNode *pRHS)
{

	return psigBase;
}

CVar * CNodeProbe::TimeExtract(const AstNode *pnode, AstNode *p)
{
	if (!psigBase)
		throw exception_etc(*pbase, pnode, "TimeExtract(): null psigBase").raise();
	ensureAudio2(*pbase, pnode, *psigBase);

	float endpoint;
	CTimeSeries *pts = psigBase;
	for (; pts; pts = pts->chain)
		endpoint = pts->CSignal::endt();
	if (psigBase->next)
	{
		pts = psigBase->next;
		endpoint = max(endpoint, pts->CSignal::endt());
	}
	pbase->ends.push_back(endpoint);
	vector<float> tpoints = pbase->gettimepoints(pnode, p);
	CVar out(*psigBase);
	out.Crop(tpoints[0], tpoints[1]);
	pbase->ends.pop_back();
	return &(pbase->Sig = out);
}

bool skope::builtin_func_call(CNodeProbe &diggy, AstNode *p)
{
	if (p->type == T_ID || p->type == N_STRUCT)
	{
		if (p->str[0] == '$' || pEnv->IsValidBuiltin(p->str)) // hook bypasses IsValidBuiltin
		{
			if (diggy.root->child)
				if (!diggy.root->alt)
					throw exception_etc(*this, diggy.root, "LHS must be an l-value. Isn't it a built-in function?").raise();
			HandleAuxFunction(p, diggy.root);
			return true;
		}
	}
	return false;
}

CNodeProbe::CNodeProbe(skope *past, AstNode *pnode, CVar *psig)
{
	psigBase = NULL;
	pbase = past;
	root = pnode; //what's this?
	psigBase = psig; // NULL except for T_REPLICA
	lhsref_single = false;
	lhsref = NULL;
}

CVar &CNodeProbe::ExtractByIndex(const AstNode *pnode, AstNode *p)
{
	CVar tsig, isig;
	if (!p->child)	
		throw exception_etc(*pbase, pnode, string(pnode->str) + string(" variable index missing")).raise();
	if (p->child->type == T_FULLRANGE && ISTEMPORAL(pbase->Sig.type()))
	{
		// nothing 
	}
	else
	{
		eval_indexing(p->child, isig);
		if (!(isig.type() & 1)) // has more than one element. 
			lhsref_single = false;
		if (isig._max() > psigBase->nSamples) // can be replaced with psigBase->nSamples
		{
			ostringstream oss;
			oss << isig._max();
			throw exception_range(*pbase, pnode, oss.str().c_str(), varname);
		}
		pbase->Sig = extract(pnode, isig);
	}
	return pbase->Sig;
}

CVar * CNodeProbe::extract(const AstNode *pnode, CTimeSeries &isig)
{
	CSignals out((psigBase)->GetFs());
	CTimeSeries *p = &out;
	uint16_t tp = psigBase->type();
	out.UpdateBuffer(isig.nSamples);
	if (isig._min() <= 0.)
	{
		ostringstream outstream;
		outstream << "Invalid index " << "for " << varname << " : " << (int)isig._min() << " (must be positive)";
		throw exception_etc(*pbase, pnode, outstream.str()).raise();
	}
	if (ISCOMPLEX(tp))
	{
		out.SetComplex();
		for (unsigned int i = 0; i < isig.nSamples; i++)
			out.cbuf[i] = (psigBase)->cbuf[(int)isig.buf[i] - 1];
	}
	else if (ISBOOL(tp))
	{
		out.MakeLogical();
		for (unsigned int i = 0; i < isig.nSamples; i++)
			out.logbuf[i] = (psigBase)->logbuf[(int)isig.buf[i] - 1];
	}
	else if (ISSTRING(tp))
	{
		out.UpdateBuffer(isig.nSamples + 1); // make room for null 
		for (unsigned int i = 0; i < isig.nSamples; i++)
			out.strbuf[i] = (psigBase)->strbuf[(int)isig.buf[i] - 1];
		out.strbuf[out.nSamples - 1] = 0;
	}
	else
	{
		if (psigBase->IsGO())
		{
			if (isig._max() > psigBase->nSamples)
			{
				ostringstream oss;
				oss << isig._max();
				throw exception_range(*pbase, pnode, oss.str().c_str(), "");
			}
			if (isig.nSamples == 1)
			{
				size_t did = (size_t)isig.value() - 1;
				if (!psigBase->buf[did]) {
					// if Reset() resets a GO into a NULL, the next two lines are not necessary 10/1/2020
					psigBase->strut.clear();
					psigBase->struts.clear();
					psigBase->Reset(1);
				}
				else
				{
					CVar* tp = (CVar*)(INT_PTR)(psigBase)->buf[did];
					pbase->pgo = psigBase = tp;
					pbase->Sig = *psigBase;
				}
				return psigBase;
			}
			else
			{
				vector<INT_PTR> gos;
				for (unsigned int k = 0; k < isig.nSamples; k++)
				{
					size_t did = (size_t)isig.buf[k] - 1;
					CVar *tp = (CVar*)(INT_PTR)(psigBase)->buf[did];
					gos.push_back((INT_PTR)tp);
				}
//				psigBase = pbase->MakeGOContainer(gos);
				pbase->Sig = *psigBase;
				return psigBase;
			}
		}
		out.SetReal();
		if (ISAUDIO(tp))
		{
			CTimeSeries *_pisig = &isig;
			while (_pisig)
			{
				int cum(0), id(0), lastid = -2;
				vector<int> size2reserve;
				for (unsigned int k = 0; k < _pisig->nSamples; k++)
				{
					id = (int)_pisig->buf[k];
					if (id - lastid > 1)
					{
						if (lastid > 0) size2reserve.push_back(lastid);
						size2reserve.push_back(id);
					}
					lastid = id;
				}
				size2reserve.push_back((int)_pisig->buf[_pisig->nSamples - 1]);
				auto it = size2reserve.begin();
				p->UpdateBuffer(*(it + 1) - *it + 1);
				id = (int)_pisig->buf[0] - 1;
				p->tmark = (float)id / pbase->Sig.GetFs() * 1000.;
				it++; it++;
				lastid = (int)_pisig->buf[0] - 1;
				for (unsigned int i = 0; i < _pisig->nSamples; i++)
				{
					id = (int)_pisig->buf[i] - 1;
					if (id - lastid > 1)
					{
						cum = 0;
						CSignals *pchain = new CSignals(pbase->Sig.GetFs());
						pchain->UpdateBuffer(*(it + 1) - *it + 1);
						pchain->tmark = (float)id / pbase->Sig.GetFs() * 1000.;
						p->chain = pchain;
						p = pchain;
						it++; it++;
					}
					p->buf[cum++] = psigBase->buf[id];
					lastid = id;
				}
				_pisig = _pisig->chain;
				if (_pisig)
				{
					p->chain = new CTimeSeries;
					p = p->chain;
				}
			}
		}
		else // should be non-audio, non-time sequence data such as vector
		{
			int id = 0;
			for (unsigned int k = 0; k < isig.nSamples; k++)
			{
				auto ind = (unsigned int)isig.buf[k];
				out.buf[id++] = (psigBase)->buf[ind - 1];
			}
		}
	}
	out.bufType = psigBase->bufType;
	out.nGroups = isig.nGroups;
	pbase->Sig = out;
	return &pbase->Sig;
}

CVar &CNodeProbe::eval_indexing(const AstNode *pInd, CVar &isig)
{
	// input: pInd, psigBase
	// output: isig -- sig holding all indices

	// process the first index
	unsigned int len;
	pbase->ends.push_back(pbase->find_endpoint(pInd, psigBase));
	try {
		skope tp(pbase);
		if (pInd->type == T_FULLRANGE)
		{ // x(:,ids) or x(:)
			isig.UpdateBuffer((unsigned int)pbase->ends.back());
			for (int k = 0; k < (int)isig.nSamples; k++)	isig.buf[k] = k + 1;
		}
		else
			isig = pbase->Compute(pInd);
		if (isig.IsLogical()) pbase->index_array_satisfying_condition(isig);
		// process the second index, if it exists
		if (pInd->next)
		{
			if (psigBase->nGroups > 1 && isig.nSamples > 1)
				isig.nGroups = isig.nSamples;
			AstNode *p = pInd->next;
			CVar isig2;
			if (p->type == T_FULLRANGE)
			{// x(ids,:)
				len = psigBase->Len();
				isig2.UpdateBuffer(len);
				for (unsigned int k = 0; k < len; k++)	isig2.buf[k] = k + 1;
			}
			else // x(ids1,ids2)
			{
				//endpoint for the second arg in 2D is determined here
				pbase->ends.push_back((float)psigBase->Len());
				isig2 = pbase->Compute(p);
				pbase->ends.pop_back(); // pop 2-D end value from the stack here
			}
			auto mx = isig2._max();
			auto nx = psigBase->Len();
			if (isig2.IsLogical()) pbase->index_array_satisfying_condition(isig2);
			else if (isig2._max() > (float)psigBase->Len())
			{
				ostringstream oss;
				oss << "max of 2nd index " << isig2._max() << " exceeds" << psigBase->Len() << ".";
				throw exception_range(*pbase, pInd, oss.str().c_str(), "");
			}
			pbase->interweave_indices(isig, isig2, psigBase->Len());
		}
	}
	catch (skope_exception e) {
		e.outstr += "invalid indexing... code to be refactored";
		throw e;
	}
	pbase->ends.pop_back();
	return isig;
}

// As of 12/23/2021, I cannot track this down. What is it?
// Maybe it became obsolete when x(whatever_including_conditional) = (something) always uses N_ARGS for HLS???

CVar * CNodeProbe::TID_condition(const AstNode *pnode, AstNode *pLHS, AstNode *pRHS)
{
	unsigned int id = 0;
	CVar isig;
	eval_indexing(pLHS, isig);
	CVar * prhs = pbase->Compute(pRHS);
	if (prhs->IsScalar())
	{
		//go through chains 6/24/2019
		CTimeSeries *p = psigBase;
		CTimeSeries *pisig = &isig;
		while (p)
		{
			for (unsigned k = 0; k < pisig->nSamples; k++)
				p->buf[(int)pisig->buf[k] - 1] = prhs->value();
			p = p->chain;
			pisig = pisig->chain;
		}
	}
	else if (pbase->searchtree(pRHS, T_REPLICA))
	{
		//go through chains 6/24/2019
		CTimeSeries *p = psigBase;
		CTimeSeries *pisig = &isig;
		while (p)
		{
			//if (p->type() & TYPEBIT_TEMPORAL) // Check this part 9/4/2018, 11/22/2021
			//{
			//	// consolidate chained rhs with lhs
			//	for (CTimeSeries *cts = prhs; cts; cts = cts->chain)
			//	{
			//		// id translated from tmark for each chain
			//		int fs = cts->GetFs();
			//		id = (unsigned int)(cts->tmark / 1000. * fs + .5);
			//		memcpy(p->buf + id, cts->buf, cts->nSamples * sizeof(float));
			//	}
			//}
			//else
			{
				for (unsigned k = 0; k < pisig->nSamples; k++)
					p->buf[(int)pisig->buf[k] - 1] = prhs->buf[k];
			}
			p = p->chain;
			if (pisig) pisig = pisig->chain;
			else break;
		}
	}
	else if (prhs->IsEmpty())
	{
		//do we need to go through chains??? 6/24/2019
	//	insertreplace(pLHS, *prhs, isig);
		pbase->Sig.Reset();
	}
	else
		throw exception_etc(*pbase, pRHS, "Invalid RHS; For LHS with conditional indexing, RHS should be either a scalar, empty, or .. (self).").raise();
	pbase->SetVar(pnode->str, psigBase);
	if (isig.nSamples > 0)
	{
//		varname = pbase->Script;
	}
	return &pbase->Sig;
}

/* These two replace() member functions could be part of body in csignals.cpp for the point of logic,
but better to keep here because of exception handling (i.e., need pnode)
*/
CTimeSeries &CNodeProbe::replace(const AstNode *pnode, CTimeSeries *pobj, body &sec, body &index)
{
	//	this is to be used when items are replaced without changing the size.
	// except when sec is empty... in which case the "index"'ed items are deleted.
	// in that case index is assumed to be sorted ascending
	//if (index.nSamples == 0) return *pobj;
	//if (sec.bufBlockSize != pobj->bufBlockSize)
	//{
	//	if (pobj->bufBlockSize == 1 && sec.bufBlockSize != 1) sec.MakeLogical();
	//	else if (pobj->bufBlockSize == 8 && sec.bufBlockSize == 1) sec.SetReal();
	//	else if (pobj->bufBlockSize == 8 && sec.bufBlockSize == 16) pobj->SetComplex();
	//	else if (pobj->bufBlockSize == 16 && sec.bufBlockSize == 1) sec.SetComplex();
	//	else if (pobj->bufBlockSize == 16 && sec.bufBlockSize == 8) sec.SetComplex();
	//}
	//for (unsigned int k = 0; k < index.nSamples; k++)
	//{
	//	if (index.buf[k] < 1.)
	//		throw CAstException(RANGE, *pbase, pnode).proc("must be equal or greater than 1", "", (int)index.buf[k]);
	//	unsigned int id = (int)index.buf[k];
	//	if (id - index.buf[k] > .25 || index.buf[k] - id > .25)
	//		throw CAstException(RANGE, *pbase, pnode).proc("must be integer", "", id);
	//	if (id > pobj->nSamples)
	//		throw CAstException(RANGE, *pbase, pnode).proc("", "", id);
	//}
	//if (sec.nSamples == 0)
	//{
	//	int trace = (int)(index.buf[0] - 1);
	//	for (unsigned int k = 0; k < index.nSamples; k++)
	//	{
	//		unsigned int diff = (unsigned int)(k < index.nSamples - 1 ? index.buf[k + 1] - index.buf[k] : pobj->nSamples + 1 - index.buf[k]);
	//		diff--;
	//		memcpy(pobj->logbuf + trace * pobj->bufBlockSize, pobj->logbuf + (int)index.buf[k] * pobj->bufBlockSize, diff*pobj->bufBlockSize);
	//		trace += diff;
	//	}
	//	pobj->nSamples -= index.nSamples;
	//}
	//else
	//	for (unsigned int k = 0; k < index.nSamples; k++)
	//	{
	//		if (index.buf[k] < 1.)
	//			throw CAstException(RANGE, *pbase, pnode).proc("must be equal or greater than 1", "", (int)index.buf[k]);
	//		unsigned int id = (unsigned int)(index.buf[k]);
	//		if (id - index.buf[k] > .05 || index.buf[k] - id > .05)
	//			throw CAstException(RANGE, *pbase, pnode).proc("must be integer", "", id);
	//		if (id > pobj->nSamples)
	//			throw CAstException(RANGE, *pbase, pnode).proc("", "", id);
	//		id--; // zero-based
	//		if (sec.nSamples == 1) // items from id1 to id2 are to be replaced with sec.value()
	//			memcpy(pobj->logbuf + id * pobj->bufBlockSize, sec.logbuf, pobj->bufBlockSize);
	//		else
	//			memcpy(pobj->logbuf + id * pobj->bufBlockSize, sec.logbuf + k * pobj->bufBlockSize, pobj->bufBlockSize);
	//	}
	return *pobj;
}

CTimeSeries &CNodeProbe::replace(const AstNode *pnode, CTimeSeries *pobj, CSignal &sec, int id1, int id2)
{ // this replaces the data body between id1 and id2 (including edges) with sec
	//if (id1 < 0) throw CAstException(RANGE, *pbase, pnode).proc("replace index cannot be negative.", "", id1);
	//if (id2 < 0) throw CAstException(RANGE, *pbase, pnode).proc("replace index cannot be negative.", "", id2);
	//if (sec.bufBlockSize != pobj->bufBlockSize)
	//{
	//	if (pobj->bufBlockSize == 1 && sec.bufBlockSize != 1) sec.MakeLogical();
	//	else if (pobj->bufBlockSize == 8 && sec.bufBlockSize == 1) sec.SetReal();
	//	else if (pobj->bufBlockSize == 8 && sec.bufBlockSize == 16) pobj->SetComplex();
	//	else if (pobj->bufBlockSize == 16 && sec.bufBlockSize == 1) sec.SetComplex();
	//	else if (pobj->bufBlockSize == 16 && sec.bufBlockSize == 8) sec.SetComplex();
	//}
	////id1 and id2 are zero-based here.
	//if (id1 > (int)pobj->nSamples - 1) throw CAstException(RANGE, *pbase, pnode).proc("replace index1 ", "", id1);
	//if (id2 > (int)pobj->nSamples - 1) throw CAstException(RANGE, *pbase, pnode).proc("replace index2 ", "", id1);
	//unsigned int secnsamples = sec.nSamples;
	//if (sec.type() & TYPEBIT_STRING) secnsamples--;
	//if (secnsamples == 1) // no change in length--items from id1 to id2 are to be replaced with sec.value()
	//{
	//	if (sec.type() & TYPEBIT_LOGICAL)
	//		for (int k = id1; k <= id2; k++) pobj->strbuf[k] = sec.strbuf[0];
	//	else if (((CSignal *)&sec)->bufBlockSize == 16)
	//		for (int k = id1; k <= id2; k++) pobj->cbuf[k] = sec.cvalue();
	//	else
	//		for (int k = id1; k <= id2; k++) pobj->buf[k] = sec.value();
	//}
	//else
	//{
	//	unsigned int nAdd = secnsamples;
	//	unsigned int nSubtr = id2 - id1 + 1;
	//	unsigned int newLen = pobj->nSamples + nAdd - nSubtr;
	//	unsigned int nToMove = pobj->nSamples - id2 - 1;
	//	if (nAdd > nSubtr) pobj->UpdateBuffer(newLen);
	//	bool *temp;
	//	if (nAdd != nSubtr)
	//	{
	//		temp = new bool[nToMove*pobj->bufBlockSize];
	//		memcpy(temp, pobj->logbuf + (id2 + 1)*pobj->bufBlockSize, nToMove*pobj->bufBlockSize);
	//	}
	//	memcpy(pobj->logbuf + id1 * pobj->bufBlockSize, sec.buf, secnsamples*pobj->bufBlockSize);
	//	if (nAdd != nSubtr)
	//	{
	//		memcpy(pobj->logbuf + (id1 + secnsamples)*pobj->bufBlockSize, temp, nToMove*pobj->bufBlockSize);
	//		delete[] temp;
	//	}
	//	pobj->nSamples = newLen;
	//}
	return *pobj;
}
