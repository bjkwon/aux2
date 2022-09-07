#include "skope.h"
#include "skope_exception.h"
#include <assert.h>


void skope::get_nodes_left_right_sides(const AstNode* pnode, const AstNode** plhs, const AstNode** prhs)
{
	*plhs = *prhs = NULL;
	*prhs = pnode;
	if (!pnode) return;
	if (pnode->child) 
	{
		*plhs = pnode;
		*prhs = pnode->child;
	}
}

void skope::eval_index(const AstNode* pInd, const CVar &varLHS, CVar &index)
{
	// input: pInd, psigBase
	// output: index -- sig holding all indices

	// process the first index
	unsigned int len;
	ends.push_back(find_endpoint(pInd, varLHS));
	try {
		skope tp(this);
		if (pInd->type == T_FULLRANGE)
		{ // x(:,ids) or x(:)
			index.UpdateBuffer((unsigned int)ends.back());
			for (int k = 0; k < (int)index.nSamples; k++)	index.buf[k] = k + 1;
		}
		else
			index = Compute(pInd);
		if (index.IsLogical()) index_array_satisfying_condition(index);
		// process the second index, if it exists
		if (pInd->next)
		{
			if (varLHS.nGroups > 1 && index.nSamples > 1)
				index.nGroups = index.nSamples;
			AstNode* p = pInd->next;
			CVar isig2;
			if (p->type == T_FULLRANGE)
			{// x(ids,:)
				len = varLHS.Len();
				isig2.UpdateBuffer(len);
				for (unsigned int k = 0; k < len; k++)	isig2.buf[k] = k + 1;
			}
			else // x(ids1,ids2)
			{
				//endpoint for the second arg in 2D is determined here
				ends.push_back((double)varLHS.Len());
				isig2 = Compute(p);
				ends.pop_back(); // pop 2-D end value from the stack here
			}
			auto mx = isig2._max();
			auto nx = varLHS.Len();
			if (isig2.IsLogical()) index_array_satisfying_condition(isig2);
			else if (isig2._max() > (double)varLHS.Len())
			{
				ostringstream oss;
				oss << "max of 2nd index " << isig2._max() << " exceeds" << varLHS.Len() << ".";
				throw exception_range(this, pInd, oss.str().c_str(), "");
			}
			interweave_indices(index, isig2, varLHS.Len());
		}
	}
	catch (skope_exception e) {
		e.outstr += "invalid indexing... code to be refactored";
		throw e;
	}
	ends.pop_back();
}

const CVar* skope::get_cell_item(const AstNode* plhs, const CVar &cellobj)
{ // from CNodeProbe::cell_indexing
	CVar* out;
	size_t cellind = (size_t)(int)Compute(plhs->alt->child)->value(); // check the validity of ind...probably it will be longer than this.
	ostringstream oss;
	if (cellobj.type() & TYPEBIT_CELL)
	{
		if (cellind > cellobj.cell.size())
		{
			oss << "Cell index " << cellind;
			throw exception_range(*this, plhs->alt, oss.str().c_str(), plhs->str);
		}
		return &cellobj.cell[cellind - 1];
		//if (plhs->child && root->child && pbase->searchtree(root->child, T_REPLICA))
		//	pbase->replica_prep(&pbase->Sig);
	}
	else
	{ // in this case x{2} means second chain
		if (cellind > cellobj.CountChains())
		{
			oss << "Cell index " << cellind;
			throw exception_range(*this, plhs->alt, oss.str().c_str(), plhs->str);
		}
		CTimeSeries* pout = (CTimeSeries*)&cellobj;
		for (size_t k = 0; k < cellind; k++, pout = pout->chain) {}
		out = (CVar*)pout;
	}
	return out;
}
// left var is available through Vars.find
void skope::eval_lhs(const AstNode* plhs, const AstNode* prhs, CVar &lhs_index, CVar& RHS, uint16_t &typelhs, bool &contig, const CVar* cell_item)
{
	// in: plhs, prhs
	// out: indices of left var, right CVar object
	ostringstream out;
	// must be eihter TID or N_VECTOR
	contig = true;
	if (plhs->type == N_VECTOR)
	{

	}
	else // assume TID
	{
		const CVar* pvarLHS;
		// if lvalue is not eligible for a statement, throw here
		if (prhs) {
			RHS = Compute(prhs);
			if (pEnv->IsValidBuiltin(plhs->str))
			{
				out << "LHS must be an l-value. " << plhs->str << " is a built-in function.";
				throw exception_etc(*this, plhs, out.str()).raise();
			}
			// if var is not defined, type is null
			auto itvar = Vars.find(plhs->str);
			if (itvar == Vars.end())
			{ // if left var is not defined
				if (plhs->alt)
				{
					out << plhs->str << " is not a variable or built-in function.";
					throw exception_etc(*this, plhs, out.str()).raise();
				}
				typelhs = TYPEBIT_NULL;
				return;
			}
			pvarLHS = &(itvar->second);
		}
		else {
			pvarLHS = cell_item;
		}
		if (!plhs->alt)
		{ // left var is available but no indexing is indicated--> replacing it with a new RHS
			typelhs = TYPEBIT_NULL;
			return;
		}
		typelhs = pvarLHS->type();
		if ((typelhs & TYPEBIT_CELL)) {
			// check if ind is valid in x{ind} -- todo 9/5/2022
			size_t cellind;
			if (!plhs->alt) { // this is not a possible path
				typelhs = TYPEBIT_NULL;
				return;
			}
			cellind = (size_t)(int)Compute(plhs->alt->child)->value(); // check the validity of ind...probably it will be longer than this.
				lhs_index.SetValue(cellind);
				return;
		}
		// check type.. mask with 0xFFF4 -- to mask the last two bits zero (clean the length bits) to check the type only (no length)
		auto typerhs = RHS.type();
		if (typerhs > 0 && (typelhs & (uint16_t)0xFFF4) != (typerhs & (uint16_t)0xFFF4) )
		{
			out << "LHS and RHS have different object type.";
			throw exception_etc(*this, plhs, out.str()).raise();
		}
		if (plhs->alt->type == N_TIME_EXTRACT)
		{
			// if lhs var is not audio, throw
			if (!ISAUDIO(typelhs))
			{
				out << "LHS must be audio.";
				throw exception_etc(*this, plhs, out.str()).raise();
			}
			lhs_index.UpdateBuffer(2); // use it for time indices
			Compute(plhs->alt->child);
			lhs_index.buf[0] = Sig.buf[0];
			Compute(plhs->alt->child->next);
			lhs_index.buf[1] = Sig.buf[0];
			return;
		}
		// x(ind): process ind
		eval_index(plhs->alt->child, *pvarLHS, lhs_index);

		//Check if index is within range 
		if (lhs_index._max() > pvarLHS->nSamples) {
			out << lhs_index._max();
			throw exception_range(*this, plhs, out.str(),"").raise();
		}

		//check size
		if (RHS.nSamples>1 && lhs_index.nSamples > 1)
		if (lhs_index.nSamples != RHS.nSamples || lhs_index.nGroups != RHS.nGroups)
		{
			out << "LHS and RHS have different dimension (lengths).";
			throw exception_etc(*this, plhs, out.str()).raise();
		}
		for (uint64_t k = 0; k < lhs_index.nSamples-1; k++)
		{
			if (lhs_index.buf[k] + 1. != lhs_index.buf[k + 1])
			{
				contig = false;
				break;
			}
		}
	}
}
void skope::right_to_left(const AstNode* plhs, const CVar& lhs_index, const CVar& robj, uint16_t typelhs, bool contig, CVar* cellitem)
{
	ostringstream out;
	if (typelhs == TYPEBIT_NULL)
	{
		if (cellitem) {
			*cellitem = robj;
		}
		else
			SetVar(plhs->str, &Sig);
	}
	else
	{
		CVar* lobj;
		if (cellitem)
			lobj = cellitem;
		else
			lobj = &Vars.find(plhs->str)->second;
		if (typelhs & TYPEBIT_CELL) {
			const CVar* cellitem = get_cell_item(plhs, *lobj);
			CVar index;
			if (!plhs->alt) {
				right_to_left(plhs->alt, index, robj, typelhs, contig, (CVar*)cellitem);
				return;
			}
			if (plhs->alt->alt) {
//				assert(plhs->alt->alt->type == N_TIME_EXTRACT);
				// compute for RHS is skipped because robj is already prepared.
				eval_lhs(plhs->alt, NULL, index, (CVar&)robj, typelhs, contig, cellitem); // robj is not updated, just provided only as a reference
				right_to_left(plhs->alt, index, robj, typelhs, contig, (CVar*)cellitem);
				return;
			}
			typelhs = TYPEBIT_NULL;
			right_to_left(plhs->alt, index, robj, typelhs, contig, (CVar*)cellitem);
			return;
		}

		if (plhs->alt->type == N_TIME_EXTRACT)
		{
			if (cellitem)
				insertreplace(plhs, robj, lhs_index, cellitem);
			else
				insertreplace(plhs, robj, lhs_index);
		}
		else
		{
			float* pv;
			if (robj.nSamples == 0)
			{ // truncate the LHS var buffer
				if (contig)
				{
					memmove(lobj->buf + (uint64_t)lhs_index.buf[0] - 1, lobj->buf + (uint64_t)lhs_index.buf[lhs_index.nSamples - 1], lhs_index.nSamples * sizeof(float));
					lobj->nSamples -= lhs_index.nSamples;
				}
				else
				{
					{
						out << "a(range)=[] -- range should be consecutive.";
						throw exception_etc(*this, plhs, out.str()).raise();
					}
				}
			}
			else if (robj.nSamples == 1)
			{ // fill the buffer with the RHS value 
				for (uint64_t k = 0; k < lhs_index.nSamples; k++)
					lobj->buf[(uint64_t)lhs_index.buf[k]-1] = robj.buf[0];
			}
			else if (lhs_index.nSamples == 1)
			{
				auto nCopied = lobj->nSamples + 1 - (uint16_t)lhs_index.buf[0];
				lobj->UpdateBuffer(lobj->nSamples + robj.nSamples);
				pv = lobj->buf;
				auto j = (uint64_t)lhs_index.buf[0] - 1;
				memmove(&pv[j + robj.nSamples], &pv[j], nCopied  * sizeof(float));
				auto val = robj.buf[0];
				auto pval = robj.buf;
				memcpy(&pv[j], pval, robj.nSamples * sizeof(float));
			}
			else if (lhs_index.nSamples == robj.nSamples)
				for (uint64_t k = 0; k < robj.nSamples; k++)
					lobj->buf[(uint64_t)lhs_index.buf[k]-1] = robj.buf[k];
			else 
			{
				out << "no way...";
				throw exception_etc(*this, plhs, out.str()).raise();
			}
		}
	}
}

CVar* skope::process_block(const AstNode* pnode)
{
	CVar* res = NULL;
	return res;
}

CVar* skope::process_statement(const AstNode* pnode)
{
	const AstNode* plhs;
	const AstNode* prhs;
	get_nodes_left_right_sides(pnode, &plhs, &prhs);
	Compute(prhs); // making Sig ready
	if (plhs)
	{
		uint16_t typelhs;
		bool contig;
		CVar index, RHS;
		eval_lhs(plhs, prhs, index, RHS, typelhs, contig);
		right_to_left(plhs, index, RHS, typelhs, contig);
	}
	return &Sig;
}

void skope::insertreplace(const AstNode* plhs, const CVar& robj, const CVar& indsig, CVar *lobj)
{
	AstNode* p = plhs->alt;
	if (!lobj)
		lobj = &Vars.find(plhs->str)->second;
	uint64_t id1;
	if ((p->alt && p->alt->type == N_TIME_EXTRACT) || // x{id}(t1~t2) = ...sqrt
		p->type == N_TIME_EXTRACT || (p->next && p->next->type == N_IDLIST))  // s(repl_RHS1~repl_RHS2)   or  cel{n}(repl_RHS1~repl_RHS2)
	{
		if (0)//(repl_RHS) //direct update of buf
		{
			id1 = (unsigned int)round(indsig.buf[0] * lobj->GetFs() / 1000.);
			memcpy(lobj->logbuf + id1 * lobj->bufBlockSize, robj.buf, robj.nSamples * robj.bufBlockSize);
			if (lobj->next)
				memcpy(lobj->next->logbuf + id1 * lobj->bufBlockSize, robj.next->buf, robj.nSamples * robj.bufBlockSize);
		}
		else
			lobj->ReplaceBetweenTPs(robj, indsig.buf[0], indsig.buf[1]);
	}
}