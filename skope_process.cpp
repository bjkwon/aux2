#include "skope.h"
#include "skope_exception.h"
#include <assert.h>
#include <deque>
#include <iostream>

bool skope::get_nodes_left_right_sides(const AstNode* pnode, const AstNode** plhs, const AstNode** prhs)
{
	replica.Reset();
	*plhs = NULL;
	*prhs = pnode;
	if (find(pEnv->type_arith_op.begin(), pEnv->type_arith_op.end(), pnode->type) != pEnv->type_arith_op.end())
		return false;
	if (find(pEnv->type_condition.begin(), pEnv->type_condition.end(), pnode->type) != pEnv->type_condition.end())
		return false;
	if (find(pEnv->type_blockflow.begin(), pEnv->type_blockflow.end(), pnode->type) != pEnv->type_blockflow.end())
		return false;
	if (!pnode)
		return false;
	if (pnode->child) 
	{
		*plhs = pnode;
		*prhs = pnode->child;
	}
	// if LHS is NULL, don't search replica on the RHS
	if (!*plhs) return false;
	// Check if RHS includes the replica
	return searchtree(*prhs, T_REPLICA) != NULL;
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
	size_t cellind = (size_t)(plhs->alt->dval); // check the validity of ind...probably it will be longer than this.
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
void skope::eval_lhs(const AstNode* plhs, const AstNode* prhs, CVar &lhs_index, CVar& RHS, uint16_t &typelhs, bool &contig, bool isreplica, const CVar* cell_item)
{
	// in: plhs, prhs
	// out: indices of left var, right CVar object
	ostringstream out;
	// must be eihter TID or N_VECTOR
	contig = true;
	if (plhs->type == N_VECTOR) {
		lhs = (AstNode*)plhs;
		const AstNode* pvector = (const AstNode*)plhs->str;
		// pvector->type should be N_VECTOR
		for (auto p = pvector->alt; p; p = p->next) {
			if (p->alt && p->alt->type==N_ARGS) {
				out << "Items in the LHS vector must not have indexing: " << p->str;
				throw exception_etc(*this, plhs, out.str()).raise();
			}
		}
		RHS = Compute(prhs); // Not to be removed 10/11/2022
		if (lhs) {
			outputbinding_for_eval_lhs(plhs);
		}
		else { // outputbinding was done at PrepareAndCallUDF
		}
	}
	else // assume TID
	{
		const CVar* pvarLHS;
		const CVar* struct_item = NULL;
		const AstNode* pstruct = NULL;
		// if lvalue is not eligible for a statement, throw here
		if (prhs) {
			if (pEnv->IsValidBuiltin(plhs->str))
			{
				out << "LHS must be an l-value. " << plhs->str << " is a built-in function.";
				throw exception_etc(*this, plhs, out.str()).raise();
			}
//			RHS = Compute(prhs); // DO SOMETHING--- RHS is computed twice..... 9/14/2022
			// if var is not defined, type is null
			auto itvar = Vars.find(plhs->str);
			if (itvar == Vars.end())
			{ // if left var is not defined
				if (isreplica) {
					out << "Replicator (..) cannot be used with undefined LHS.";
					throw exception_etc(*this, plhs, out.str()).raise();
				}
				// For N_STRUCT, in a....propn = RHS anything a to propn can be undefined
				// but a....propn(id) = RHS should issue an error
				bool alert_undefinedLHS = false;
				if (plhs->alt) {
					switch (plhs->alt->type) {
					case N_STRUCT:
						for (auto p = plhs; p; p = p->alt) {
							pstruct = p;
							if (!p->alt) 
								break;
						}
						if (pstruct->type== N_ARGS || pstruct->type == N_TIME_EXTRACT || pstruct->type == N_CELL)
							alert_undefinedLHS = true;
						break;
					case N_CELL:
						if (plhs->alt->dval != 0.)
							alert_undefinedLHS = true;
						break;
					case N_ARGS:
					case N_TIME_EXTRACT:
						alert_undefinedLHS = true;
						break;
					}
				}
				if (alert_undefinedLHS) {
					out << "The object on the LHS has not been defined and cannot be indexed.";
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
		// I think this was done--- x....prop = RHS; this works whether .prop was defined or not
		// Todo-- x....prop(id) = RHS   Assume .prop is available, then we need to do type-check and length-check for .prop 
		if (!plhs->alt)
		{ // left var is available but no indexing is indicated--> replacing it with a new RHS
			typelhs = TYPEBIT_NULL;
			return;
		}
		if (plhs->alt->type == N_STRUCT) // look at the prop item, not the base one
		{
			// x.prop1.prop2...propk...propn = RHS
			//  propk is the last item available as struct (i.e., .prop(k+1) has not been defined)
			//  struct_item is the CVar of .propk
			//  plhs (input) points to x
			//  pstruct (output) points to the node of .prop(k+1)
			//go down to the prop item. x.prop1.prop2.prop3....prop7 
			// If any of these items are not present, return with TYPEBIT_NULL
			struct_item = get_available_struct_item(plhs, &pstruct);
			// struct_item NULL means nothing has been defined from prop1 through propn
			// struct_item not NULL but if it doesn't have strut means prop(k+1) is defined without indexing 
			// pstruct->alt->type == N_STRUCT means .prop(k+1) has a decendent node that was not defined--> no need to check
			// (pstruct->alt->type == T_ID or N_TIME_EXTRACT) means .prop(k+1) has a decendent node accessed with indexing--need to check
			if (!struct_item || !pstruct->alt || pstruct->alt->type == N_STRUCT) {
				typelhs = TYPEBIT_NULL;
				return;
			}
			pvarLHS = struct_item;
		}
		typelhs = pvarLHS->type();
		if ((typelhs & TYPEBIT_CELL)) {
			// check if ind is valid in x{ind} -- todo 9/5/2022
			if (plhs->alt->type == N_ARGS)
				throw exception_etc(*this, plhs, "Items in the cell array on LHS should be accessed with {}").raise();
			lhs_index.SetValue(plhs->alt->dval);
			return;
		}
		// check type.. mask with 0xFFF4 -- to mask the last two bits zero (clean the length bits) to check the type only (no length)
		auto typerhs = RHS.type();
		if (!pstruct || pstruct->alt)
			if (typerhs > 0 && plhs->alt->type != N_STRUCT && (typelhs & (uint16_t)0xFFF4) != (typerhs & (uint16_t)0xFFF4) )
				throw exception_etc(*this, plhs, "LHS and RHS have different object type.").raise();
		if (plhs->alt->type == N_TIME_EXTRACT)
		{
			// if lhs var is not audio, throw
			if (!ISAUDIO(typelhs))	{
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
		// if pstruct is not NULL, .prop is being checked 
		if (pstruct) {
			if (pstruct->alt)
				plhs = pstruct;
			else
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
			throw exception_etc(*this, plhs, "LHS and RHS have different dimension (lengths).").raise();
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

void skope::assign_struct(CVar* lobj, const AstNode* plhs, const AstNode* pstruct, const CVar& robj)
{
	deque<string> strchain;
	deque<CVar> cvarchain;
	const AstNode* p = pstruct;
	for (auto q = pstruct->alt; q; q = q->alt)
	{
		strchain.push_back(q->str);
		cvarchain.push_back(CVar());
	}
	auto pvar = (CVar*)&robj;
	for (auto rit = cvarchain.end(); !strchain.empty(); strchain.pop_back())
	{
		rit--;
		auto str = strchain.back();
		(*rit).strut[str] = pvar;
		pvar = &(*rit);
	}
	if (!lobj)
		SetVar(plhs->str, pvar);
	else
		lobj->strut[pstruct->str] = pvar;
}
const CVar* skope::get_available_struct_item(const AstNode* plhs, const AstNode** pstruct)
{
	const CVar* pvarLHS = NULL;
	*pstruct = plhs;
	auto it = Vars.find(plhs->str);
	if (it != Vars.end()) {
		map<std::string, CVar>::iterator itvar;
		for (pvarLHS = &(it->second); plhs->alt && plhs->alt->type == N_STRUCT; plhs = plhs->alt) {
			*pstruct = plhs->alt;
			itvar = ((CVar*)pvarLHS)->strut.find(plhs->alt->str);
			if (itvar == pvarLHS->strut.end()) {
				break;
			}
			if (plhs->alt->alt)
				pvarLHS = &(itvar->second);
		}
	}
	return pvarLHS;
}

void skope::assign_adjust(const AstNode* pn, CVar* lobj, const CVar& lhs_index, CVar& robj, bool contig)
{
	ostringstream out;
	if (robj.nSamples == 0)
	{ // truncate the LHS var buffer
		if (contig)
		{
			memmove(lobj->buf + (uint64_t)lhs_index.buf[0] - 1, lobj->buf + (uint64_t)lhs_index.buf[lhs_index.nSamples - 1], lhs_index.nSamples * sizeof(float));
			lobj->nSamples -= lhs_index.nSamples;
		}
		else
		{
			throw exception_etc(*this, pn, "a(range)=[] -- range should be consecutive.").raise();
		}
	}
	else if (robj.nSamples == 1)
	{ // fill the buffer with the RHS value 
		for (uint64_t k = 0; k < lhs_index.nSamples; k++)
			lobj->buf[(uint64_t)lhs_index.buf[k] - 1] = robj.buf[0];
	}
	else if (lhs_index.nSamples == 1)
	{
		auto nCopied = lobj->nSamples + 1 - (uint16_t)lhs_index.buf[0];
		lobj->UpdateBuffer(lobj->nSamples + robj.nSamples);
		float* pv = lobj->buf;
		auto j = (uint64_t)lhs_index.buf[0] - 1;
		memmove(&pv[j + robj.nSamples], &pv[j], nCopied * sizeof(float));
		auto val = robj.buf[0];
		auto pval = robj.buf;
		memcpy(&pv[j], pval, robj.nSamples * sizeof(float));
	}
	else if (lhs_index.nSamples == robj.nSamples) {
		if (contig)
			memmove(lobj->logbuf + lobj->bufBlockSize * ((uint64_t)lhs_index.buf[0] - 1), robj.buf, lobj->bufBlockSize * robj.nSamples);
		else
			for (uint64_t k = 0; k < robj.nSamples; k++)
				lobj->buf[(uint64_t)lhs_index.buf[k] - 1] = robj.buf[k];
	}
	else
		throw exception_etc(*this, pn, "Unexpected case").raise();
}

void skope::right_to_left(const AstNode* plhs, const CVar& lhs_index, CVar& robj, uint16_t typelhs, bool contig, const AstNode* prhs, CVar* lobj)
{
	bool isreplica = prhs != NULL;
	ostringstream out;
	if (typelhs == TYPEBIT_NULL) {
		if (lobj) {
			*lobj = robj;
		}
		else if (plhs->alt && plhs->alt->type == N_STRUCT) {
			const AstNode* pstruct;
			const CVar* struct_item = get_available_struct_item(plhs, &pstruct);
			if (isreplica) { //RL-NS
				replica = GetVariable(pstruct->str, (CVar*)struct_item);
				robj = Compute(prhs);
			}
			// p.prop1.prop2.prop3=[3 5 2] 
			assign_struct((CVar*)struct_item, plhs, pstruct, robj);
		}
		else {
			if (isreplica) { //RL-N
				if (!plhs->alt) {
					replica = &Vars.find(plhs->str)->second;
					Compute(prhs);
				}
			}
			SetVar(plhs->str, &Sig);
		}
	}
	else if (plhs->type == N_VECTOR) {
		if (lhs) { // NULL lhs means outputbinding was done at PrepareAndCallUDF
			outputbinding(plhs);
			SigExt.clear();
			lhs = nullptr;
		}
	}
	else
	{
		if (!lobj)
			lobj = &Vars.find(plhs->str)->second;
		if (typelhs & TYPEBIT_CELL) {
			const CVar* cellitem = get_cell_item(plhs, *lobj);
			CVar index;
			if (!plhs->alt) {
				right_to_left(plhs->alt, index, robj, typelhs, contig, prhs, (CVar*)cellitem);
				return;
			}
			if (plhs->alt->alt) {
				eval_lhs(plhs->alt, NULL, index, (CVar&)robj, typelhs, contig, isreplica, cellitem); // robj is not updated, just provided only as a reference
				right_to_left(plhs->alt, index, robj, typelhs, contig, prhs, (CVar*)cellitem);
				return;
			}
			if (isreplica) { //RL-C
				replica = *cellitem;
				robj = Compute(prhs);
			}
			typelhs = TYPEBIT_NULL;
			right_to_left(plhs->alt, index, robj, typelhs, contig, prhs, (CVar*)cellitem);
			return;
		}

		if (lobj && plhs->alt->type == N_TIME_EXTRACT)
		{
			uint64_t bufshift = 0;
			if (isreplica) { //RL-T
				replica = *lobj;
				replica.Crop(lhs_index.buf[0], lhs_index.buf[1]);
				robj = Compute(prhs);
				replica.Reset();
			}
			insertreplace(plhs, robj, lhs_index, lobj, isreplica);
		}
		else
		{
			const AstNode* pn = plhs->alt;
			if (plhs->alt->type == N_STRUCT) { // RL-S
				// go til the tip of prop item and call assign_adjust
				const AstNode* pstruct;
				const CVar* struct_item = get_available_struct_item(plhs, &pstruct);
				pn = pstruct;
				if (struct_item) lobj = (CVar*)struct_item;
				if (pstruct->alt->type != N_ARGS) {
					assign_struct((CVar*)struct_item, plhs, pstruct, robj);
					return;
				}
			}
			if (isreplica) { //RL-X
				replica.UpdateBuffer(lhs_index.nSamples);
				if (contig)
					memmove(replica.logbuf, lobj->logbuf + (size_t)(lobj->bufBlockSize * (lhs_index.buf[0] - 1)), lobj->bufBlockSize * lhs_index.nSamples);
				else
					for (uint64_t k = 0; k < lhs_index.nSamples; k++)
						replica.buf[k] = lobj->buf[(uint64_t)lhs_index.buf[k] - 1];
				robj = Compute(prhs);
			} 
			assign_adjust(pn, lobj, lhs_index, robj, contig);
		}
	}
}

CVar* skope::process_block(const AstNode* pnode)
{
	CVar* res = NULL;
	return res;
}
void skope::sanitize_cell_node(const AstNode* p)
{ // evaluate cell indexing in child into dval
	if (p->alt && p->alt->type == N_CELL)
	{
		Compute(p->alt->child);
		if (Sig.type() != 1)
			throw exception_etc(*this, p, "Invalid cell index").raise();
		p->alt->dval = Sig.value();
		yydeleteAstNode(p->alt->child, 0);
		p->alt->child = NULL;
	}
}

CVar* skope::process_statement(const AstNode* pnode)
{
	const AstNode* plhs;
	const AstNode* prhs;
	bool isreplica = get_nodes_left_right_sides(pnode, &plhs, &prhs);
	CVar RHS;
	if (!isreplica && (!plhs || plhs->type != N_VECTOR))
		RHS = Compute(prhs); // if (plhs->type == N_VECTOR) in eval_lhs()
	if (plhs)
	{
		uint16_t typelhs;
		bool contig;
		CVar index;
		sanitize_cell_node(plhs);
		eval_lhs(plhs, prhs, index, RHS, typelhs, contig, isreplica);
		right_to_left(plhs, index, RHS, typelhs, contig, isreplica ? prhs:NULL);
	}
	return &Sig;
}

void skope::insertreplace(const AstNode* plhs, const CVar& robj, const CVar& indsig, CVar *lobj, bool isreplica)
{
	const AstNode* p = plhs;
	if ((p->alt && p->alt->type == N_TIME_EXTRACT) || // x{id}(t1~t2) = ...sqrt
		p->type == N_TIME_EXTRACT || (p->next && p->next->type == N_IDLIST))  // s(repl_RHS1~repl_RHS2)   or  cel{n}(repl_RHS1~repl_RHS2)
	{
		if (isreplica) // direct update of buf
		{
			// find out where robj begins and ends (index and tmark for the chain of interest)
			auto begin = lobj->FindChainAndID(indsig.buf[0], true);
			auto end = lobj->FindChainAndID(indsig.buf[1], false);
			if (begin.first==NULL) 
				throw exception_etc(*this, p, "Time indexing out of range").raise();
			int len;
			CTimeSeries* q = (CTimeSeries*)&robj;
			int offset = begin.second;
			for (auto p = begin.first; p; p = p->chain, q = q->chain) {
				if (begin.first == end.first)
					len = end.second - begin.second + 1;
				else
					len = q->nSamples;
				memcpy(p->logbuf + offset * p->bufBlockSize, q->buf, len * q->bufBlockSize);
				offset = 0;
			}
			// TO DO: stereo case 10/13/2022
		}
		else
			lobj->ReplaceBetweenTPs(robj, indsig.buf[0], indsig.buf[1]);
	}
}