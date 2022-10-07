#include <algorithm>
#include "skope.h"
#include "skope_exception.h"
#include "typecheck.h"
#include "utils.h"
#include <assert.h>

CVar* skope::BLOCK(const AstNode* pnode)
{
	for (AstNode* p = pnode->next; p && !fExit && !fBreak; p = p->next)
	{
		pLast = p;
		//				hold_at_break_point(p);
		Compute(p);
		//			pgo = NULL; // without this, go lingers on the next line 2/9/2019
		Sig.Reset(1); // without this, fs=3 lingers on the next line 2/9/2019
	}
	return &Sig;
}

CVar* skope::FOR(const AstNode* pnode)
{
	AstNode* p = pnode->child;
	fExit = fBreak = false;
	CVar isig = Compute(p->child);
	//isig must be a vector
	ensureVector3(*this, p, isig, "For-loop index variable must be a vector.");
	//If index variable already exists in the scope, throw
	if (GetVariable(p->str, NULL))
		exception_etc(*this, p, " ""for"" Index variable already exists outside the for loop").raise();
	for (unsigned int i = 0; i < isig.nSamples && !fExit && !fBreak; i++)
	{
		CVar tp(isig.buf[i]);
		SetVar(p->str, &tp); // This is OK, SetVar of non-GO object always makes a duplicate object (as opposed to SetVar of Go obj grabbing the reference)
		//	assuming that (pnode->alt->type == N_BLOCK)
		// Now, not going through N_BLOCK 1/4/2020
		// 1) When running in a debugger, it must go through N_BLOCK
		// 2) check if looping through pa->next is bullet-proof
		for (AstNode* pa = pnode->alt->next; pa; pa = pa->next)
		{
			pLast = pa;
			//					hold_at_break_point(pa);
			Compute(pa);
		}
	}
	fBreak = false;
	return &Sig;
}

CVar* skope::IF(const AstNode* pnode)
{
	if (pnode) {
		AstNode* p = pnode->child;
		pLast = p;
		if (checkcond(p))
			Compute(p->next);
		else if (pnode->alt)
			Compute(pnode->alt);
	}
	return &Sig;
}

CVar* skope::WHILE(const AstNode* pnode)
{
	AstNode* p = pnode->child;
	fExit = fBreak = false;
	while (checkcond(p) && !fExit && !fBreak)
		Compute(pnode->alt);
	fBreak = false;
	return &Sig;
}

CVar* skope::TRY(const AstNode* pnode)
{
	AstNode* p = pnode->child;
	inTryCatch++;
	Try_here(pnode, p);
	return &Sig;
}

CVar* skope::CATCH(const AstNode* pnode)
{
	AstNode* p = pnode->child;
	inTryCatch--; //???? Maybe no longer needed?? 3/7/2020
	// p is T_ID for ME (exception message caught)
	// continue here............1/12/2020
	Compute(pnode->next);
	return &Sig;
}

CVar* skope::ID(const AstNode* pnode)
{
	return TID((AstNode*)pnode, pnode->child);
}

CVar* skope::TSEQ(const AstNode* pnode)
{
	return TSeq((AstNode*)pnode, pnode->child);
}

CVar* skope::NUMBER(const AstNode* pnode)
{
	Sig.SetValue(pnode->dval);
	return TID(pnode->alt, pnode->child, &Sig);
}

CVar* skope::STRING(const AstNode* pnode)
{
	Sig.Reset();
	Sig.SetString(pnode->str);
	return TID(pnode->alt, pnode->child, &Sig);
}

CVar* skope::MATRIX(const AstNode* pnode)
{
	if (pnode->child) throw_LHS_lvalue(pnode, false);
	NodeMatrix(pnode);
	return Dot(pnode->alt);
}

CVar* skope::VECTOR(const AstNode* pnode)
{
	AstNode* p = pnode->child;
	if (p)
	{ // if p (RHS exists), evaluate RHS first; then go to LHS (different from usual ways)
		string emsg;
		string funcname;
		if (p->type == T_ID)
			funcname = p->str;
		if (p->alt && p->alt->type == N_STRUCT)
			funcname = p->alt->str;
		// Now, evaluate RHS
		// why not TID(((AstNode*)pnode->str), p), which might be more convenient? (that's the "inner" N_VECTOR node)
		// Because then there's no way to catch [out1 out2].sqrt = func
		return TID((AstNode*)pnode, p);
	}
	else
	{
		NodeVector(pnode);
		return Dot(pnode->alt);
	}
}

CVar* skope::REPLICA(const AstNode* pnode)
{
	return TID((AstNode*)pnode, NULL, &replica); //Make sure replica has been prepared prior to this
}

CVar* skope::ENDPOINT(const AstNode* pnode)
{
	CVar tsig;
	tsig.SetValue(ends.back());
	return TID((AstNode*)pnode, NULL, &tsig); //Make sure endpoint has been prepared prior to this
}

CVar* skope::ARITH_PLUS(const AstNode* pnode)
{
	CVar tsig;
	AstNode* p = pnode->child;
	tsig = Compute(p->next);
	blockCellStruct2(*this, pnode, Sig);
	blockString2(*this, pnode, Sig);
	Compute(p);
	blockCellStruct2(*this, pnode, Sig);
	blockString2(*this, pnode, Sig);
	if (Sig.type() == 0) Sig.SetValue(0.);
	if (tsig.type() == 0) tsig.SetValue(0.);
	Sig += tsig;
	return TID((AstNode*)pnode->alt, NULL, &Sig);
}

CVar* skope::ARITH_MINUS(const AstNode* pnode)
{
	CVar tsig;
	AstNode* p = pnode->child;
	tsig = -*Compute(p->next);
	blockCellStruct2(*this, pnode, Sig);
	blockString2(*this, pnode, Sig);
	Compute(p);
	blockCellStruct2(*this, pnode, Sig);
	blockString2(*this, pnode, Sig);
	if (Sig.type() == 0) Sig.SetValue(0.);
	if (tsig.type() == 0) tsig.SetValue(0.);
	Sig += tsig;
	return TID((AstNode*)pnode->alt, NULL, &Sig);
}

CVar* skope::ARITH_MULT(const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = Compute(p);
	blockCellStruct2(*this, pnode, Sig);
	blockString2(*this, pnode, Sig);
	Compute(p->next);
	blockCellStruct2(*this, pnode, Sig);
	blockString2(*this, pnode, Sig);
	if (Sig.type() == 0) Sig.SetValue(0.f);
	if (tsig.type() == 0) tsig.SetValue(0.f);
	// reciprocal should be after blocking string (or it would corrupt the heap) 6/3/2020
	Sig *= tsig;
	return TID((AstNode*)pnode->alt, NULL, &Sig);
}

CVar* skope::ARITH_DIV(const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = Compute(p);
	blockCellStruct2(*this, pnode, Sig);
	blockString2(*this, pnode, Sig);
	Compute(p->next);
	blockCellStruct2(*this, pnode, Sig);
	blockString2(*this, pnode, Sig);
	if (Sig.type() == 0) Sig.SetValue(0.f);
	if (tsig.type() == 0) tsig.SetValue(0.f);
	// reciprocal should be after blocking string (or it would corrupt the heap) 6/3/2020
	Sig.reciprocal();
	Sig *= tsig;
	return TID((AstNode*)pnode->alt, NULL, &Sig);
}

CVar* skope::MATRIXMULT(const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = Compute(p);
	blockCellStruct2(*this, pnode, Sig);
	blockString2(*this, pnode, Sig);
	ensureVector1(*this, pnode, tsig);
	Compute(p->next);
	ensureVector1(*this, pnode, Sig);
	Sig = (CSignals)tsig.matrixmult(&Sig);
	return TID((AstNode*)pnode, NULL, &Sig);
}

CVar* skope::ARITH_MOD(const AstNode* pnode)
{
	//only in the format of A %= B
	AstNode* p = pnode->child;
	((AstNode*)pnode)->type = N_CALL;
	((AstNode*)pnode)->str = strdup("mod");
	((AstNode*)pnode)->alt = p->next;
	p->next = NULL;
	Sig = replica;
	HandleAuxFunctions(pnode); // Assuming that current body content (Sig) is already prepared...is it true? 8/23/2018
	return &Sig;
}

CVar* skope::TRANSPOSE(const AstNode* pnode)
{
	Transpose(pnode, pnode->child);
	return TID((AstNode*)pnode->alt, NULL, &Sig);
}

CVar* skope::NEGATIVE(const AstNode* pnode)
{
	-*Compute(pnode->child);
	blockString2(*this, pnode, Sig);
	return TID((AstNode*)pnode->alt, NULL, &Sig);
}

CVar* skope::TIMESHIFT(const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = Compute(p->next);
	blockCellStruct2(*this, pnode, Sig);
	ensureScalar1(*this, pnode, Sig, ">>");
	Compute(p);
	ensureAudio1(*this, pnode, Sig, ">>");
	Sig >>= tsig.value();
	return TID((AstNode*)pnode, NULL, &Sig);
}

CVar* skope::CONCAT(const AstNode* pnode)
{
	Concatenate(pnode, pnode->child);
	return TID((AstNode*)pnode->alt, NULL, &Sig);
}

CVar* skope::LOGIC(const AstNode* pnode)
{
	ConditionalOperation(pnode, pnode->child);
	return TID((AstNode*)pnode, NULL, &Sig);
}


CVar* skope::LEVELAT(const AstNode* pnode)
{
	SetLevel(pnode, pnode->child);
	return TID((AstNode*)pnode->alt, NULL, &Sig);
}

CVar* skope::INITCELL(const AstNode* pnode)
{
	return InitCell(pnode, pnode->child);
}

CVar* skope::BREAK(const AstNode* pnode)
{
	fBreak = true;
	return &Sig;
}

CVar* skope::RETURN(const AstNode* pnode)
{
	fExit = true;
	return &Sig;
}

