#include <algorithm>
#include "skope.h"
#include "skope_exception.h"
#include "typecheck.h"
#include "utils.h"
#include <assert.h>

CVar* CAstSigEnv::BLOCK(skope* psk, const AstNode* pnode)
{
	for (AstNode* p = pnode->next; p && !psk->fExit && !psk->fBreak; p = p->next)
	{
		psk->pLast = p;
		//				hold_at_break_point(p);
		psk->process_statement(p);
		//			pgo = NULL; // without this, go lingers on the next line 2/9/2019
		psk->Sig.Reset(1); // without this, fs=3 lingers on the next line 2/9/2019
		if (psk->inTryCatch)
			psk->pTryLast = p;
	}
	return &psk->Sig;
}

CVar* CAstSigEnv::FOR(skope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	psk->fExit = psk->fBreak = false;
	CVar isig = psk->Compute(p->child);
	//isig must be a vector
	ensureVector3(*psk, p, isig, "For-loop index variable must be a vector.");
	//If index variable already exists in the scope, throw
	if (psk->GetVariable(p->str, NULL))
		exception_etc(*psk, p, " ""for"" Index variable already exists outside the for loop").raise();
	for (unsigned int i = 0; i < isig.nSamples && !psk->fExit && !psk->fBreak; i++)
	{
		CVar tp(isig.buf[i]);
		psk->SetVar(p->str, &tp); // This is OK, SetVar of non-GO object always makes a duplicate object (as opposed to SetVar of Go obj grabbing the reference)
		//	assuming that (pnode->alt->type == N_BLOCK)
		// Now, not going through N_BLOCK 1/4/2020
		// 1) When running in a debugger, it must go through N_BLOCK
		// 2) check if looping through pa->next is bullet-proof
		for (AstNode* pa = pnode->alt->next; pa; pa = pa->next)
		{
			psk->pLast = pa;
			//					hold_at_break_point(pa);
			psk->process_statement(pa);
		}
	}
	psk->fBreak = false;
	return &psk->Sig;
}

CVar* CAstSigEnv::IF(skope* psk, const AstNode* pnode)
{
	if (pnode) {
		AstNode* p = pnode->child;
		psk->pLast = p;
		if (psk->checkcond(p))
			psk->process_statement(p->next);
		else if (pnode->alt)
			psk->process_statement(pnode->alt);
	}
	return &psk->Sig;
}

CVar* CAstSigEnv::WHILE(skope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	psk->fExit = psk->fBreak = false;
	while (psk->checkcond(p) && !psk->fExit && !psk->fBreak)
		psk->process_statement(pnode->alt);
	psk->fBreak = false;
	return &psk->Sig;
}

CVar* CAstSigEnv::TRY(skope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	psk->inTryCatch++;
	psk->Try_here(pnode, p);
	return &psk->Sig;
}

CVar* CAstSigEnv::CATCH(skope* psk, const AstNode* pnode)
{
	// AstNode* p = pnode->child; // not necessary
	// p is T_ID for the catch variable (exception message caught), handled in catch{} in Try_here
	psk->inTryCatch--;
	psk->process_statement(pnode->next);
	return &psk->Sig;
}

CVar* CAstSigEnv::ID(skope* psk, const AstNode* pnode)
{
	return psk->TID((AstNode*)pnode, pnode->child);
}

CVar* CAstSigEnv::TSEQ(skope* psk, const AstNode* pnode)
{
	return psk->TSeq((AstNode*)pnode, pnode->child);
}

CVar* CAstSigEnv::NUMBER(skope* psk, const AstNode* pnode)
{
	psk->Sig.SetValue(pnode->dval);
	return psk->TID(pnode->alt, pnode->child, &psk->Sig);
}

CVar* CAstSigEnv::STRING(skope* psk, const AstNode* pnode)
{
	psk->Sig.Reset();
	psk->Sig.SetString(pnode->str);
	return psk->TID(pnode->alt, pnode->child, &psk->Sig);
}

CVar* CAstSigEnv::MATRIX(skope* psk, const AstNode* pnode)
{
	if (pnode->child) psk->throw_LHS_lvalue(pnode, false);
	psk->NodeMatrix(pnode);
	return psk->Dot(pnode->alt);
}

CVar* CAstSigEnv::VECTOR(skope* psk, const AstNode* pnode)
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
		return psk->TID((AstNode*)pnode, p);
	}
	else
	{
		psk->NodeVector(pnode);
		return psk->Dot(pnode->alt);
	}
}

CVar* CAstSigEnv::REPLICA(skope* psk, const AstNode* pnode)
{
	return psk->TID((AstNode*)pnode, NULL, &psk->replica); //Make sure replica has been prepared prior to this
}

CVar* CAstSigEnv::ENDPOINT(skope* psk, const AstNode* pnode)
{
	CVar tsig;
	tsig.SetValue(psk->ends.back());
	return psk->TID((AstNode*)pnode, NULL, &tsig); //Make sure endpoint has been prepared prior to this
}

CVar* CAstSigEnv::ARITH_PLUS(skope* psk, const AstNode* pnode)
{
	CVar tsig;
	AstNode* p = pnode->child;
	tsig = psk->Compute(p->next);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	psk->Compute(p);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	if (psk->Sig.type() == 0) psk->Sig.SetValue(0.);
	if (tsig.type() == 0) tsig.SetValue(0.);
	psk->Sig += tsig;
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* CAstSigEnv::ARITH_MINUS(skope* psk, const AstNode* pnode)
{
	CVar tsig;
	AstNode* p = pnode->child;
	tsig = -*psk->Compute(p->next);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	psk->Compute(p);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	if (psk->Sig.type() == 0) psk->Sig.SetValue(0.);
	if (tsig.type() == 0) tsig.SetValue(0.);
	psk->Sig += tsig;
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* CAstSigEnv::ARITH_MULT(skope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = psk->Compute(p);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	psk->Compute(p->next);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	if (psk->Sig.type() == 0) psk->Sig.SetValue(0.f);
	if (tsig.type() == 0) tsig.SetValue(0.f);
	// reciprocal should be after blocking string (or it would corrupt the heap) 6/3/2020
	psk->Sig *= tsig;
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* CAstSigEnv::ARITH_DIV(skope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = psk->Compute(p);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	psk->Compute(p->next);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	if (psk->Sig.type() == 0) psk->Sig.SetValue(0.f);
	if (tsig.type() == 0) tsig.SetValue(0.f);
	// reciprocal should be after blocking string (or it would corrupt the heap) 6/3/2020
	psk->Sig.reciprocal();
	psk->Sig *= tsig;
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* CAstSigEnv::MATRIXMULT(skope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = psk->Compute(p);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	ensureVector1(*psk, pnode, tsig);
	psk->Compute(p->next);
	ensureVector1(*psk, pnode, psk->Sig);
	psk->Sig = (CSignals)tsig.matrixmult(&psk->Sig);
	return psk->TID((AstNode*)pnode, NULL, &psk->Sig);
}

CVar* CAstSigEnv::ARITH_MOD(skope* psk, const AstNode* pnode)
{
	//only in the format of A %= B
	AstNode* p = pnode->child;
	((AstNode*)pnode)->type = N_CALL;
	((AstNode*)pnode)->str = strdup("mod");
	((AstNode*)pnode)->alt = p->next;
	p->next = NULL;
	psk->Sig = psk->replica;
	psk->HandleAuxFunctions(pnode); // Assuming that current body content (psk->Sig) is already prepared...is it true? 8/23/2018
	return &psk->Sig;
}

CVar* CAstSigEnv::TRANSPOSE(skope* psk, const AstNode* pnode)
{
	psk->Transpose(pnode, pnode->child);
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* CAstSigEnv::NEGATIVE(skope* psk, const AstNode* pnode)
{
	-*psk->Compute(pnode->child);
	blockString2(*psk, pnode, psk->Sig);
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* CAstSigEnv::TIMESHIFT(skope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = psk->Compute(p->next);
	blockCellStruct2(*psk, pnode, psk->Sig);
	ensureScalar1(*psk, pnode, psk->Sig, ">>");
	psk->Compute(p);
	ensureAudio1(*psk, pnode, psk->Sig, ">>");
	psk->Sig >>= tsig.value();
	return psk->TID((AstNode*)pnode, NULL, &psk->Sig);
}

CVar* CAstSigEnv::CONCAT(skope* psk, const AstNode* pnode)
{
	psk->Concatenate(pnode, pnode->child);
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* CAstSigEnv::LOGIC(skope* psk, const AstNode* pnode)
{
	psk->ConditionalOperation(pnode, pnode->child);
	return psk->TID((AstNode*)pnode, NULL, &psk->Sig);
}

CVar* CAstSigEnv::LEVELAT(skope* psk, const AstNode* pnode)
{
	psk->SetLevel(pnode, pnode->child);
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* CAstSigEnv::INITCELL(skope* psk, const AstNode* pnode)
{
	return psk->InitCell(pnode, pnode->child);
}

CVar* CAstSigEnv::BREAK(skope* psk, const AstNode* pnode)
{
	psk->fBreak = true;
	return &psk->Sig;
}

CVar* CAstSigEnv::RETURN(skope* psk, const AstNode* pnode)
{
	psk->fExit = true;
	return &psk->Sig;
}
