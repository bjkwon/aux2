#include <algorithm>
#include "skope.h"
#include "skope_exception.h"
#include "typecheck.h"
#include <assert.h>

#ifndef _WINDOWS
#include <libgen.h>

#endif


#define MAX_PATH 256

skope::skope(string instr)
{
	node = NULL;
	done = false;
	script = instr;
	nodeAllocated = false;
}
skope::~skope()
{
	//clean up node

}

CAstSigEnv::CAstSigEnv(const int fs)
	: Fs(fs), curLine(-1)
{
	shutdown = false;
	if (fs < 0)	throw "Internal error: Fs must be greater than 1.";
}

CAstSigEnv::~CAstSigEnv()
{

}
/*
const AstNode* skope::findDadNode(const AstNode* p, const AstNode* pME)
{
	if (!p) return NULL;
	if (p == pME) return p;
	if (p->type == N_BLOCK)
	{
		p = p->next;
		const AstNode* res = findDadNode(p, pME);
		while (!res)
		{
			if (!p) break;
			p = p->next;
			res = findDadNode(p, pME);
		}
		return res;
	}
	if (!p->child && !p->alt) return NULL;
	if (p->child == pME) return p;
	const AstNode* res = NULL;
	do
	{
		if (p->child)
			res = findParentNode(p->child, res);
		else
			res = findParentNode(p->alt, res);
		if (!res) break;
	} while (res->type == N_STRUCT);
	if (res) return res;
	if (p->type == N_VECTOR)
	{
		auto pp = (const AstNode*)(p->str);
		if (!pp)
		{// p is "true" N_VECTOR -- where p->alt and the success on nexts are actual elements
			for (auto p2 = p->alt; p2; p2 = p2->next)
			{
				if (p2 == pME) return p;
			}
			return NULL;
		}
		else
		{
			if (pp == pME) return p;
			res = findDadNode(pp, pME);
			if (res) return res;
		}
	}
	return NULL;
}

const AstNode* skope::findParentNode(const AstNode* p, const AstNode* pME, bool altonly)
{
	if (p)
	{
		if (!p->child && !p->alt) return NULL;
		if (!altonly && p->child == pME) return p;
		if (p->alt == pME) return p;
		if (!altonly && p->child)
			return findParentNode(p->child, pME, altonly);
		else
			return findParentNode(p->alt, pME, altonly);
	}
	return NULL;
}
*/


void skope::outputbinding(const AstNode *pnode, size_t nArgout)
{
	auto count = 0;
	assert(lhs);
	// lhs must be T_ID, N_VECTOR, or N_ARGS
	AstNode *pp = lhs;
	if (lhs->type == N_VECTOR)
		pp = ((AstNode *)lhs->str)->alt;
	for (auto varname : son->u.argout)
	{
		if (son->Vars.find(varname) != son->Vars.end())
		{
			bind_psig(pp, &son->Vars[varname]);
			if (count++ == 0)
			{
				Sig = son->Vars[varname]; //why is this needed? -- so update go Sig with non-go Sig 2/9/2019
				pgo = NULL;
			}
		}
		else if (son->GOvars.find(varname) != son->GOvars.end())
		{
			//if (son->GOvars[varname].size() > 1) // need to make an CSIG_HDLARRAY object
			//	pgo = MakeGOContainer(son->GOvars[varname]);
			//else
				pgo = son->GOvars[varname].front();
			if (count++ == 0)
				Sig = *pgo; //Ghost output to console
			SetVar(pp->str, pgo);
		}
		if (--nArgout == 0) break;
		pp = pp->next;
		if (!pp) break;
	}
	lhs = nullptr;
}
inline void skope::throw_LHS_lvalue(const AstNode* pn, bool udf)
{
	ostringstream out;
	out << "LHS must be an l-value. ";
	if (udf)
	{
		out << "Name conflict between the LHS variable " << endl;
		out << "\"" << pn->str << "\" and a user-defined function" << endl;
//		out << pEnv->udf[pn->str].fullname;
	}
	else if (pn->type == N_MATRIX)
	{
		out << " (cannot have a matrix on the LHS). ";
	}
	else if (pn->type == N_VECTOR)
	{
		out << " (LHS can be a vector [...] only if RHS is a function call.) ";
	}
	else
	{
		out << pn->str << " is a built-in function.";
	}
	throw exception_misuse(*this, pn, out.str()).raise();
}

AstNode* skope::makenodes(const string& instr)
{
	int res;
	char* errmsg;
	if (instr.empty()) return node;
	if (nodeAllocated) {
		yydeleteAstNode(node, 0);
		nodeAllocated = false;
	}
	AstNode* out = NULL;
	reset_stack_ptr();
	if ((res = yysetNewStringToScan(instr.c_str(), NULL /*str_autocorrect*/)))
	{
		throw "[INTERNAL ERROR] yysetNewStringToScan() failed!";
	}
	res = yyparse(&out, &errmsg);
	nodeAllocated = out ? true : false;
	if (!errmsg && res == 2)
	{
		throw "[INTERNAL ERROR] Out of memory!";
		return NULL;
	}
	if (errmsg) {
		if (nodeAllocated) {
			yydeleteAstNode(out, 0);
			nodeAllocated = false;
			out = NULL;
		}
		emsg = errmsg;
		throw emsg.c_str();
	}
	script = instr;
	return out;
}

CVar* skope::Compute(const AstNode* pnode)
{
	CVar tsig, isig;
	if (!pnode) return &Sig;
	AstNode* p = pnode->child;
	switch (pnode->type) {
	case T_ID:
		return TID((AstNode*)pnode, p);
	case T_TRY:
		inTryCatch++;
		Try_here(pnode, p);
		break;
	case T_CATCH:
		inTryCatch--; //???? Maybe no longer needed?? 3/7/2020
		// p is T_ID for ME (exception message caught)
		// continue here............1/12/2020
		Compute(pnode->next);
		break;
	case N_HOOK:
		return TID((AstNode*)pnode, p);
	case N_TSEQ:
		return TSeq(pnode, p);
	case N_IDLIST:
		tsig = Compute(p);
		if (p && pnode->alt && !pnode->tail && !pnode->str)
		{    // LHS is x(tp1~tp2)
			Compute(pnode->alt);
			Sig += &tsig;
		}
	return &Sig;
	case T_NUMBER:
		Sig.SetValue(pnode->dval);
		return TID(pnode->alt, p, &Sig);
	case T_STRING:
		Sig.SetString(pnode->str);
		return TID(pnode->alt, p, &Sig);
	case N_MATRIX:
		if (p) throw_LHS_lvalue(pnode, false);
		NodeMatrix(pnode);
		return Dot(pnode->alt);
	case N_VECTOR:
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
		break;
	case T_REPLICA:
		return TID((AstNode*)pnode, NULL, &replica); //Make sure replica has been prepared prior to this
	case T_ENDPOINT:
		tsig.SetValue(endpoint);
		return TID((AstNode*)pnode, NULL, &tsig); //Make sure endpoint has been prepared prior to this
	case '+':
	case '-':
		if (pnode->type == '+')	tsig = Compute(p->next);
		else					tsig = -*Compute(p->next);
		blockCellStruct2(*this, pnode, Sig);
		blockString2(*this, pnode, Sig);
		Compute(p);
		blockCellStruct2(*this, pnode, Sig);
		blockString2(*this, pnode, Sig);
		if (Sig.type() == 0) Sig.SetValue(0.);
		if (tsig.type() == 0) tsig.SetValue(0.);
		Sig += tsig;
		return TID((AstNode*)pnode->alt, NULL, &Sig);
	case '*':
	case '/':
	case T_MATRIXMULT: // "**"
		tsig = Compute(p);
		blockCellStruct2(*this, pnode, Sig);
		blockString2(*this, pnode, Sig);
		if (pnode->type == '*' || pnode->type == '/')	Compute(p->next);
		else
		{
			ensureVector1(*this, pnode, tsig);
			Compute(p->next);
			ensureVector1(*this, pnode, Sig);
			Sig = (CSignals)tsig.matrixmult(&Sig);
			return TID((AstNode*)pnode, NULL, &Sig);
		}
		blockCellStruct2(*this, pnode, Sig);
		blockString2(*this, pnode, Sig);
		if (Sig.type() == 0) Sig.SetValue(0.f);
		if (tsig.type() == 0) tsig.SetValue(0.f);
		// reciprocal should be after blocking string (or it would corrupt the heap) 6/3/2020
		if (pnode->type == '/') Sig.reciprocal();
		Sig *= tsig;
		return TID((AstNode*)pnode->alt, NULL, &Sig);
	case '%':
		//only in the format of A %= B
		((AstNode*)pnode)->type = N_CALL;
		((AstNode*)pnode)->str = strdup("mod");
		((AstNode*)pnode)->alt = p->next;
		p->next = NULL;
		Sig = replica;
		HandleAuxFunctions(pnode); // Assuming that current body content (Sig) is already prepared...is it true? 8/23/2018
		break;
	case T_TRANSPOSE:
		Transpose(pnode, p);
		return TID((AstNode*)pnode->alt, NULL, &Sig);
		break;
	case T_NEGATIVE:
		-*Compute(p);
		blockString2(*this, pnode, Sig);
		return TID((AstNode*)pnode->alt, NULL, &Sig);
	case T_OP_SHIFT:
		tsig = Compute(p->next);
		blockCellStruct2(*this, pnode, Sig);
		ensureScalar1(*this, pnode, Sig, ">>");
		Compute(p);
		ensureAudio1(*this, pnode, Sig, ">>");
		Sig >>= tsig.value();
		return TID((AstNode*)pnode, NULL, &Sig);
		break;
	case T_OP_CONCAT:
		Concatenate(pnode, p);
		return TID((AstNode*)pnode->alt, NULL, &Sig);
		break;
	case T_LOGIC_OR:
	case T_LOGIC_AND:
	case '<':
	case '>':
	case T_LOGIC_LE:
	case T_LOGIC_GE:
	case T_LOGIC_EQ:
	case T_LOGIC_NE:
	case T_LOGIC_NOT:
		ConditionalOperation(pnode, p);
		return TID((AstNode*)pnode, NULL, &Sig);
	case '@':
		SetLevel(pnode, p);
		return TID((AstNode*)pnode->alt, NULL, &Sig);
	case N_INITCELL:
		return InitCell(pnode, p);
	case N_BLOCK:
		for (p = pnode->next; p && !fExit && !fBreak; p = p->next)
		{
			pLast = p;
//				hold_at_break_point(p);
			Compute(p);
			//			pgo = NULL; // without this, go lingers on the next line 2/9/2019
			Sig.Reset(1); // without this, fs=3 lingers on the next line 2/9/2019
		}
		break;
	case T_IF:
		if (!p) break;
		pLast = p;
		if (checkcond(p))
			Compute(p->next);
		else if (pnode->alt)
			Compute(pnode->alt);
		break;
	case T_SWITCH:
	{
		switch_case_handler(pnode);
		tsig = Compute(p);
		auto fd = find(pEnv->udf[u.base].switch_case_undefined.begin(), pEnv->udf[u.base].switch_case_undefined.end(), p->line);
		if (fd != pEnv->udf[u.base].switch_case_undefined.end())
		{ // update switch_case
			CVar tsig2 = Compute(p->alt);
			pEnv->udf[u.base].switch_case.emplace(tsig2, p->next);
		}
		if (pEnv->udf[u.base].switch_case.find(tsig) == pEnv->udf[u.base].switch_case.end())
		{
			Compute(pnode->tail->next);
		}
		else
		{
			auto ret = pEnv->udf[u.base].switch_case.equal_range(tsig);
			auto it = ret.first;
			for (; it != ret.second; it++)
			{
				int endline = (*it).second->line + 1;
				if (pnode->next)
					endline = pnode->next->line;
				if ((*it).second->line > pnode->line && (*it).second->line < endline)
				{
					Compute((*it).second);
					break;
				}
			}
			if (it == ret.second)
				Compute(pnode->tail->next);
		}
	}
	break;
	case T_WHILE:
		fExit = fBreak = false;
		while (checkcond(p) && !fExit && !fBreak)
			Compute(pnode->alt);
		fBreak = false;
		break;
	case T_FOR:
		fExit = fBreak = false;
		isig = Compute(p->child);
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
		break;
	case T_BREAK:
		fBreak = true;
		break;
	case T_RETURN:
		fExit = true;
		break;
	default:
	{
		ostringstream oss;
		oss << "TYPE=" << pnode->type << "Unknown node type";
		throw exception_etc(*this, pnode, oss.str()).raise();
	}
	}
	return &Sig;
}

CVar* skope::TSeq(const AstNode* pnode, AstNode* p)
{
	//For now (6/12/2018) only [vector1][vector2] where two vectors have the same length.
	CVar tsig2, tsig = Compute(p);
	auto type1 = tsig.type();
	if (type1 > 2)
	{
		strcpy(pnode->str, "TSEQ"); // what is this? 12/24/2021
		throw exception_etc(*this, pnode, "Invalid tmark array").raise();
	}
	if (pnode->child->next) {
		//[tvalues][val] pnode->child->next is N_VECTOR for val
		//[tvalues][val;] pnode->child->next is N_MATRIX for val
		tsig2 = Compute(pnode->child->next);
		ensureVector3(*this, pnode, tsig2, "vector expected for tseq");
		if (tsig2.nGroups == 1)
		{
			//if
			if (pnode->child->next->type == N_VECTOR)
			{
				if (tsig2.nSamples != tsig.nSamples)
				{
					strcpy(pnode->str, "TSEQ");
					throw exception_etc(*this, pnode, "Time-point and value arrays must have the same length.").raise();
				}
			}
			else //pnode->child->next->type == N_MATRIX
			{
				//ok
			}
		}
		else
		{
			if (tsig2.nGroups != tsig.nSamples)
			{
				strcpy(pnode->str, "TSEQ");
				throw exception_etc(*this, pnode, "Time-point and value arrays must have the same length.").raise();
			}
		}
	}
	Sig.Reset(tsig.GetFs());
	CTimeSeries tp, * run(&Sig);
	if (pnode->str && pnode->str[0] == 'R') //Relative tmarks
	{
		tp.SetFs(0); // new fs is 0.
		Sig.SetFs(0);
	}
	else // if it is not relative, fs is set from the current system default.
		tp.SetFs(GetFs());
	for (unsigned int k = 0; k < tsig.nSamples; k++)
	{
		tp.tmark = tsig.buf[k];
		if (tsig2.nSamples > 0)
		{
			if (tsig2.nGroups == 1 && pnode->child->next->type == N_VECTOR)
				tp.SetValue(tsig2.buf[k]);
			else
			{
				unsigned int len = tsig2.Len();
				tp.UpdateBuffer(len);
				memcpy(tp.buf, tsig2.buf + len * k, len * sizeof(float));
			}
		}
		if (k == 0) *run = tp;
		else
		{
			run->chain = new CTimeSeries;
			run = run->chain;
			*run = tp;
		}
	}
	Sig.setsnap();
	return &Sig;
}

static AstNode* get_next_parsible_node(AstNode* pn)
{
	if (pn->alt && pn->alt->type == N_CELL) pn = pn->alt;
	return pn->alt;
}

static bool IsConditional(const AstNode* pnode)
{
	if (!pnode) return false;
	switch (pnode->type) {
	case '<':
	case '>':
	case T_LOGIC_EQ:
	case T_LOGIC_NE:
	case T_LOGIC_LE:
	case T_LOGIC_GE:
	case T_LOGIC_NOT:
	case T_LOGIC_AND:
	case T_LOGIC_OR:
		return true;
	default:
		return false;
	}
}

AstNode* skope::searchtree(const AstNode* pTarget, AstNode* pStart)
{ // search pTarget in all lower nodes (child, alt, next) in pStart
	// return NULL if not found
	if (pStart->line != pTarget->line) return NULL;
	AstNode* p = pStart;
	while (p)
	{
		if (p == pTarget)  return u.pLastRead = p;
		if (p->alt) if (searchtree(pTarget, p->alt)) { u.pLastRead = pStart;  return p->alt; }
		if (p->child) if (searchtree(pTarget, p->child)) { u.pLastRead = pStart;  return p->child; }
		if (p->next) if (searchtree(pTarget, p->next)) { u.pLastRead = pStart;  return p->next; }
		return NULL;
	}
	return NULL;
}
AstNode* skope::searchtree(AstNode* p, int type)
{ // if there's a node with "type" in the tree, return that node
	if (p)
	{
		if (p->type == N_VECTOR || p->type == N_MATRIX) // for [  ], search must continue through p->str
			return searchtree(((AstNode*)p->str)->alt, type);
		if (p->type == type) return p;
		if (p->child)
		{
			if (p->child->type == type) return p->child;
			else
			{
				if (searchtree(p->child, type)) return p->child;
				if (searchtree(p->alt, type)) return p->alt;
			}
		}
		if (p->alt)
		{
			if (p->alt->type == type) return p->alt;
			else
			{
				if (searchtree(p->alt, type)) return p->alt;
				if (searchtree(p->child, type)) return p->child;
			}
		}
		if (p->next)
		{
			if (p->next->type == type) return p->next;
			else return searchtree(p->next, type);
		}
	}
	return NULL;
}
AstNode* skope::read_node(CNodeProbe& np, AstNode* ptree, AstNode* ppar, bool& RHSpresent)
{
	if (ptree->type == T_OP_CONCAT || ptree->type == '+' || ptree->type == '-' || ptree->type == T_TRANSPOSE || ptree->type == T_MATRIXMULT
		|| ptree->type == '*' || ptree->type == '/' || ptree->type == T_OP_SHIFT || ptree->type == T_NEGATIVE || (ptree == np.root && IsConditional(ptree)))
	{ //No further actions
		return get_next_parsible_node(ptree);
	}
	string emsg;
	AstNode* p = ptree;
	int ind(0);
	CVar* pres;
	ostringstream out;
	if (builtin_func_call(np, ptree))
	{
		// In a top-level assignment e.g., a = 1; RHSpresent is not yet set, ptree->child should be checked instead
		if (RHSpresent || ptree->child)	throw_LHS_lvalue(ptree, false);
		np.psigBase = &Sig;
		// if a function call follows N_ARGS, skip it for next_parsible_node
		if (ptree->alt)
		{
			if (ptree->alt->type == N_ARGS || ptree->alt->type == N_HOOK)
				ptree = ptree->alt; // to skip N_ARGS
		}
	}
	else if (ptree->type == N_ARGS)
	{
		// ptree points to LHS, no need for further processing. Skip
		// (except that RHS involves T_REPLICA (.. or a compoud op)
		if (!RHSpresent || searchtree(np.root->child, T_REPLICA) || ptree->alt)
			np.tree_NARGS(ptree, ppar);
		else
			return NULL;
	}
	else if (ptree->type == N_TIME_EXTRACT)
		np.TimeExtract(ptree, ptree->child);
	else if (ptree->type == T_REPLICA || ptree->type == T_ENDPOINT)
	{
		if (ptree->alt && ptree->alt->type == N_CELL)
			np.cell_indexing(np.psigBase, ptree);
		else
			Sig = *np.psigBase;
	}
	else
	{
		// If RHS exists (i.e., child is non-null), no need to get the LHS variable completely,
		// i.e., a.prop_layer_1.prop_layer_2 = (something else)
		// you don't need to getvariable at the level of prop_layer_2
		// (you still need to go down to the level prop_layer_1, though)
		//when ptree is conditional, there's no RHS, i.e., ptree->child doesn't mean RHS; just skip
		if (!IsConditional(ptree))
			RHSpresent |= ptree->child != nullptr;
		// With RHS, if ptree->alt is null, no need to GetVariable; just return (no update of Sig or pgo)
		if (RHSpresent && !searchtree(np.root->child, T_REPLICA))
		{
			if (ptree->type == N_VECTOR && ptree->alt) throw_LHS_lvalue(ptree, false);
			if (ptree->alt == nullptr) return nullptr; // a.prop.more = (something) --> when ptree points to more, read_node() returns null (done with LHS) and proceed to RHS
		}
		if (IsConditional(ptree))
		{
			if (np.psigBase->type() & TYPEBIT_CELL)
				throw exception_misuse(*this, ppar, string("A cell array ") + string(ppar->str) + " cannot be accessed with().").raise();
			CVar isig, isig2;
			np.eval_indexing(ptree, isig);
			pres = np.extract(ptree, isig);
			if (!ptree->next) // 1D indexing, unGroup it
				Sig.nGroups = 1;
		}
		else
		{
			//Need to scan the whole statement whether this is a pgo statement requiring an update of GO
			if (!np.varname.empty()) np.varname += '.';
			np.varname += ptree->str;
			if (!(pres = GetVariable(ptree->str, np.psigBase)))
			{
				AstNode* t_func;
				if ((t_func = ReadUDF(emsg, ptree->str)))
				{
					if (ptree->child || RHSpresent)	throw_LHS_lvalue(ptree, true);
					// if static function, np.psigBase must be NULL
					if (t_func->suppress == 3 && np.psigBase)
						throw exception_misuse(*this, t_func, string(ptree->str) + "() : function declared as static cannot be called as a member function.").raise();
					if (PrepareAndCallUDF(ptree, np.psigBase)) // this probably won't return false
					{// if a function call follows N_ARGS, skip it for next_parsible_node
						np.psigBase = &Sig;
						if (ptree->alt && (ptree->alt->type == N_ARGS || IsConditional(ptree->alt)))
							ptree = ptree->alt; // to skip N_ARGS
					}
					return get_next_parsible_node(ptree);
				}
				else
				{
					if (RHSpresent)
					{ // a new variable or struct member being defined
					  // need to update varname here
						for (auto q = ptree->alt; q; q = q->alt)
						{
							np.varname += '.';
							np.varname += q->str;
						}
						return nullptr;
					}
					if (emsg.empty())
					{
						string varname;
						string ex_msg = "Variable or function not available: ";
						if (ptree->type == N_STRUCT) varname = '.';
						varname += ptree->str;
						if (strlen(ptree->str) < 256)
							throw exception_etc(*this, ptree, ex_msg.c_str() + varname).raise();
						else
							throw exception_etc(*this, ptree, ex_msg.c_str() + varname + string("--A UDF name cannot be longer than 255 characters.")).raise();
					}
					else
						throw exception_etc(*this, ptree, emsg.c_str()).raise();
				}
			}
			// Need a case where both cell and strut are used?
			// Right now it's not prohibited, but not properly processed either.
			// Only cell is processed even if a strut has been defined 1/31/2021
			if (pres->IsGO()) // the variable ptree->str is a GO
				Sig = *(np.psigBase = pgo = pres);
		}
		if (searchtree(np.root->child, T_REPLICA))
		{
//			replica_prep(pres);
			// Updating replica with pres, the variable reading at current node, is necessary whenever replica is present
			// But, if current node is final and type is one of these, don't update np.psigBase
			if (!ptree->alt && (ptree->type == N_STRUCT || ptree->type == N_ARGS || ptree->type == N_TIME_EXTRACT))
				return get_next_parsible_node(ptree);
		}
		if (IsConditional(ptree)) return get_next_parsible_node(ptree);
		if (ptree->alt && ptree->alt->type == N_CELL)
			//either cellvar{2} or cellvar{2}(3). cellvar or cellvar(2) doesn't come here.
			// i.e., ptree->alt->child should be non-NULL
		{
			np.cell_indexing(pres, ptree);
		}
		else
		{
			Sig = *(np.psigBase = pres);
		}
	}
	return get_next_parsible_node(ptree);
}


AstNode* skope::read_nodes(CNodeProbe& np, bool bRHS)
{
	AstNode* pn = np.root;
	AstNode* p, * pPrev = NULL;
	CVar pvar;
	bool RHSpresent = bRHS;
	while (pn)
	{
		// Sig gets the info on the last node after this call.
		// when np.root->child is not NULL,
		// if pn->alt is terminal (not null), it doesn't have to go thru getvariable.
		p = read_node(np, pn, pPrev, RHSpresent);
		if (!p) return pn;
		if (p->type == N_ARGS)
			pPrev = pn;
		else
			pPrev = NULL;
		pn = p;
	}
	return NULL; // shouldn't come thru here; only for the formality
}

/* skope::bind_psig and CNodeProbe::TID_RHS2LHS perform a similar task.
The difference:
	bind_psig binds *psig (already computed) to an existing variable (no new creation of a variable)
	TID_RHS2LHS computes RHS and binds it to a variable, existing or new.
	11/6/2019
*/

void skope::bind_psig(AstNode* pn, CVar* psig)
{ // update psig with newsig
	// if pn is T_ID without alt-->SetVar
	// if pn is N_AGRS or N_STRUCT --> update psig with newsig according to the indices or dot
	assert(pn->type == T_ID);
	if (!pn->alt)
	{
		SetVar(pn->str, psig);
	}
	else
	{
		if (pn->alt->type == N_ARGS)
		{
			CNodeProbe ndprob(this, pn, NULL);
			// ndprob.psigBase should be prepared to do indexing
			AstNode* lhs_now = read_nodes(ndprob, true);
			ndprob.TID_indexing(pn, pn->alt, NULL);
		}
		else if (pn->alt->type == N_TIME_EXTRACT)
		{
		}
		else
		{
			assert(pn->alt->type == N_STRUCT);
			CVar* pbasesig = GetVariable(pn->str);
			SetVar(pn->alt->str, psig, pbasesig);
		}
	}

}

CVar* skope::TID(AstNode* pnode, AstNode* pRHS, CVar* psig)
{
	CNodeProbe np(this, pnode, psig); // psig is NULL except for T_REPLICA
	if (pnode)
	{
		char ebuf[256] = {};
		AstNode* pLast = read_nodes(np); // that's all about LHS.
		AstNode* lhsCopy = nullptr;
		if (pRHS)
			lhsCopy = lhs = pLast;
		else
		{
			if (np.psigBase && np.psigBase->IsGO())
				return np.psigBase;
			else	return &Sig;
		}
		CVar* pres;
		//if (!np.psigBase)
		//	Script = np.varname.empty() ? pnode->str : np.varname;
		pres = np.TID_RHS2LHS(pnode, pLast, pRHS);
//		replica.Reset();
		lhs = lhsCopy;
		//if (np.psigBase)
		//	Script = np.varname;
		// At this point, Sig should be it
		// psig : the base content of Sig
		// pLast: the node corresponding to psig

		//The rest is about SetGoProperties, f.pos(2) = 200, or the other pgo stuff
	}
	return &Sig;
}
static CVar* GetGOVariable(const skope& ths, const char* varname, CVar* pvar)
{ // To retrieve a GO variable.
  // For a single element, returns its pointer
  // For a array GO, create a container showing the pointers of the elements and return its pointer
	try {
		CVar* pout(NULL);
		vector<CVar*> GOs;
		if (pvar)
			GOs = pvar->struts.at(varname);
		else
			GOs = ths.GOvars.at(varname);
		// If the retrieved GOs is a size of 1, return the front element pointer
		// OK to return it even if the retrieved GOs is a GO container
		if (GOs.size() == 1)
			return GOs.front();
		//return the newly created container for multiple GOs
//		return MakeGOContainer(GOs);
		return NULL;
	}
	catch (out_of_range oor)
	{
		throw exception_etc(ths, ths.node, "GetGOVariable() should be called when varname is sure to exist in GOvars").raise();
	}
}CVar* skope::GetVariable(const char* varname, CVar* pvar)
{ //To retrive a variable from a workspace pvar is NULL (default)
  //To retrive a member variable, specify pvar as the base variable
 // For multiple GO's, calls GetGOVariable()
	string fullvarname = "";
	CVar* pout(NULL);
	if (!varname)
		throw exception_etc(*this, node, "GetVariable(): NULL varname").raise();
	if (pvar)
	{
		if (pvar->strut.find(varname) != pvar->strut.end())
			pout = &pvar->strut.at(varname);
		else if (pvar->struts.find(varname) == pvar->struts.end())
			return NULL;
		fullvarname += '.';
		fullvarname += varname;
	}
	else
	{
		if (Vars.find(varname) != Vars.end())
			pout = &Vars.at(varname);
		else if (GOvars.find(varname) == GOvars.end())
			return NULL;
		fullvarname = varname;
	}
	if (pout) return pout;
	return GetGOVariable(*this, varname, pvar);
}

bool skope::PrepareAndCallUDF(const AstNode* pCalling, CVar* pBase, CVar* pStaticVars)
{
	//if (!pCalling->str)
	//	throw exception_etc(*this, pCalling, "p->str null pointer in PrepareAndCallUDF(p,...)").raise();
	//// Check if the same udf is called during debugging... in that case Script shoudl be checked and handled...

	//// Checking the number of input args used in the call
	//size_t nargs = 0;
	//if (pCalling->alt)
	//	for (auto pp = pCalling->alt->child; pp; pp = pp->next)
	//		nargs++;
	//CAstSigEnv tempEnv(*pEnv);
	//son.reset(new skope(&tempEnv));
	//son->u = u;
	//son->u.title = pCalling->str;
	//son->u.debug.status = null;
	//son->lhs = lhs;
	//son->level = level + 1;
	//son->baselevel.front() = baselevel.back();
	//son->dad = this; // necessary when debugging exists with stepping (F10), the stepping can continue in tbe calling skope without breakpoints. --=>check 7/25
	//son->fpmsg = fpmsg;
	//if (GOvars.find("?foc") != GOvars.end()) son->GOvars["?foc"] = GOvars["?foc"];
	//if (GOvars.find("gcf") != GOvars.end())	son->GOvars["gcf"] = GOvars["gcf"];
	//auto udftree = pEnv->udf.find(pCalling->str);
	//if (udftree != pEnv->udf.end())
	//{
	//	son->u.t_func = (*udftree).second.uxtree;
	//	son->u.t_func_base = son->u.t_func;
	//	son->u.base = son->u.t_func->str;
	//}
	//else
	//{
	//	auto udftree2 = pEnv->udf.find(u.base); // if this is to be a local udf, base should be ready through a previous iteration.
	//	if (udftree2 == pEnv->udf.end())
	//		throw CAstException(INTERNAL, *this, pCalling).proc("PrepareCallUDF():supposed to be a local udf, but AstNode with that name not prepared");
	//	son->u.t_func_base = (*udftree2).second.uxtree;
	//	son->u.base = u.base; // this way, base can maintain through iteration.
	//	son->u.t_func = (*pEnv->udf.find(u.base)).second.local[pCalling->str].uxtree;
	//}
	//son->xtree = son->u.t_func_base->child->next;
	//son->xtree = son->u.t_func->child->next;
	////output argument string list
	//son->u.argout.clear();
	//AstNode* pOutParam = son->u.t_func->alt;
	//ostringstream oss;
	//if (pOutParam) {
	//	if (pOutParam->type == N_VECTOR)
	//	{
	//		if (pOutParam->str)
	//			pOutParam = ((AstNode*)pOutParam->str)->alt;
	//		else
	//			pOutParam = pOutParam->alt;
	//		for (AstNode* pf = pOutParam; pf; pf = pf->next)
	//			son->u.argout.push_back(pf->str);
	//		son->u.nargout = (int)son->u.argout.size();
	//	}
	//	else
	//	{
	//		oss << "TYPE=" << pOutParam->type << "PrepareCallUDF():UDF output should be alt and N_VECTOR";
	//		throw CAstException(INTERNAL, *this, pCalling).proc(oss.str().c_str());
	//	}
	//}
	//AstNode* pf, * pa;	 // formal & actual parameter; pa is for this, pf is for son
	////Checking requested output arguments vs formal output arguments
	//if (lhs)
	//{
	//	AstNode* p = lhs->type == N_VECTOR ? ((AstNode*)lhs->str)->alt : lhs;
	//	if (lhs->type == N_VECTOR) for (son->u.nargout = 0; p && p->line == lhs->line; p = p->next, son->u.nargout++) {}
	//	if (son->u.nargout > (int)son->u.argout.size()) {
	//		oss << "More output arguments than in the definition (" << son->u.argout.size() << ").";
	//		throw CAstException(USAGE, *this, pCalling).proc(oss.str().c_str(), son->u.t_func->str);
	//	}
	//}
	//else
	//	son->u.nargout = (int)son->u.argout.size(); // probably not correct
	//// input parameter binding
	//pa = pCalling->alt;
	////If the line invoking the udf res = udf(arg1, arg2...), pa points to arg1 and so on
	//if (pa && pa->type == N_ARGS)
	//	pa = pa->child;
	////If the line invoking the udf res = var.udf(arg1, arg2...), binding of the first arg, var, is done separately via pBase. The rest, arg1, arg2, ..., are done below with pf->next
	//pf = son->u.t_func->child->child;
	//if (pBase) { son->SetVar(pf->str, pBase); pf = pf->next; }
	////if this is for udf object function call, put that psigBase for pf->str and the rest from pa
	//for (son->u.nargin = 0; pa && pf; pa = pa->next, pf = pf->next, son->u.nargin++)
	//{
	//	CVar tsig = Compute(pa);
	//	if (tsig.IsGO())
	//		son->SetVar(pf->str, pgo); // variable pf->str is created in the context of son
	//	else
	//		son->SetVar(pf->str, &tsig); // variable pf->str is created in the context of son
	//}
	//if (nargs > son->u.nargin)
	//{
	//	oss << "Excessive input arguments specified (expected " << son->u.nargin << ", given " << nargs << ") :";
	//	throw CAstException(USAGE, *this, pCalling).proc(oss.str().c_str(), son->u.t_func->str);
	//}
	////son->u.nargin is the number of args specified in udf
	//if (u.debug.status == stepping_in) son->u.debug.status = stepping;
	//xscope.push_back(son.get());
	////son->SetVar("_________",pStaticVars); // how can I add static variables here???
	//size_t nArgout = son->CallUDF(pCalling, pBase);
	//// output parameter binding
	//vector<CVar*> holder;
	//size_t cnt = 0;
	//// output argument transfer from son to this
	//double nargin = son->Vars["nargin"].value();
	//double nargout = son->Vars["nargout"].value();
	////Undefined output variables are defined as empty
	//for (auto v : son->u.argout)
	//	if (son->Vars.find(v) == son->Vars.end() && (son->GOvars.find(v) == son->GOvars.end() || son->GOvars[v].front()->IsEmpty()))
	//		son->SetVar(v.c_str(), new CVar);
	////lhs is either NULL (if not specified), T_ID or N_VECTOR
	//if (lhs)
	//{
	//	outputbinding(pCalling, nArgout);
	//}
	//else // no output parameter specified. --> first formal output arg goes to ans
	//{
	//	if (son->u.argout.empty())
	//		Sig.Reset();
	//	else
	//	{
	//		if (son->Vars.find(son->u.argout.front()) != son->Vars.end())
	//			Sig = son->Vars[son->u.argout.front()];
	//		else
	//		{
	//			if (son->GOvars[son->u.argout.front()].size() > 1)
	//				Sig = *(pgo = MakeGOContainer(son->GOvars[son->u.argout.front()]));
	//			else if (son->GOvars[son->u.argout.front()].size() == 1)
	//				Sig = *(pgo = son->GOvars[son->u.argout.front()].front());
	//			else
	//				Sig = CVar();
	//		}
	//	}
	//}
	//if ((son->u.debug.status == stepping || son->u.debug.status == continuing) && u.debug.status == null)
	//{ // no b.p set in the main udf, but in these conditions, as the local udf is finishing, the stepping should continue in the main udf, or debug.status should be set progress, so that the debugger would be properly exiting as it finishes up in CallUDF()
	//	u.debug.GUI_running = true;
	//	if (son->u.debug.status == stepping) // b.p. set in a local udf
	//		u.debug.status = stepping;
	//	else // b.p. set in other udf
	//		u.debug.status = progress;
	//}
	//if (son->GOvars.find("?foc") != son->GOvars.end()) GOvars["?foc"] = son->GOvars["?foc"];
	//if (son->GOvars.find("gcf") != son->GOvars.end()) GOvars["gcf"] = son->GOvars["gcf"];
	//u.pLastRead = NULL;
	//if (pgo) pgo->functionEvalRes = true;
	//Sig.functionEvalRes = true;
	//xscope.pop_back(); // move here????? to make purgatory work...
	return true;
}


AstNode* skope::ReadUDF(string& emsg, const char* udf_filename)
{
	// returns node for the UDF, if it exists.
	if (!udf_filename) return NULL;
	if (strlen(udf_filename) > 255) return NULL; //MR 1
	emsg.clear();
	AstNode* pout;
	if (pEnv->udf.empty())
	{ // Read aux private functions
	}
	//local functions have precedence; i.e., if fun1() has myloc() in it, the file myloc.aux will not be searched.
	auto udf_finder = pEnv->udf.find(u.base);
	if (udf_finder != pEnv->udf.end())
	{
		map<string, UDF>::iterator jt = udf_finder->second.local.find(udf_filename);
		if (jt != udf_finder->second.local.end())
			return jt->second.uxtree;
	}
	//Search for the file
	string fullpath;
	FILE* auxfile = fopen_from_path(udf_filename, "aux", fullpath);
	if (!auxfile)
	{ //if not found, it shoulbe be a built-in or local function, then return
		if (pEnv->udf.find(udf_filename) == pEnv->udf.end())
			return NULL;
		else // file not found but it is not a local function.... what can it be?
			return pEnv->udf[udf_filename].uxtree; // check
	}
	//file found
	transform(fullpath.begin(), fullpath.end(), fullpath.begin(), ::tolower);
	string filecontent;
	//if (GetFileText(auxfile, filecontent) <= 0)
	//{// File reading error or empty file
	//	emsg = string("Cannot read specified udf or script file ") + udf_filename;
	//	fclose(auxfile);
	//	return NULL;
	//}
	fclose(auxfile);
	// if udf exists and filecontent is still the content, use it
	auto udftree = pEnv->checkout_udf(udf_filename, filecontent);
	if (udftree)
		pout = udftree; // should be the same
	else
	{ // if not, register or re-register the udf with the new content
		if (!(pout = pEnv->checkin_udf(udf_filename, fullpath, filecontent, emsg)))
		{ // parsing error in udf file
			char buf[256];
			sprintf(buf, " in %s", fullpath.c_str());
			emsg += buf;
			auto fd = pEnv->udf.find(udf_filename);
			pEnv->udf.erase(fd);
			return NULL; // error caught here
		}
	}
	//pout->child should be ID_LIST
	if (!pout->child->child) // function takes no input argument
		pout->suppress = 3;
	return pout;
}

FILE* skope::fopen_from_path(string fname, string ext, string& fullfilename)
{ // in in out
	char drive[64], dir[MAX_PATH], filename[MAX_PATH], extension[MAX_PATH];
//	_splitpath(fname.c_str(), drive, dir, filename, extension);
	transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	char type[4];
	if (ext == "txt") strcpy(type, "rt");
	else						strcpy(type, "rb");
	size_t pdot = fname.rfind('.');
	if ((pdot == fname.npos || pdot < fname.length() - 4) && !extension[0])
		fname += "." + ext;

	if (drive[0] + dir[0] > 0)
	{ // if path is included in fname, open with fname
		fullfilename = fname;
		return fopen(fname.c_str(), type);
	}
	else {
		string ofname(fname);
		// search fname in AuxPath, beginning with current working directory
		fullfilename = pEnv->AppPath + fname;
		FILE* file = fopen(fullfilename.c_str(), type);
		if (!file)
		{
			for (auto path : pEnv->AuxPath)
			{
				fullfilename = path + fname;
				file = fopen(fullfilename.c_str(), type);
				if (file) return file;
			}
		}
		return file;
	}
}

AstNode* CAstSigEnv::checkin_udf(const string& udfname, const string& fullpath, const string& filecontent, string& emsg)
{
	AstNode* pout = NULL;
	skope qscope(this);
	//qscope.Script = udfname;
	//transform(qscope.Script.begin(), qscope.Script.end(), qscope.Script.begin(), ::tolower);
	//udf[qscope.Script].newrecruit = true;
	//auto udftree = qscope.parse_aux(filecontent.c_str(), emsg);
	//if (udftree)
	//{
	//	qscope.node = udftree;
	//	pout = qscope.RegisterUDF(qscope.node, fullpath.c_str(), filecontent);
	//	// The following should be after all the throws. Otherwise, the UDF AST will be dangling.
	//	// To prevent de-allocation of the AST of the UDF when qscope goes out of skope.
	//	if (qscope.node->type == N_BLOCK)
	//		qscope.node->next = NULL;
	//	else
	//		qscope.node = NULL;
	//	return pout;
	//}
	return NULL;
}

AstNode* CAstSigEnv::checkout_udf(const string& udfname, const string& filecontent)
{
	// if udfname is present in the environment AND
	//    the content is the same as filecontent
	// returns xtree
	// otherwise NULL

	//assumption: udfname is all lower case
	if (udf.find(udfname) != udf.end())
	{
		if (udf[udfname].content == filecontent)
			return udf[udfname].uxtree;
	}
	return NULL;
}

//RECOMMENDED CONSTRUCTOR 1  7/8/2017
skope::skope(CAstSigEnv* env) // Use this constructor for auxlab. env has been defined prior to this.
{
	init();
	pEnv = env;
}
//RECOMMENDED CONSTRUCTOR 2  7/8/2017
//Now (10/10/2018), if this is used not for the temporary variable during func in AuxFunc.cpp, then the validity of commenting out dad = src->dad; should be checked.
skope::skope(const skope* src)
{
	init();
	Vars = src->Vars;
	GOvars = src->GOvars;
	u = src->u;
	if (src) {
		pEnv = src->pEnv;
	}
	else
		pEnv = new CAstSigEnv(22050);
	node = src->node;
	//endpoint = src->endpoint;
	//pgo = src->pgo;
	//Script = src->Script;
	//pLast = src->pLast;
	//inTryCatch = src->inTryCatch;
}


void skope::init()
{
	node = NULL;
	//statusMsg = "";
	//fAllocatedAst = false;
	//pLast = NULL;
	//son = NULL;
	//dad = NULL;
	//lhs = NULL;

	//FsFixed = false;
	//pgo = NULL;
	//Tick0 = 1;
	//inTryCatch = 0;
	//level = 1;
	//baselevel.push_back(level);
}

skope& skope::SetVar(const char* name, CVar* psig, CVar* pBase)
// To do--chanage CVar *psig to const CVar &tsig and make sure the second arg is the const, target signal to use
//to do so, improve the case of psig->GetFs() == 3, so that psig is not changed, let's think about it how.
//11/6/2019
{// NULL pBase --> name will be the variable in the workspace.
 //non-NULL pBase --> pBase is a class variable. name will be a member variable under pBase.
	if (!pBase) // top skope
	{
		map<string, CVar>::iterator it = Vars.find(name);
		map<string, vector<CVar*>>::iterator jt = GOvars.find(name);
		if (psig->IsGO()) // name and psig should be fed to GOvars
		{
			//gca, gcf shouldn't be "push_backed" but instead replaced.
			if (!strcmp(name, "gca") || !strcmp(name, "gcf"))
				GOvars[name].clear();
			if (jt != GOvars.end())  GOvars.erase(jt);
			if (psig->GetFs() == 3)
			{
				if (psig->nSamples == 1)
				{
					psig = (CVar*)(INT_PTR)psig->value();
					GOvars[name].push_back(psig);
				}
				else
					for (unsigned int k = 0; k < psig->nSamples; k++)
						GOvars[name].push_back((CVar*)(INT_PTR)psig->buf[k]);
			}
			else
				GOvars[name].push_back(psig);
			if (it != Vars.end()) Vars.erase(it);
		}
		else // name and psig should be fed to Var
		{
			Vars[name] = *psig;
			if (jt != GOvars.end())  GOvars.erase(jt);
		}
	}
	else
	{
		if (psig->IsGO()) // name and psig should be fed to struts
		{
			// Previous one should be cleared.
			// I wonder if this clear() would cause an issue
			// What has the SetVar convention been for GO 1/3/2021
			pBase->struts[name].clear();
			pBase->struts[name].push_back(psig);
			auto it = pBase->strut.find(name);
			if (it != pBase->strut.end()) pBase->strut.clear();
		}
		else // name and psig should be fed to strut
		{
			pBase->strut[name] = *psig;
			auto jt = pBase->struts.find(name);
			if (jt != pBase->struts.end())  pBase->struts[name].clear();
		}
	}
	return *this;
}

string skope::makefullfile(const string& fname, string extension)
{
	string fullfilename;
#ifdef _WINDOWS
	char drive[64], dir[MAX_PATH], file[MAX_PATH], ext[MAX_PATH];
	_splitpath(fname.c_str(), drive, dir, file, ext);
	if (drive[0] == 0 && dir[0] == 0) // no directory info or current directory
	{
		fullfilename = pEnv->AppPath;
		fullfilename += fname;
	}
	else if (drive[0] == 0 && !strcmp(dir, ".\\")) // no directory info or current directory
	{
		fullfilename = pEnv->AppPath;
		fullfilename += file;
		fullfilename += ext;
	}
	else
		fullfilename = fname;
	// if the target extension is not specified, add default extension
	if (!ext[0] && !extension.empty())
		fullfilename += extension;
#else
	auto tcopy = fname;
	fullfilename = fname;
	transform(tcopy.begin(), tcopy.end(), tcopy.begin(), ::tolower);
	if (tcopy.substr(tcopy.size() - 4) != ".wav")
		fullfilename += ".wav";
#endif
	return fullfilename;
}

int skope::checkArgsType(const AstNode* pnode, const AstNode* p, const Cfunction& fn)
{
	bool pass = false;
	//for (auto sets : fn.allowed_arg_types)
	//{
	//	for (auto tp : sets)
	//	{
	//		pass = 

	//	}
	//}
	return 1;
}
int skope::checkNumArgs(const AstNode* pnode, const AstNode* p, string& FuncSigs, int minArgs, int maxArgs)
{
	ostringstream msg;
	if (minArgs > 0 && !p) msg << "must have at least 1 argument.";
	else
	{
		int nArgs(0);
		if (!p || p->type == N_STRUCT) return nArgs;
		for (const AstNode* cp = p; cp; cp = cp->next)
			++nArgs;
		msg << "must have ";
		if (minArgs == 0 && maxArgs == 0 && nArgs > 0)
			msg << "0 argument.";
		else if (nArgs < minArgs && maxArgs == 0)
			msg << "at least " << minArgs << (minArgs > 1 ? " arguments." : " argument.");
		else if (nArgs < minArgs || maxArgs > 0 && nArgs > maxArgs) {
			msg << minArgs;
			for (int i = minArgs + 1; i < maxArgs; ++i)
				msg << ", " << i;
			if (minArgs != maxArgs)
				msg << " or " << maxArgs;
			msg << (maxArgs > 1 ? " arguments." : " argument.");
		}
		else
			return nArgs;
	}
	throw exception_etc(*this, pnode, msg.str()).raise();
}

unsigned long skope::tic()
{
	// FInd out something equivalent to GetTickCount in Linux
//	return Tick0 = 0; // GetTickCount0();
	return 0;
}

unsigned long skope::toc(const AstNode* p)
{
	return 0;
	//if (Tick0 == 1)
	//	throw CAstException(USAGE, *this, p).proc("toc called without tic");
	//return Tick1 = (GetTickCount0() - Tick0);
}


//Maybe move to SAstSigEnv?? 11/19/2021
bool skope::HandlePseudoVar(const AstNode* pnode)
{
	string fname = pnode->str;
	string dummy;
	vector<CVar> dummy2;
	auto it = pEnv->pseudo_vars.find(fname);
	if (it != pEnv->pseudo_vars.end())
		(*it).second.func(this, pnode, dummy2);
	else
		return false;
	return true;
}
CVar* skope::pseudoVar(const AstNode* pnode, AstNode* p, CSignals* pout)
{
	//	int res;
	string dummy;
	if (!pout) pout = &Sig;
	if (pnode->type == N_HOOK)
		HandlePseudoVar(pnode);
	else
	{
		if (p->type == N_ARGS) p = p->child;
		if (p->type == N_HOOK) p = p->child;
		if (!strcmp(p->str, "sel"))
		{
			if (p->alt)
			{
				float tp[2];
				memcpy(tp, pout->buf, sizeof(float) * 2);
				pout->Reset();
				if (!strcmp(p->alt->str, "t1"))
					pout->SetValue(tp[0]);
				else if (!strcmp(p->alt->str, "t2"))
					pout->SetValue(tp[1]);
			}
		}
	}
	return &Sig; // nominal return value
}

CVar* skope::NodeVector(const AstNode* pn)
{
	// vector should not allow string, cell, or audio object.
	// Also it should not allow struct or structs and not GO
	// scalar, bool, complex OK
	// Inspect each element. After the first element, nRows is established
	// If any subsequent element produces different nRows, throw
	// If the first element is bool, all the other must be bool
	// If the first element is real, continue as real.
	//    (if any subsequent element is complex, make a new vector as a complex)
	// If the first element is complex, continue as complex
	// If the first element is a GO, all must be GO.

	AstNode* p = pn->str ? (AstNode*)pn->str : (AstNode*)pn;
	CVar* psig0, * psig;
	bool GO0, GO = false;
	unsigned int ngroups0, ngroups = 0;
	vector<bool> bbuf;
	vector<float> dbuf;
	vector<complex<float>> cbuf;
	unsigned int nCount = 0;
	for (p = p->alt; p; p = p->next, nCount++)
	{
		psig = Compute(p);
		blockString2(*this, pn, *psig);
		//		blockTemporal(pn, *psig);
//		GO = blockCell_allowGO(pn, *psig);
		ngroups = psig->nGroups; // first number of rows
		auto type = psig->type();
		if (!nCount) {
			if (!p->next) return &(Sig = *psig); // if only single item, return here
			if (psig->IsGO()) {
				if (psig->GetFs() == 3)
					for (unsigned int k = 0; k < psig->nSamples; k++) dbuf.push_back(psig->buf[k]);
				else
					dbuf.push_back((float)(INT_PTR)pgo); // psig is the address of Sig, which is a copy; pgo is correct 9/28/2020
			}
			else if (psig->IsBool())
				for (unsigned int k = 0; k < psig->nSamples; k++) bbuf.push_back(psig->logbuf[k]);
			else if (psig->IsComplex())
				for (unsigned int k = 0; k < psig->nSamples; k++) cbuf.push_back(psig->cbuf[k]);
			else
				for (unsigned int k = 0; k < psig->nSamples; k++) dbuf.push_back(psig->buf[k]);
		}
		else
		{ // if this is not the first, check with history
			if (!psig->type()) continue;
			if (ngroups0 != ngroups)
				throw exception_etc(*this, pn, "Attempting to append an element with a different row count from existing one.").raise();
			if (GO ^ GO0)
				throw exception_etc(*this, pn, "Attempting to append a GO to a non-Go array.").raise();
			// if bool it must be bool throughout
			if (!bbuf.empty())
			{
				if (psig->IsBool() && !psig->IsGO())
					for (unsigned int k = 0; k < psig->nSamples; k++) bbuf.push_back(psig->logbuf[k]);
				else
					throw exception_etc(*this, pn, "Attempting to append a non-bool element to a bool array").raise();
			}
			else
			{
				if (psig->IsGO()) {
					if (psig->GetFs() == 3)
						for (unsigned int k = 0; k < psig->nSamples; k++) dbuf.push_back(psig->buf[k]);
					else
						dbuf.push_back((float)(INT_PTR)pgo);
				}
				else if (psig->IsBool() && !psig->IsGO())
					throw exception_etc(*this, pn, "Attempting to append a bool element to a non-bool array").raise();
				else if (!cbuf.empty())
				{
					if (psig->IsComplex())
						for (unsigned int k = 0; k < psig->nSamples; k++) cbuf.push_back(psig->cbuf[k]);
					else
						for (unsigned int k = 0; k < psig->nSamples; k++) cbuf.push_back(psig->buf[k]);
				}
				else
				{ // dbuf must be non-empty
					if (psig->IsComplex())
					{
						//convert it to a complex array and clear dbuf
						for (auto v : dbuf) cbuf.push_back(v);
						for (unsigned int k = 0; k < psig->nSamples; k++) cbuf.push_back(psig->cbuf[k]);
						dbuf.clear();
					}
					else
						for (unsigned int k = 0; k < psig->nSamples; k++) dbuf.push_back(psig->buf[k]);
				}
			}
		}
		GO0 = GO;
		psig0 = psig;
		ngroups0 = ngroups;
	}
	CVar out;
	if (!bbuf.empty())
	{
		out.UpdateBuffer((unsigned)bbuf.size());
		out.MakeLogical();
		int k = 0;
		for (auto v : bbuf) out.logbuf[k++] = v;
	}
	else if (!cbuf.empty())
	{
		out.UpdateBuffer((unsigned)cbuf.size());
		out.SetComplex();
		int k = 0;
		for (auto v : cbuf) out.cbuf[k++] = v;
	}
	else
	{
		out.UpdateBuffer((unsigned)dbuf.size());
		int k = 0;
		for (auto v : dbuf) out.buf[k++] = v;
	}
	//Do something about nColumn
	if (GO) out.SetFs(3);
	return &(Sig = out);
}

vector<float> skope::gettimepoints(const AstNode* pnode, AstNode* p)
{ // assume: pnode type is N_TIME_EXTRACT
	vector<float> out;
	CVar tsig1 = Compute(p);
	if (!tsig1.IsScalar())
		throw exception_misuse(*this, pnode, "Time marker should be scalar", 1);
	CVar tsig2 = Compute(p->next);
	if (!tsig2.IsScalar())
		throw exception_misuse(*this, pnode, "Time marker should be scalar", 2);
	out.push_back(tsig1.value());
	out.push_back(tsig2.value());
	return out;
}

CVar* skope::NodeMatrix(const AstNode* pnode)
{ //[x1; x2]  if for a stereo audio signal, both x1 and x2 must be audio
	// if none of these elements are audio, it can have multiple rows [x1; x2; x3; .... xn]. But these elements must be the same length.
	AstNode* p = ((AstNode*)pnode->str)->alt;
	CVar esig, tsig;
	if (!p) return &(Sig = CVar());
	tsig = Compute(p);
	blockCellStruct2(*this, pnode, tsig);
	auto tp = tsig.type();
	int audio = 0;
	if (ISAUDIO(tp)) audio = 1;
	else if (ISTSEQ(tp)) audio = 10;
	else if (ISSTRING(tp) || ISSCALAR(tp) || ISVECTOR(tp)) audio = -1;
	if (audio == 0 && !p->next)
		return &Sig.Reset();
	CVar* psig = &tsig;
	unsigned int k(1);
	if (audio >= 0)
	{
		if (!p->next) // must be audio > 0
		{
			CSignals* nullnext = new CSignals(tsig.GetFs());
			tsig.SetNextChan(*nullnext);
		}
		else
		{
			if (p->next->alt && p->next->alt->type == N_STRUCT && p->next->alt == pnode->tail) // in the last loop, if p->alt is structure and the same as pnode->tail; i.e., the "dot" operation applies to the matrix, not the vector, so, Compute(p), which tries to apply the dot opeartor to the vector through N_VECTOR, shouldn't be called. The matrix-wide dot operation should be handled at the bottom. 4/17/2018
				p->next->alt = NULL;
			esig = Compute(p->next);
			blockCellStruct2(*this, pnode, esig);
			if (audio == 0 && (esig.IsAudio()|| ISTSEQ(esig.type())))
			{
				if (p->next->next)	throw exception_etc(*this, pnode, "Currently two channels or less for audio signals or t-series are allowed.").raise();
				tsig.SetNextChan(esig);
			}
			else if (audio > 0)
			{
				if (p->next->next)	throw exception_etc(*this, pnode, "Currently two channels or less for audio signals or t-series are allowed.").raise();
				if (esig.GetType() != tsig.GetType())
					throw exception_etc(*this, pnode, "Signal type, Audio(or t - series), must be the same in both channels.").raise();
				tsig.SetNextChan(esig);
			}
		}
	}
	else
	{
		for (p = p->next; p; p = p->next, k++) {
			if (!p->next && p->alt && p->alt->type == N_STRUCT && p->alt == pnode->tail) // in the last loop, if p->alt is structure and the same as pnode->tail; i.e., the "dot" operation applies to the matrix, not the vector, so, Compute(p), which tries to apply the dot opeartor to the vector through N_VECTOR, shouldn't be called. The matrix-wide dot operation should be handled at the bottom. 4/17/2018
				p->alt = NULL;
			esig = Compute(p);
			if (!esig.IsEmpty() && tsig.Len() != esig.Len()) throw exception_etc(*this, pnode, "All groups must have the same length for 2-D data. (All rows must be the same length in the matrix)").raise();
			tsig += &esig;
			tsig.nGroups += esig.nGroups;
		}
	}
	return &(Sig = tsig);
}

CVar* skope::Dot(AstNode* p)
{ // apply dot operator to Sig that was computed from the previous node
	// At this point, p is alt from a previous node and should not have child (i.e., RHS);
	// therefore, lhs should not be updated here.
	CNodeProbe np(this, p, &Sig);
	read_nodes(np);
	if (np.psigBase->IsGO() && np.psigBase->GetFs() != 3) return pgo;
	else	return &Sig;
}

void skope::Transpose(const AstNode* pnode, AstNode* p)
{
	Compute(p);
	Sig.transpose();
}

void skope::Concatenate(const AstNode* pnode, AstNode* p)
{
	ostringstream oss;
	CVar tsig = Compute(p->next);
	if (pgo)
	{ //special treatment needed to multiple GO's
		vector<CVar*> tp;
		tp.push_back(pgo);
		Compute(p);
		if (Sig.IsEmpty())
		{
			Sig = *pgo;
			return;
		}
		//Now, Sig can be CSIG_HDLARRAY, then use it as is.
		if (!pgo)
			throw exception_etc(*this, p, "RHS is a graphic handle. LHS is not. Can't concatenate.").raise();
		//if (Sig.GetType() == CSIG_HDLARRAY)
		//{
		//	Sig.UpdateBuffer(Sig.nSamples + 1);
		//	Sig.buf[Sig.nSamples - 1] = (float)(INT_PTR)tp.front();
		//}
		return;
	}
	Compute(p);
	uint16_t a = tsig.type();
	uint16_t b = Sig.type();
	if (a & TYPEBIT_CELL)
	{
		if (b & TYPEBIT_CELL)
		{
			for (size_t k = 0; k < tsig.cell.size(); k++)
				Sig.cell.push_back(tsig.cell[(int)k]);
		}
		else
			Sig.cell.push_back(tsig);
	}
	else
	{
		if (b > 0 && a >> 2 != b >> 2)
			throw exception_etc(*this, p, "Different object type between LHS and RHS. Can't concatenate.");
		//Check rejection conditions
		if (tsig.nSamples * Sig.nSamples > 0) // if either is empty, no rejection
		{
			if (tsig.nGroups != Sig.nGroups && tsig.Len() != Sig.Len())
				throw exception_etc(*this, p->next, "To concatenate, the second operand must have the same number of elements or the same number of groups (i.e., rows) ");
		}
		//For matrix, Group-wise (i.e., row-wise) concatenation
		if (Sig.nGroups > 1 && Sig.Len() != tsig.Len())
		{ //  append row-wise
			unsigned int len0 = Sig.Len();
			Sig.UpdateBuffer(Sig.nSamples + tsig.nSamples);
			unsigned int len1 = Sig.Len();
			unsigned int lent = tsig.Len();
			for (unsigned int k, kk = 0; kk < Sig.nGroups; kk++)
			{
				k = Sig.nGroups - kk - 1;
				memcpy(Sig.buf + len1 * k, Sig.buf + len0 * k, sizeof(double) * len0);
				memcpy(Sig.buf + len1 * k + len0, tsig.buf + lent * k, sizeof(double) * lent);
			}
		}
		else
		{ // Sig and tsig have both row and column counts, concatenation is done here... at the end of row--making more rows withthe same column counts (not row-wise concatenating)
			if (Sig.nGroups > 1) Sig.nGroups += tsig.nGroups;
			Sig += &tsig;
			Sig.MergeChains();
		}
	}
}

CVar* skope::ConditionalOperation(const AstNode* pnode, AstNode* p)
{
	//	why pgo = NULL; ?
	// pgo should be reset right after all Compute calls so upon exiting ConditionalOperation
	// it shouldn't have any lingering pgo.
	// pgo is supposed to be used only temporarily-- to relay go to the next step and it shouldn't linger too long.
	// then at some point it may incorrectly try to process the GO when it is not about GO.
	// 2/5/2019
	CVar rsig;
	switch (pnode->type)
	{
	case '<':
	case '>':
	case T_LOGIC_LE:
	case T_LOGIC_GE:
	case T_LOGIC_EQ:
	case T_LOGIC_NE:
		rsig = Compute(p->next);
		blockCellStruct2(*this, pnode, rsig);
		Compute(p);
		pgo = NULL;
		blockCellStruct2(*this, pnode, Sig);
		if (Sig.IsString())
			Sig.SetValue((float)((CSignal)Sig == (CSignal)rsig));
		else
			Sig.LogOp(rsig, pnode->type);
		break;
	case T_LOGIC_NOT:
		Sig = Compute(p);
		blockCellStruct2(*this, pnode, Sig);
		Sig.MakeLogical();
		Sig.LogOp(rsig, pnode->type); // rsig is a dummy for func signature.
		break;
	case T_LOGIC_AND:
		rsig = ConditionalOperation(p, p->child);
		if (rsig.nSamples == 1 && !rsig.logbuf[0])
		{
			Sig.bufBlockSize = 1;
			Sig.logbuf[0] = false;
			break;
		}
		ConditionalOperation(p->next, p->next->child);
		Sig.LogOp(rsig, pnode->type);
		break;
	case T_LOGIC_OR:
		rsig = ConditionalOperation(p, p->child);
		if (rsig.nSamples == 1 && rsig.logbuf[0])
		{
			Sig.bufBlockSize = 1;
			Sig.logbuf[0] = true;
			break;
		}
		ConditionalOperation(p->next, p->next->child);
		Sig.LogOp(rsig, pnode->type);
		break;
	case T_NUMBER:
		Compute(pnode);
		Sig.MakeLogical();
		if (Sig.value() == 0.)		Sig.logbuf[0] = false;
		else Sig.logbuf[0] = true;
		break;
	case N_VECTOR:
	case N_MATRIX:
		//Coming here: if [1 1].and statement; end
		return Compute(pnode);
		break;
	default:
		//Coming here: if and([1 1]) statement; end
		return TID((AstNode*)pnode, NULL);
		//		Compute(pnode);
		break;
	}
	//	return Sig;
		//Need this instead of return Sig to cover
		// if (a==[1 4 5]).and statement; end
	return TID(pnode->alt, NULL, &Sig);
}

CVar* skope::SetLevel(const AstNode* pnode, AstNode* p)
{
	//CVar sigRMS, refRMS, dB = Compute(p->next);
	//// if tsig is scalar -- apply it across the board of Sig
	//// if tsig is two-element vector -- if Sig is stereo, apply each; if not, take only the first vector and case 1
	//// if tsig is stereo-scalar, apply the scalar to each L and R of Sig. If Sig is mono, ignore tsig.next
	//// if tsig is tseq, it must have the same chain and next structure (exception otherwise)
	//if (p->type == '@')
	//{// trinary
	//	if (dB.type() & TYPEBIT_TSEQ)
	//		throw exception_etc(*this, pnode, "sig @ ref @ level ---- level cannot be time sequence.").raise();
	//	refRMS <= Compute(p->child->next);
	//	if (!refRMS.IsAudio())
	//		throw exception_etc(*this, pnode, "sig @ ref @ level ---- ref must be an audio signal.").raise();
	//	refRMS.RMS(); // this should be called here, once another Compute is called refRMS.buf won't be valid
	//	Compute(p->child);
	//	sigRMS <= Sig;
	//	sigRMS.RMS();
	//	sigRMS -= refRMS;
	//}
	//else
	//{
	//	Sig = Compute(p);
	//	sigRMS <= Sig;
	//	if (dB.type() == TYPEBIT_TSEQ + 1) // scalar time sequence
	//	{// for tseq leveling, % operator is used.
	//		for (CTimeSeries* p = &dB; p; p = p->chain)
	//			p->buf[0] = pow(10, p->buf[0] / 20.);
	//		for (CTimeSeries* p = dB.next; p; p = p->chain)
	//			p->buf[0] = pow(10, p->buf[0] / 20.);
	//		Sig% dB;
	//		return &Sig;
	//	}
	//	// sig @ level
	//	// if sig is chained, level is applied across all chains
	//	// i.e., currently there's no simple way to specify chain-specific levels 6/17/2020

	//	// A known hole in the logic here---if dB is stereo but first channel is scalar
	//	// and next is chained, or vice versa, this will not work
	//	// Currently it's difficult to define dB that way (maybe possible, but I can't think about an easy way)
	//	// A new, simpler and intuitive way to define T_SEQ should be in place
	//	// before fixing this hole.  10/7/2019

	//	//Being fixed.... but need further checking
	//	//6/17/2020
	//	//if (dB.chain || (dB.next && dB.next->chain))
	//	//	sigRMS = sigRMS.evoke_getval(&CSignal::RMS);
	//	//else
	//		sigRMS.RMS(); // this should be called before another Compute is called (then, refRMS.buf won't be valid)
	//}
	////Reject dB if empty, if string, or if bool
	//if (dB.IsEmpty() || dB.IsString() || dB.IsBool())
	//	throw exception_etc(*this, pnode, "Target_RMS_dB after @ must be a real value.").raise();
	//if (dB.nSamples > 1 && !dB.next)
	//	dB.SetNextChan(new CSignals(dB.buf[1]));
	//sigRMS = dB - sigRMS;
	//Sig | sigRMS;
	return &Sig;
}

void skope::prepare_endpoint(const AstNode* p, CVar* pvar)
{  // p is the node the indexing starts (e.g., child of N_ARGS... wait is it also child of conditional p?
	if (p->next) // first index in 2D
		endpoint = (float)pvar->nGroups;
	else
		endpoint = (float)pvar->nSamples;
}

void skope::interweave_indices(CVar& isig, CVar& isig2, unsigned int len)
{
	CVar out;
	out.UpdateBuffer(isig.nSamples * isig2.nSamples);
	out.nGroups = isig.nGroups;
	for (size_t p = 0; p < isig.nSamples; p++)
		for (unsigned int q = 0; q < isig2.nSamples; q++)
		{
			out.buf[p * isig2.nSamples + q] = (isig.buf[p] - 1) * len + isig2.buf[q];
		}
	isig = out;
}

void skope::index_array_satisfying_condition(CVar& isig)
{ // input: isig, logical array
  // output: isig, a new array of indices satisfying the condition
	CTimeSeries* p = &isig;
	CVar out;
	CTimeSeries* p_out = &out;
	while (p)
	{
		int count = 0;
		for (unsigned int k = 0; k < p->nSamples; k++)
			if (p->logbuf[k]) count++;
		CSignal part;
		part.UpdateBuffer(count);
		part.nGroups = p->nGroups;
		part.tmark = p->tmark;
		count = 0;
		for (unsigned int k = 0; k < p->nSamples; k++)
			if (p->logbuf[k]) part.buf[count++] = k + 1;
		*p_out = part;
		p = p->chain;
		if (p)
		{
			p_out->chain = new CTimeSeries;
			p_out = p_out->chain;
		}
	}
	isig = out;
}

CVar* skope::InitCell(const AstNode* pnode, AstNode* p)
{
	try {
		skope temp(this); // temp is used to protect Sig
		int count = 0;
		// x={"bjk",noise(300), 4.5555}
		for (; p; count++, p = p->next)
			;
		p = pnode->child;
		Sig.Reset(1);
		Sig.cell.reserve(count);
		for (; p; p = p->next)
			Sig.appendcell(*temp.Compute(p));
		if (pnode->str)
			SetVar(pnode->str, &Sig);
		return &Sig;
	}
	catch (const skope_exception& e) {
		throw exception_etc(*this, pnode, e.getErrMsg());
	}
}

bool skope::checkcond(const AstNode* p)
{
	ConditionalOperation(p, p->child);
	if (!Sig.IsScalar())	throw exception_etc(*this, p, "Logical operation applied to a non-scalar.").raise();
	if (Sig.IsLogical())
		return Sig.logbuf[0];
	else
		return Sig.value() != 0.;
}

void skope::switch_case_handler(const AstNode* pnode)
{
	// if switch is first encountered, take care of switch_case_undefined
	multimap<CVar, AstNode*> more;
	vector<int> less = pEnv->udf[u.base].switch_case_undefined;
	AstNode* p = pnode->alt;
	for (; p && p->next && !less.empty(); p = p->next->alt)
	{
		if (p->line == less.front())
		{
			pLast = p; // to show the error line correctly inside the switch block
			more.emplace(Compute(p), p->next);
			auto fd = find(less.begin(), less.end(), p->line);
			less.erase(fd);
		}
	}
	pLast = pnode;
	//check if there's a duplicate
	//first is there a duplicate within more?
	for (auto mi = more.begin(); mi != more.end(); mi++)
	{
		auto va = (*mi).first;
		for (auto mj = next(mi, 1); mj != more.end(); mj++)
		{
			bool b1 = (*mi).first < (*mj).first;
			bool b2 = (*mj).first < (*mi).first;
			if (!b1 && !b2)
			{
				ostringstream out;
				out << "case duplates detected -- line " << (*mi).second->line - 1 << "and line " << (*mj).second->line - 1;
				throw exception_etc(*this, pnode, out.str());
			}
		}
	}
	// check more against pEnv->udf[u.base].switch_case
	for (auto ck = more.begin(); ck != more.end(); ck++)
	{
		auto fd = pEnv->udf[u.base].switch_case.find((*ck).first);
		if (fd != pEnv->udf[u.base].switch_case.end())
		{ // duplicate found. Is it in the same switch range?
			int a = (*ck).second->line;
			int endline = (*ck).second->line + 1;
			if (pnode->next)
				endline = pnode->next->line;
			if ((*ck).second->line > pnode->line && (*ck).second->line < endline)
			{
				ostringstream out;
				out << "case duplates detected -- line " << (*ck).second->line - 1 << "and line " << (*fd).second->line - 1;
				throw exception_etc(*this, pnode, out.str());
			}
		}
	}
	for (auto ck = more.begin(); ck != more.end(); ck++)
		pEnv->udf[u.base].switch_case.emplace((*ck).first, (*ck).second);
	pEnv->udf[u.base].switch_case_undefined.clear();
}
