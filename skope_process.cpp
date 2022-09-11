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
		RHS = Compute(prhs);
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
			RHS = Compute(prhs);
			// if var is not defined, type is null
			auto itvar = Vars.find(plhs->str);
			if (itvar == Vars.end())
			{ // if left var is not defined
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
		// Todo---make a list of commands that you expect to work and to issue an error
		// work on each... 9/11/2022
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
			if (!struct_item || struct_item->strut.empty()) {
				typelhs = TYPEBIT_NULL;
				return;
			}
			pvarLHS = struct_item;
		}
		typelhs = pvarLHS->type();
		if ((typelhs & TYPEBIT_CELL)) {
			// check if ind is valid in x{ind} -- todo 9/5/2022
			size_t cellind;
			if (plhs->alt->type == N_ARGS) {
				out << "Items in the cell array on LHS should be accessed with {}";
				throw exception_etc(*this, plhs, out.str()).raise();
			}
			cellind = (size_t)(int)Compute(plhs->alt->child)->value(); // check the validity of ind...probably it will be longer than this.
				lhs_index.SetValue(cellind);
				return;
		}
		// check type.. mask with 0xFFF4 -- to mask the last two bits zero (clean the length bits) to check the type only (no length)
		auto typerhs = RHS.type();
		if (!pstruct || pstruct->alt)
			if (typerhs > 0 && plhs->alt->type != N_STRUCT && (typelhs & (uint16_t)0xFFF4) != (typerhs & (uint16_t)0xFFF4) )
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

// vv.prop1.prop2...prop5 = RHS
// --> define or update (if it already exists) .prop1, .prop2, ... to .prop5
// lobj points to the available object. 
// If vv is not available, lobj is NULL
// If prop1 is available and prop2 is not; lobj points to prop1; plhs->str is prop1; pstruct->str is prop2
void skope::assign_struct(CVar* lobj, const AstNode* plhs, const AstNode* pstruct, const CVar& robj)
{
	if (!lobj) {
		CVar newobj = CVar();
		newobj.strut[pstruct->str] = robj;
		Vars[plhs->str] = newobj;
	}
	else
		SetVar(pstruct->str, (CVar *)&robj, lobj);
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

//{ // vv.prop = RHS
//	// case 1: vv is not present
//	// case 2: vv is present, but not as a struct obj
//	// case 3: vv is present as a struct obj, but no .prop is present
//	// case 4: vv is present as a struct obj, and .prop has been defined
//	// cases 1 and 2: typelhs is TYPEBIT_NULL, lobj is N/A; cases 3 and 4 it is not. lobj is available, but won't need it
//	auto itvar = Vars.find(plhs->str);
//	if (itvar == Vars.end()) {
//		// case 1
//		// define vv as an object with struct, item .prop with RHS
//		CVar newobj = new CVar();
//		CVar* pvar = (CVar*)&robj;
//		newobj.strut[pstruct->str] = robj;
//		Vars[plhs->str] = newobj;
//	}
//	else {
//		CVar * pvarLHS = &(itvar->second);
//		if (pvarLHS->strut.empty()) {
//			// case 2
//			// update vv (the same action as case 1)
//			CVar newobj = new CVar();
//			CVar* pvar = (CVar*)&robj;
//			newobj.strut[pstruct->str] = robj;
//			Vars[plhs->str] = newobj;
//		}
//		else {
//			auto itvar2 = pvarLHS->strut.find(pstruct->str);
//			if (itvar2 == pvarLHS->strut.end()) {
//				// case 3
//				// define .prop with RHS (don't define vv as a whole)
//				CVar* leftobj = &Vars[plhs->str];
//				CVar* pvar = (CVar*)&robj;
//				leftobj->strut[pstruct->str] = robj;
//			}
//			else {
//				// case 4
//				// update .prop with RHS (the same actin as case 3)
//				CVar* leftobj = &Vars[plhs->str];
//				CVar* pvar = (CVar*)&robj;
//				leftobj->strut[pstruct->str] = robj;
//			}
//		}
//	}
//}

void skope::assign_adjust(const AstNode* pn, CVar* lobj, const CVar& lhs_index, const CVar& robj, bool contig)
{
	ostringstream out;
	if (pn->type == N_TIME_EXTRACT)
	{
		insertreplace(pn, robj, lhs_index, lobj);
	}
	else
	{


		//if (pn->alt->type == N_STRUCT) {
		//	assign_struct(pn, )
		//		// do this  tomorrow 9/7/2022.... a.prop = RHS
		//		SetVar(pn->str, (CVar*)&robj); //???
		//}
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
					throw exception_etc(*this, pn, out.str()).raise();
				}
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
			pv = lobj->buf;
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
		{
			out << "no way...";
			throw exception_etc(*this, pn, out.str()).raise();
		}
	}
}

void skope::right_to_left(const AstNode* plhs, const CVar& lhs_index, const CVar& robj, uint16_t typelhs, bool contig, CVar* cellitem)
{
	ostringstream out;
	if (typelhs == TYPEBIT_NULL) {
		if (cellitem) {
			*cellitem = robj;
		}
		else if (plhs->alt && plhs->alt->type == N_STRUCT) {
			const AstNode* pstruct;
			const CVar* struct_item = get_available_struct_item(plhs, &pstruct);
			// p.prop1.prop2.prop3=[3 5 2] --> not working 9/11/2022
			assign_struct((CVar*)struct_item, plhs, plhs->alt, robj);
		}
		else
			SetVar(plhs->str, &Sig);
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

		if (cellitem && plhs->alt->type == N_TIME_EXTRACT)
		{
			insertreplace(plhs, robj, lhs_index, cellitem);
		}
		else
		{
			if (plhs->alt->type == N_STRUCT) {
				// go til the tip of prop item and call assign_adjust
				const AstNode* pstruct;
				const CVar* struct_item = get_available_struct_item(plhs, &pstruct);
				assign_struct((CVar*)struct_item, plhs, pstruct, robj);
				return;
			}
			assign_adjust(plhs->alt, lobj, lhs_index, robj, contig);
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
	const AstNode* p = plhs;
	//if (!lobj)
	//	lobj = &Vars.find(plhs->str)->second;
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