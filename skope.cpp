#include <algorithm>
#include "skope.h"
#include "skope_exception.h"
#include "typecheck.h"
#include "utils.h"
#include <assert.h>

#ifndef _WINDOWS
#include <libgen.h>
#define MAX_PATH 256
#endif

int GetFileText(FILE* fp, string& strOut); // utils.cpp

//Application-wide global variables
vector<skope*> xscope;

map<string, vector<CVar*>> glovar_dummy; // need some kind of global definition
map<string, vector<CVar*>> CAstSigEnv::glovar = glovar_dummy;

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
	: Fs(fs), curLine(-1), inTryCatch(0)
{
	shutdown = false;
	if (fs < 0)	throw "Internal error: Fs must be greater than 1.";
	xff[N_BLOCK] = &CAstSigEnv::BLOCK;
	xff[T_FOR] = &CAstSigEnv::FOR;
	xff[T_IF] = &CAstSigEnv::IF;
	xff[T_WHILE] = &CAstSigEnv::WHILE;
	xff[T_TRY] = &CAstSigEnv::TRY;
	xff[T_CATCH] = &CAstSigEnv::CATCH;
	xff[T_CATCHBACK] = &CAstSigEnv::CATCH;
	xff[N_HOOK] = &CAstSigEnv::ID;
	xff[T_ID] = &CAstSigEnv::ID;
	xff[N_TSEQ] = &CAstSigEnv::TSEQ;
	xff[T_NUMBER] = &CAstSigEnv::NUMBER;
	xff[T_STRING] = &CAstSigEnv::STRING;
	xff[N_MATRIX] = &CAstSigEnv::MATRIX;
	xff[N_VECTOR] = &CAstSigEnv::VECTOR;
	xff[T_REPLICA] = &CAstSigEnv::REPLICA;
	xff[T_ENDPOINT] = &CAstSigEnv::ENDPOINT;
	xff['+'] = &CAstSigEnv::ARITH_PLUS;
	xff['-'] = &CAstSigEnv::ARITH_MINUS;
	xff['*'] = &CAstSigEnv::ARITH_MULT;
	xff['/'] = &CAstSigEnv::ARITH_DIV;
	xff[T_MATRIXMULT] = &CAstSigEnv::MATRIXMULT; // "**"
	xff['%'] = &CAstSigEnv::ARITH_MOD;
	xff[T_TRANSPOSE] = &CAstSigEnv::TRANSPOSE;
	xff[T_NEGATIVE] = &CAstSigEnv::NEGATIVE;
	xff[T_OP_SHIFT] = &CAstSigEnv::TIMESHIFT;
	xff[T_OP_CONCAT] = &CAstSigEnv::CONCAT;
	xff['<'] = &CAstSigEnv::LOGIC;
	xff['>'] = &CAstSigEnv::LOGIC;
	xff[T_LOGIC_OR] = &CAstSigEnv::LOGIC;
	xff[T_LOGIC_AND] = &CAstSigEnv::LOGIC;
	xff[T_LOGIC_LE] = &CAstSigEnv::LOGIC;
	xff[T_LOGIC_GE] = &CAstSigEnv::LOGIC;
	xff[T_LOGIC_EQ] = &CAstSigEnv::LOGIC;
	xff[T_LOGIC_NE] = &CAstSigEnv::LOGIC;
	xff[T_LOGIC_NOT] = &CAstSigEnv::LOGIC;
	xff['@'] = &CAstSigEnv::LEVELAT;
	xff[N_INITCELL] = &CAstSigEnv::INITCELL;
	xff[T_BREAK] = &CAstSigEnv::BREAK;
	xff[T_RETURN] = &CAstSigEnv::RETURN;

	type_arith_op.push_back('+');
	type_arith_op.push_back('-');
	type_arith_op.push_back('*');
	type_arith_op.push_back('/');
	type_arith_op.push_back('^');
	type_arith_op.push_back('@');
	type_arith_op.push_back('%');
	type_arith_op.push_back(T_POSITIVE);
	type_arith_op.push_back(T_NEGATIVE);
	type_arith_op.push_back(T_TRANSPOSE);
	type_arith_op.push_back(T_MATRIXMULT);
	type_arith_op.push_back(T_OP_SHIFT);
	type_arith_op.push_back(T_OP_CONCAT);
	type_condition.push_back('>');
	type_condition.push_back('<');
	type_condition.push_back(T_LOGIC_EQ);
	type_condition.push_back(T_LOGIC_NE);
	type_condition.push_back(T_LOGIC_LE);
	type_condition.push_back(T_LOGIC_GE);
	type_condition.push_back(T_LOGIC_NOT);
	type_condition.push_back(T_LOGIC_AND);
	type_condition.push_back(T_LOGIC_OR);
	type_blockflow.push_back(T_IF);
	type_blockflow.push_back(T_FOR);
	type_blockflow.push_back(T_WHILE);
	type_blockflow.push_back(T_TRY);
	type_blockflow.push_back(T_CATCH);
	type_blockflow.push_back(T_CATCHBACK);
	type_blockflow.push_back(N_BLOCK);
	
}

CAstSigEnv::~CAstSigEnv()
{

}

void skope::outputbinding_for_eval_lhs(const AstNode* plhs)
{
	assert(plhs->type == N_VECTOR);
	if (SigExt.empty())
	{
		AstNode* p = ((AstNode*)plhs->str)->alt;
		if (p->next)
			throw exception_etc(*this, p, "Too many output arguments.").raise();
	}
	else
	{
		// add something if you want to impose any other conditions example not allowing [a(id) b] = func(x,y)
	}
}

void skope::outputbinding(const AstNode* plhs)
{
	if (SigExt.empty())
	{
		AstNode* p = ((AstNode*)plhs->str)->alt;
		if (p->next)
			throw exception_etc(*this, p, "Too many output arguments.").raise();
		bind_psig(p, &Sig);
	}
	else
	{
		vector<unique_ptr<CVar*>>::iterator it = SigExt.begin();
		for (AstNode* p = ((AstNode*)plhs->str)->alt; p; p = p->next)
		{
			auto pp = *it->release();
			bind_psig(p, pp);
			if (it != SigExt.begin())
				delete pp; // most likely pp was created in _func() in _functions
			it++;
			if (it == SigExt.end() && p->next)
			{
				SigExt.clear(); // without this, a subsequent call to this function has invalid elements of SigExt *it->release() will crash.
				throw exception_etc(*this, p, "Too many output arguments.").raise();
			}
		}
	}
}

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
	}
	if (errmsg) {
		if (nodeAllocated) {
			yydeleteAstNode(out, 0);
			nodeAllocated = false;
			out = NULL;
		}
		emsg = errmsg;
		return NULL;
	}
	script = instr;
	return out;
}

vector<CVar*> skope::Compute()
{
	// There are many reasons to use this function as a Gateway function in the application, avoiding calling Compute(xtree) directly.
	// Call Compute(xtree) only if you know exactly what's going on. 11/8/2017 bjk
	vector<CVar*> res;
	Sig.cell.clear();
	Sig.strut.clear();
	Sig.struts.clear();
//	Sig.SetNextChan(NULL);
	Sig.functionEvalRes = false;
	//	pgo = NULL;
	lhs = NULL;
	if (!node) {
		res.push_back(&Sig);
		return res;
	}
	fBreak = false;
	if (node->type == N_BLOCK && (u.application == "xcom" || u.application == "auxlib")) {
		AstNode* p = node->next;
		while (p)
		{
			res.push_back(Compute(p));
			p = p->next;
			lhs = NULL; // to clear lhs from the last statement in the block 7/23/2019
		}
	}
	else
	{
		CVar* r;
		baselevel.push_back(level);
		r = process_statement(node);
		res.push_back(r);
		baselevel.pop_back();
	}
//	Tick1 = GetTickCount0();
	return res;
}

CVar* skope::Compute(const AstNode* pnode)
{
	if (pEnv->xff.find(pnode->type)== pEnv->xff.end())
	{
		ostringstream oss;
		oss << "Unknown node type, TYPE=" << pnode->type;
		throw exception_etc(*this, pnode, oss.str()).raise();
	}
	// Leaving these lines for future references--
	// CAstSigEnv::xflow_func fp = pEnv->xff[pnode->type];
	// return (pEnv->*fp)(this, pnode);
	return (pEnv->*pEnv->xff[pnode->type])(this, pnode);
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
	Sig.setsnap(0); // why is it necessary? 1/6/2022
	return &Sig;
}

static AstNode* get_next_parsible_node(AstNode* pn)
{
	if (pn->alt && pn->alt->type == N_CELL) pn = pn->alt;
	return pn->alt;
}

bool skope::IsConditional(const AstNode* pnode)
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

string skope::ComputeString(const AstNode* p)
{
	return (p->type == T_STRING) ? p->str : Compute(p)->str();
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
const AstNode* skope::searchtree(const AstNode* p, int type, int line2check)
{ // if there's a node with "type" in the tree, return that node
	// if line2check is specified (positive), that means search is good only for the same line, if it reaches the next line, it returns NULL
	// line2check was added to check replica on the RHS (to stop the search if it moves on to the next line), but it's a faulty approach because multiple statements can be on the same line
	// The issue was resolved to limit search to the case when LHS exists (assignment, not singular expression) regardless of line
	// so for now line2check is not needed any more, but keep this for the future 10/16/2022
	if (!p)
		return NULL;
	if (line2check > 0 && line2check != p->line)
		return NULL;
	if (p->type == N_VECTOR || p->type == N_MATRIX) // for [  ], search must continue through p->str
		return searchtree(((AstNode*)p->str)->alt, type, line2check);
	if (p->type == type) return p;
	if (p->child)
	{
		if (p->child->type == type) return p->child;
		else
		{
			if (searchtree(p->child, type, line2check)) return p->child;
			if (searchtree(p->alt, type, line2check)) return p->alt;
		}
	}
	if (p->alt)
	{
		if (p->alt->type == type) return p->alt;
		else
		{
			if (searchtree(p->alt, type, line2check)) return p->alt;
			if (searchtree(p->child, type, line2check)) return p->child;
		}
	}
	if (p->next)
	{
		if (p->next->type == type) return p->next;
		else return searchtree(p->next, type, line2check);
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
	else if (ptree->type == N_HOOK)
	{
		pres = GetGlobalVariable(ptree, ptree->str);
		if (!pres)
		{
			if (np.root->child && (!ptree->alt || ptree->alt->type == N_STRUCT))
				return NULL;
			throw exception_etc(*this, ptree, string("Undefined special variable: ") + ptree->str).raise();
		}
		np.psigBase = pres;
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
					PrepareAndCallUDF(ptree, &Sig);
					// if a function call follows N_ARGS, skip it for next_parsible_node
					np.psigBase = &Sig;
					if (ptree->alt && (ptree->alt->type == N_ARGS || IsConditional(ptree->alt)))
						ptree = ptree->alt; // to skip N_ARGS
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
			replica_prep(pres);
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
	AstNode* lastp = NULL;
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
		{
			pPrev = pn;
			pheadthisline = NULL;
		}
		else
		{
			pPrev = NULL;
			if (pn->type==T_ID && pn->alt && pn->alt->type==N_STRUCT) 
				pheadthisline = pn;
			else
				pheadthisline = NULL;
		}
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
		throw exception_etc(*this, pnode, "Unhandled path in skope::TID()").raise();
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
}

CVar* skope::GetGlobalVariable(const AstNode* pnode, const char* varname)
{
	auto it = pEnv->pseudo_vars.find(varname);
	if (it != pEnv->pseudo_vars.end())
	{
		vector<CVar> dummy;
		(*it).second.func(this, pnode, dummy);
	}
	else
	{
		map<string, vector<CVar*>>::iterator jt = CAstSigEnv::glovar.find(varname);
		if (jt == CAstSigEnv::glovar.end())
			return NULL;
		if ((*jt).second.front()->IsGO())
		{
			return NULL; // just for now 8/16/2019
		}
		else
		{
			Sig = *(*jt).second.front();
		}
	}
	return &Sig;
}

CVar* skope::GetVariable(const char* varname, CVar* pvar)
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

void skope::PrepareAndCallUDF(const AstNode* pCalling, CVar* pBase, CVar* pStaticVars)
{
	if (!pCalling->str)
		throw exception_etc(*this, pCalling, "p->str null pointer in PrepareAndCallUDF(p,...)").raise();
	// Check if the same udf is called during debugging... in that case Script shoudl be checked and handled...

	// Checking the number of input args used in the call
	size_t nargs = pCalling->type==N_STRUCT ? 1 : 0;
	if (pCalling->alt)
		for (auto pp = pCalling->alt->child; pp; pp = pp->next)
			nargs++;
	CAstSigEnv tempEnv(*pEnv);
	son.reset(new skope(&tempEnv));
	son->u = u;
	son->u.title = pCalling->str;
	son->u.debug.status = null;
	son->lhs = lhs;
	son->level = level + 1;
	son->baselevel.front() = baselevel.back();
	son->dad = this; // necessary when debugging exists with stepping (F10), the stepping can continue in tbe calling skope without breakpoints. --=>check 7/25
//	son->fpmsg = fpmsg;
	if (GOvars.find("?foc") != GOvars.end()) son->GOvars["?foc"] = GOvars["?foc"];
	if (GOvars.find("gcf") != GOvars.end())	son->GOvars["gcf"] = GOvars["gcf"];
	auto udftree = pEnv->udf.find(pCalling->str);
	if (udftree != pEnv->udf.end())
	{
		son->u.t_func = (*udftree).second.uxtree;
		son->u.t_func_base = son->u.t_func;
		son->u.base = son->u.t_func->str;
	}
	else
	{
		auto udftree2 = pEnv->udf.find(u.base); // if this is to be a local udf, base should be ready through a previous iteration.
		if (udftree2 == pEnv->udf.end())
			throw exception_etc(*this, pCalling, "PrepareCallUDF():supposed to be a local udf, but AstNode with that name not prepared").raise();
		son->u.t_func_base = (*udftree2).second.uxtree;
		son->u.base = u.base; // this way, base can maintain through iteration.
		son->u.t_func = (*pEnv->udf.find(u.base)).second.local[pCalling->str].uxtree;
	}
	son->node = son->u.t_func_base->child->next;
	son->node = son->u.t_func->child->next;
	//output argument string list
	son->u.argout.clear();
	AstNode* pOutParam = son->u.t_func->alt;
	ostringstream oss;
	if (pOutParam) {
		if (pOutParam->type == N_VECTOR)
		{
			if (pOutParam->str)
				pOutParam = ((AstNode*)pOutParam->str)->alt;
			else
				pOutParam = pOutParam->alt;
			for (AstNode* pf = pOutParam; pf; pf = pf->next)
				son->u.argout.push_back(pf->str);
		}
		else
		{
			oss << "TYPE=" << pOutParam->type << "PrepareCallUDF():UDF output should be alt and N_VECTOR";
			throw exception_etc(*this, pCalling, oss.str()).raise();
		}
	}
	AstNode* pf, * pa;	 // formal & actual parameter; pa is for this, pf is for son
	//Checking requested output arguments vs formal output arguments
	size_t nargout = 0;
	if (lhs)
	{
		if (lhs->type == N_VECTOR) {
			for (AstNode* tp = ((AstNode*)lhs->str)->alt; tp; tp = tp->next, nargout++) {}
		}
		else
			nargout = (int)son->u.argout.size(); // not applicable; as of oct 2022, all udf calls with output arg is N_VECTOR on LHS
		if (nargout > (int)son->u.argout.size()) {
			oss << son->u.t_func->str << ": max output arguments in the udf def: " << son->u.argout.size() << ", requested: " << nargout;
			throw exception_etc(*this, pCalling, oss.str()).raise();
		}
	}
	// input parameter binding
	pa = pCalling->alt;
	//If the line invoking the udf res = udf(arg1, arg2...), pa points to arg1 and so on
	son->u.nargin = 0;
	if (pa) {
		if (pa->type == N_ARGS)
			pa = pa->child;
		if (pCalling->type == N_STRUCT) // if it is a dot function call, make son->u.nargin 1
			son->u.nargin = 1;
	}
	//If the line invoking the udf res = var.udf(arg1, arg2...), binding of the first arg, var, is done separately via pBase. The rest, arg1, arg2, ..., are done below with pf->next
	pf = son->u.t_func->child->child;
	if (pBase && pCalling->type==N_STRUCT) { son->SetVar(pf->str, pBase); pf = pf->next; }
	//if this is for udf object function call, put that psigBase for pf->str and the rest from pa
	for (; pa && pf; pa = pa->next, pf = pf->next)
	{
		CVar tsig = Compute(pa);
		if (tsig.IsGO())
			son->SetVar(pf->str, pgo); // variable pf->str is created in the context of son
		else
			son->SetVar(pf->str, &tsig); // variable pf->str is created in the context of son
		son->u.nargin++;
	}
	if (nargs > son->u.nargin)
	{
		oss << "Excessive input arguments specified (expected " << son->u.nargin << ", given " << nargs << ") :";
		oss << son->u.t_func->str;
		throw exception_etc(*this, pCalling, oss.str()).raise();
	}
	//son->u.nargin is the number of args specified in udf
	if (u.debug.status == stepping_in) son->u.debug.status = stepping;
	xscope.push_back(son.get());
	//son->SetVar("_________",pStaticVars); // how can I add static variables here???
	son->CallUDF(pCalling, pBase, nargout);
	// output parameter binding
	vector<CVar*> holder;
	size_t cnt = 0;
	// output argument transfer from son to this
	float nargin = son->Vars["nargin"].value();
	float son_nargout = nargout;// son->Vars["nargout"].value();
	//Undefined output variables are defined as empty
	for (auto v : son->u.argout)
		if (son->Vars.find(v) == son->Vars.end() && (son->GOvars.find(v) == son->GOvars.end() || son->GOvars[v].front()->IsEmpty()))
			son->SetVar(v.c_str(), new CVar);
	//lhs is either NULL (if not specified), T_ID or N_VECTOR
	if (lhs)
	{
		outputbinding(pCalling, nargout);
	}
	else // no output parameter specified. --> first formal output arg goes to ans
	{
		if (son->u.argout.empty())
			Sig.Reset();
		else
		{
			if (son->Vars.find(son->u.argout.front()) != son->Vars.end())
				Sig = son->Vars[son->u.argout.front()];
			else
			{
//				if (son->GOvars[son->u.argout.front()].size() > 1)
//					Sig = *(pgo = MakeGOContainer(son->GOvars[son->u.argout.front()]));
//				else if (son->GOvars[son->u.argout.front()].size() == 1)
//					Sig = *(pgo = son->GOvars[son->u.argout.front()].front());
//				else
					Sig = CVar();
			}
		}
	}
	if ((son->u.debug.status == stepping || son->u.debug.status == continuing) && u.debug.status == null)
	{ // no b.p set in the main udf, but in these conditions, as the local udf is finishing, the stepping should continue in the main udf, or debug.status should be set progress, so that the debugger would be properly exiting as it finishes up in CallUDF()
		u.debug.GUI_running = true;
		if (son->u.debug.status == stepping) // b.p. set in a local udf
			u.debug.status = stepping;
		else // b.p. set in other udf
			u.debug.status = progress;
	}
	if (son->GOvars.find("?foc") != son->GOvars.end()) GOvars["?foc"] = son->GOvars["?foc"];
	if (son->GOvars.find("gcf") != son->GOvars.end()) GOvars["gcf"] = son->GOvars["gcf"];
	u.pLastRead = NULL;
	if (pgo) pgo->functionEvalRes = true;
	Sig.functionEvalRes = true;
	xscope.pop_back(); // move here????? to make purgatory work...
}

void skope::CallUDF(const AstNode* pnode4UDFcalled, CVar* pBase, size_t nargout_requested)
{
	// Returns the number of output arguments requested in the call
	// 
	// t_func: the T_FUNCTION node pointer for the current UDF call, created after ReadUDF ("formal" context--i.e., how the udf file was read with variables used in the file)
	// pOutParam: AstNode for formal output variable (or LHS), just used inside of this function.
	// Output parameter dispatching (sending the output back to the calling worksapce) is done with pOutParam and lhs at the bottom.

	// u.debug.status is set when debug key is pressed (F5, F10, F11), prior to this call.
	// For an initial entry UDF, u.debug.status should be null
	CVar nargin((float)u.nargin);
	CVar nargout((float)nargout_requested);
	SetVar("nargin", &nargin);
	SetVar("nargout", &nargout);
	// If the udf has multiple statements, p->type is N_BLOCK), then go deeper
	// If it has a single statement, take it from there.
	AstNode* pFirst = u.t_func->child->next;
	if (pFirst->type == N_BLOCK)	pFirst = pFirst->next;
	//Get the range of lines for the current udf
	u.currentLine = pFirst->line;
	AstNode* p;
	int line2;
	for (p = pFirst; p; p = p->next)
	{
		line2 = p->line;
		if (!p->next) // if the node is T_FOR, T_WHILE or T_IF, p-next is NULL is it should continue through p->child
		{
			if (p->type == T_FOR || p->type == T_WHILE)
				p = p->alt;
			else if (p->type == T_IF)
			{
				if (p->alt)
					p = p->alt;
				else
					p = p->child;
			}
		}
	}
	//probably needed to enter a new, external udf (if not, may skip)
//	if (pEnv->udf[u.base].newrecruit)
//		fpmsg.UpdateDebuggerGUI(this, refresh, -1); // shouldn't this be entering instead of refresh? It seems that way at least to F11 an not-yet-opened udf 10/16/2018. But.. it crashes. It must not have been worked on thoroughly...
														//if this is auxconscript front astsig, enter call fpmsg.UpdateDebuggerGUI()
//	if (u.debug.status == stepping)
//		/*u.debug.GUI_running = true, */ fpmsg.UpdateDebuggerGUI(this, entering, -1);
//	else
	{ // probably entrance udf... First, check if current udfname (i.e., Script) is found in DebugBreaks
		// if so, mark u.debug.status as progress and set next breakpoint
		// and call debug_GUI
		vector<int> breakpoint = pEnv->udf[u.base].DebugBreaks;
		for (vector<int>::iterator it = breakpoint.begin(); it != breakpoint.end(); it++)
		{
			if (*it < u.currentLine) continue;
			if (*it <= line2) {
				u.debug.status = progress; u.nextBreakPoint = *it;
				//u.debug.GUI_running = true, fpmsg.UpdateDebuggerGUI(this, entering, -1);
				break;
			}
		}
	}
	p = pFirst;
	while (p)
	{
		pLast = p;
		// T_IF, T_WHILE, T_FOR are checked here to break right at the beginning of the loop
		u.currentLine = p->line;
//		if (p->type == T_ID || p->type == T_FOR || p->type == T_IF || p->type == T_WHILE || p->type == N_IDLIST || p->type == N_VECTOR)
//			hold_at_break_point(p);
		process_statement(p);
		//		pgo = NULL; // without this, go lingers on the next line
		//		Sig.Reset(1); // without this, fs=3 lingers on the next line
		if (fExit) break;
		p = p->next;
	}
	if (u.debug.status != null)
	{
		//		currentLine = -1; // to be used in CDebugDlg::ProcessCustomDraw() (re-drawing with default background color)... not necessary for u.debug.status==stepping (because currentLine has been updated stepping) but won't hurt
		if (u.debug.GUI_running == true)
		{
			// send to purgatory and standby for another debugging key action, if dad is the base scope
			//if (dad == xscope.front() && u.debug.status == stepping)
			//{
			//	fpmsg.UpdateDebuggerGUI(this, purgatory, -1);
			//	fpmsg.HoldAtBreakPoint(this, pLast);
			//}
			u.currentLine = -1;
			u.debug.inPurgatory = false; // necessary to reset the color of debug window listview.
			//when exiting from a inside udf (whether local or not) to a calling udf with F10 or F11, the calling udf now should have stepping.
//			fpmsg.UpdateDebuggerGUI(this, exiting, -1);
		}
		if (xscope.size() > 2) // pvevast hasn't popped yet... This means son is secondary udf (either a local udf or other udf called by the primary udf)
		{//why is this necessary? 10/19/2018----yes this is...2/16/2019
//			if (u.debug.status == stepping && fpmsg.IsCurrentUDFOnDebuggerDeck && !fpmsg.IsCurrentUDFOnDebuggerDeck(Script.c_str()))
//				fpmsg.UpdateDebuggerGUI(dad, entering, -1);
		}
	}
}

multimap<CVar, AstNode*> skope::register_switch_cvars(const AstNode* pnode, vector<int>& undefined)
{
	multimap<CVar, AstNode*> out;
	// This evaluates the udf and computes all case keys for switch..case... loops.
	AstNode* p, * _pnode = (AstNode*)pnode;
	for (; _pnode; _pnode = _pnode->next)
	{
		if (_pnode->type == T_SWITCH)
		{
			for (p = _pnode->alt; p && p->next; p = p->next->alt)
			{
				if (p->type == N_ARGS)
				{
					for (AstNode* pa = p->child; pa; pa = pa->next)
					{
						const CVar tsig2 = Compute(pa);
						out.emplace(tsig2, p->next);
					}
				}
				else if (p->type == T_OTHERWISE)
					break;
				else
				{
					try {
						CVar tsig2 = Compute(p);
						out.emplace(tsig2, p->next);
					}
					catch (const skope_exception& e)
					{
						string emsg = e.outstr;
						string estr = e.tidstr;
						int line = e.line;
						line++; line--;
						undefined.push_back(e.line);
					}
				}
			}
		}
	}
	return  out;
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
	if (GetFileText(auxfile, filecontent) <= 0)
	{// File reading error or empty file
		emsg = string("Cannot read specified udf or script file ") + udf_filename;
		fclose(auxfile);
		return NULL;
	}
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

AstNode* skope::RegisterUDF(const AstNode* p, const char* fullfilename, const string& filecontent)
{
	//Deregistering takes place during cleaning out of pEnv i.e., ~CAstSigEnv()
	string udf_filename = get_name_only(fullfilename);
	AstNode* pnode4Func = (AstNode*)((p->type == N_BLOCK) ? p->next : p);
	string namefrompnode = pnode4Func->str;
	transform(namefrompnode.begin(), namefrompnode.end(), namefrompnode.begin(), ::tolower);
	if (namefrompnode != udf_filename) // pnode4Func is NULL for local function call and it will crash.....8/1/
	{
		auto it = pEnv->udf.find(p->str);
		if (it != pEnv->udf.end())	pEnv->udf.erase(pEnv->udf.find(p->str));
		string emsg = string(udf_filename + " vs ") + pnode4Func->str;
		throw exception_etc(*this, p, string("inconsistent function name: ") + emsg).raise();
	}
	vector<int> undefined;
	auto vv = register_switch_cvars(pnode4Func->child->next->next, undefined);

	pEnv->udf[udf_filename].uxtree = pnode4Func;
	pEnv->udf[udf_filename].fullname = fullfilename;
	pEnv->udf[udf_filename].content = filecontent.c_str();
	pEnv->udf[udf_filename].switch_case = vv;
	pEnv->udf[udf_filename].switch_case_undefined = undefined;

	for (AstNode* pp = pnode4Func->next; pp; pp = pp->next)
	{// if one or more local functions exists
		UDF loc;
		loc.uxtree = pp;
		namefrompnode = pp->str;
		transform(namefrompnode.begin(), namefrompnode.end(), namefrompnode.begin(), ::tolower);
		loc.fullname = fullfilename;
		loc.content = "see base function for content";
		pEnv->udf[udf_filename].local[namefrompnode] = loc;
	}
	return pnode4Func;
}

#if !defined(MAX_PATH)
#define MAX_PATH          260
#endif

FILE* skope::fopen_from_path(const string& fname, const string& ext, string& fullfilename)
{ // in in out
	char extension[MAX_PATH] = {0};
#ifdef _WINDOWS
	char drive[64], dir[MAX_PATH], filename[MAX_PATH];
	_splitpath(fname.c_str(), drive, dir, filename, extension);
#else
	strcpy(extension, get_ext_only(fname).c_str());
#endif
	string _fname(fname), _ext(ext);
	transform(_ext.begin(), _ext.end(), _ext.begin(), ::tolower);
	char fopenopt[4];
	if (_ext == "txt" || _ext == "aux") strcpy(fopenopt, "rt");
	else			strcpy(fopenopt, "rb");
	size_t pdot = fname.rfind('.');
	if ((pdot == fname.npos || pdot < fname.length() - 4) && !extension[0])
		_fname += "." + ext;

#ifdef _WINDOWS
	if (drive[0] + dir[0] > 0)
#else
	if (fname.find('/') != string::npos)
#endif
	{ // if path is included in fname, open with fname
		fullfilename = _fname;
		return fopen(_fname.c_str(), fopenopt);
	}
	else {
		string ofname(_fname);
		// search fname in AuxPath, beginning with current working directory
		fullfilename = pEnv->AppPath + _fname;
		FILE* file = fopen(fullfilename.c_str(), fopenopt);
		if (!file)
		{
			for (auto path : pEnv->AuxPath)
			{
				fullfilename = path + _fname;
				file = fopen(fullfilename.c_str(), fopenopt);
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
	qscope.script = udfname;
	transform(qscope.script.begin(), qscope.script.end(), qscope.script.begin(), ::tolower);
	udf[qscope.script].newrecruit = true;
	auto udftree = qscope.makenodes(filecontent);
	if (udftree)
	{
		qscope.node = udftree;
		pout = qscope.RegisterUDF(qscope.node, fullpath.c_str(), filecontent);
		// The following should be after all the throws. Otherwise, the UDF AST will be dangling.
		// To prevent de-allocation of the AST of the UDF when qscope goes out of skope.
		if (qscope.node->type == N_BLOCK)
			qscope.node->next = NULL;
		else
			qscope.node = NULL;
		return pout;
	}
	else
		emsg = qscope.emsg;
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
	ends = src->ends;
	level = src->level;
	pLast = src->pLast;
	//pgo = src->pgo;
	//Script = src->Script;
	//pLast = src->pLast;
	//inTryCatch = src->inTryCatch;
}


void skope::init()
{
	node = NULL;
	statusMsg = "";
//	fAllocatedAst = false;
	pLast = NULL;
	son = NULL;
	dad = NULL;
	lhs = NULL;

//	FsFixed = false;
	pgo = NULL;
	Tick0 = 1;
	level = 1;
	baselevel.push_back(level);
}

skope& skope::SetVar(const char* name, CVar* prhs, CVar* pBase)
// To do--chanage CVar *prhs to const CVar &tsig and make sure the second arg is the const, target signal to use
//to do so, improve the case of prhs->GetFs() == 3, so that prhs is not changed, let's think about it how.
//11/6/2019
{// NULL pBase --> name will be the variable in the workspace.
 //non-NULL pBase --> pBase is a class variable. name will be a member variable under pBase.
	if (!pBase) // top skope
	{
		map<string, CVar>::iterator it = Vars.find(name);
		map<string, vector<CVar*>>::iterator jt = GOvars.find(name);
		if (prhs->IsGO()) // name and prhs should be fed to GOvars
		{
			//gca, gcf shouldn't be "push_backed" but instead replaced.
			if (!strcmp(name, "gca") || !strcmp(name, "gcf"))
				GOvars[name].clear();
			if (jt != GOvars.end())  GOvars.erase(jt);
			if (prhs->GetFs() == 3)
			{
				if (prhs->nSamples == 1)
				{
					prhs = (CVar*)(INT_PTR)prhs->value();
					GOvars[name].push_back(prhs);
				}
				else
					for (unsigned int k = 0; k < prhs->nSamples; k++)
						GOvars[name].push_back((CVar*)(INT_PTR)prhs->buf[k]);
			}
			else
				GOvars[name].push_back(prhs);
			if (it != Vars.end()) Vars.erase(it);
		}
		else // name and prhs should be fed to Var
		{
			Vars[name] = *prhs; // if any part of prhs is ghost, it should make it real (because once it loses the scope, references on LHS will be corrupted) 1/6/2022
			if (jt != GOvars.end())  GOvars.erase(jt);
		}
	}
	else
	{
		if (prhs->IsGO()) // name and prhs should be fed to struts
		{
			// Previous one should be cleared.
			// I wonder if this clear() would cause an issue
			// What has the SetVar convention been for GO 1/3/2021
			pBase->struts[name].clear();
			pBase->struts[name].push_back(prhs);
			auto it = pBase->strut.find(name);
			if (it != pBase->strut.end()) pBase->strut.clear();
		}
		else // name and prhs should be fed to strut
		{
			pBase->strut[name] = *prhs;
			auto jt = pBase->struts.find(name);
			if (jt != pBase->struts.end())  pBase->struts[name].clear();
		}
	}
	return *this;
}

string CAstSigEnv::path_delimited_semicolon()
{
	string out;
	for (auto s : AuxPath)
		out += s + ';';
	return out;
}

void CAstSigEnv::AddPath(string path)
{
	trim(path, string("\r \n\t"));
	if (!path.empty())
	{
		transform(path.begin(), path.end(), path.begin(), ::tolower);
		if (path.back() != '\\') path += '\\';
		auto fd = find(AuxPath.begin(), AuxPath.end(), path);
		if (fd == AuxPath.end())
			AuxPath.push_back(path);
	}
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

unsigned long skope::tic()
{ // tic toc gives away time elapsed in milliseconds (approximately)
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	Tick0 = (intmax_t)ts.tv_sec * 1000;
	double ms = ts.tv_nsec / (double)1000000.;
	Tick0 += (int)ms;
	return Tick0;
}

unsigned long skope::toc(const AstNode* p)
{
	if (Tick0 == 1)
		throw exception_etc(*this, p, "toc called without tic").raise();
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	Tick1 = (intmax_t)ts.tv_sec * 1000;
	double ms = ts.tv_nsec / (double)1000000.;
	Tick1 += (int)ms;
	Tick1 -= Tick0;
	return Tick1;
}


//Maybe move to AstSigEnv?? 11/19/2021
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
		out.bufType = 'R';
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
	if (!(a & TYPEBIT_CELL) && !(b & TYPEBIT_CELL))
	{
		if (b > 0 && a >> 2 != b >> 2)
			throw exception_etc(*this, p, "Different object type between LHS and RHS. Can't concatenate.").raise();
		//Check rejection conditions
		if (tsig.nSamples * Sig.nSamples > 0) // if either is empty, no rejection
		{
			if (tsig.nGroups != Sig.nGroups && tsig.Len() != Sig.Len())
				throw exception_etc(*this, p->next, "To concatenate, the second operand must have the same number of elements or the same number of groups (i.e., rows) ").raise();
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
				memcpy(Sig.buf + len1 * k, Sig.buf + len0 * k, sizeof(float) * len0);
				memcpy(Sig.buf + len1 * k + len0, tsig.buf + lent * k, sizeof(float) * lent);
			}
		}
		else
		{ // Sig and tsig have both row and column counts, concatenation is done here... at the end of row--making more rows withthe same column counts (not row-wise concatenating)
			if (Sig.nGroups > 1) Sig.nGroups += tsig.nGroups;
			Sig += &tsig;
			Sig.MergeChains();
		}
	}
	else
	{
		if (a & TYPEBIT_CELL || !(b & TYPEBIT_CELL))
			throw exception_etc(*this, p, "2nd op is cell; 1st op is not.").raise();
		if (b & TYPEBIT_CELL)
		{
			if (a & TYPEBIT_CELL)
			{
				for (size_t k = 0; k < tsig.cell.size(); k++)
					Sig.cell.push_back(tsig.cell[(int)k]);
			}
			else
				Sig.cell.push_back(tsig);
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
	uint16_t t1, t2;
	// Logical operations are allowed for a struct or cell object based on the head obj
	// TODO--other operations (arithmetic, @, >> etc) should be the same... allow struct or cell based on the head obj
	switch (pnode->type)
	{
	case '<':
	case '>':
	case T_LOGIC_LE:
	case T_LOGIC_GE:
	case T_LOGIC_EQ:
	case T_LOGIC_NE:
		rsig = Compute(p->next);
		Compute(p);
		// if at least one of the operands is zero-length, throw
		if (Sig.nSamples == 0 || rsig.nSamples == 0)
			throw exception_etc(*this, pnode, "Logical operation not applicable to a null obj.").raise();
		t1 = rsig.type();
		t2 = Sig.type();
		// If one is string-plus, the other one must be also string-plus
		if (ISSTRINGG(t1) ^ ISSTRINGG(t2))
			throw exception_etc(*this, pnode, "String obj is compared with non-string object.").raise();
		// If one is bool-plus, the other one must be also bool-plus
		if (ISBOOLG(t1) ^ ISBOOLG(t2))
			throw exception_etc(*this, pnode, "Boolean obj is compared with non-Boolean object.").raise();
		//		blockCellStruct2(*this, pnode, rsig);
		pgo = NULL;
		Sig.LogOp(rsig, pnode->type);
		if (ISSTRINGG(t1)) { // for string operation, Sig still has fs of 2. Update it with fs=1. SetFs() won't work because it sets bufBlockSize back to the real size
			CVar out;
			out.MakeLogical();
			out.UpdateBuffer(Sig.nSamples - 1);
			memmove(out.logbuf, Sig.logbuf, out.nSamples);
			Sig = out;
		}
		break;
	case T_LOGIC_NOT:
		// bufType of a or b should be 'R' 'B' or 'L'
		Sig = Compute(p);
		if (Sig.bufType!='R' && Sig.bufType != 'L' && Sig.bufType != 'B')
			throw exception_etc(*this, pnode, "Logical reverse can be only applied to a real, byte or logical object.").raise();
		if (Sig.nSamples == 0)
			throw exception_etc(*this, pnode, "Logical operation not applicable to a null obj.").raise();
		Sig.MakeLogical();
		Sig.LogOp(rsig, pnode->type); // rsig is a dummy for func signature.
		break;
	case T_LOGIC_AND:
	case T_LOGIC_OR:
		// a && b, a || b --> if either a or b is a scalar, the output length is the length of the other one
		// if none of them is a scalar, the output length is the shorter of a or b
		// bufType of a or b should be 'R' 'B' or 'L'
		rsig = Compute(p->next);
		Compute(p);
		if ((Sig.bufType != 'R' && Sig.bufType != 'L' && Sig.bufType != 'B') ||
			(rsig.bufType != 'R' && rsig.bufType != 'L' && rsig.bufType != 'B'))
			throw exception_etc(*this, pnode, "Logical reverse can be only applied to a real, byte or logical object.").raise();
		if (Sig.nSamples == 0 || rsig.nSamples == 0)
			throw exception_etc(*this, pnode, "Logical operation not applicable to a null obj.").raise();
		Sig.LogOp(rsig, pnode->type);
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
	//At this point, need to clear struct or cell from Sig
	Sig.strut.clear();
	Sig.cell.clear();
	return TID(pnode->alt, NULL, &Sig);
}

CVar* skope::SetLevel(const AstNode* pnode, AstNode* p)
{
	CVar sigRMS, refRMS, dB = Compute(p->next);
	// if tsig is scalar -- apply it across the board of Sig
	// if tsig is two-element vector -- if Sig is stereo, apply each; if not, take only the first vector and case 1
	// if tsig is stereo-scalar, apply the scalar to each L and R of Sig. If Sig is mono, ignore tsig.next
	// if tsig is tseq, it must have the same chain and next structure (exception otherwise)
	auto tp = dB.type();
	if (p->type == '@')
	{// trinary
		// types for scalar tsq TYPEBIT_TEMPO_ONE+1, TYPEBIT_TEMPO_CHAINS+1, or TYPEBIT_MULTICHANS+TYPEBIT_TEMPO_ONE+1, TYPEBIT_MULTICHANS+TYPEBIT_TEMPO_CHAINS+1
		// mask the first two hex digits (and forget about TYPEBIT_MULTICHANS) then compare
		if ( (tp & 0x00FF) == TYPEBIT_TEMPO_ONE + 1 || (tp & 0x00FF) == TYPEBIT_TEMPO_CHAINS + 1) // scalar time sequence
			throw exception_etc(*this, pnode, "sig @ ref @ level ---- level cannot be time sequence.").raise();
		refRMS <= Compute(p->child->next);
		auto tp2 = refRMS.type();
		if (!ISAUDIO(tp2))
			throw exception_etc(*this, pnode, "sig @ ref @ level ---- ref must be an audio signal.").raise();
		refRMS.RMS(); // this should be called here, once another Compute is called refRMS.buf won't be valid
		Compute(p->child);
		sigRMS <= Sig;
		sigRMS.RMS();
		sigRMS -= refRMS;
	}
	else
	{
		Sig = Compute(p);
		sigRMS <= Sig;
		if ((tp & 0x00FF) == TYPEBIT_TEMPO_ONE + 1 || (tp & 0x00FF) == TYPEBIT_TEMPO_CHAINS + 1) // scalar time sequence
		{// for tseq leveling, % operator is used.
			for (CTimeSeries* p = &dB; p; p = p->chain)
				p->buf[0] = powf(10, p->buf[0] / 20.);
			for (CTimeSeries* p = dB.next; p; p = p->chain)
				p->buf[0] = powf(10, p->buf[0] / 20.);
			Sig % dB;
			return &Sig;
		}
		// sig @ level
		// if sig is chained, level is applied across all chains
		// i.e., currently there's no simple way to specify chain-specific levels 6/17/2020

		// A known hole in the logic here---if dB is stereo but first channel is scalar
		// and next is chained, or vice versa, this will not work
		// Currently it's difficult to define dB that way (maybe possible, but I can't think about an easy way)
		// A new, simpler and intuitive way to define T_SEQ should be in place
		// before fixing this hole.  10/7/2019

		//Being fixed.... but need further checking
		//6/17/2020
		if (dB.chain || (dB.next && dB.next->chain))
			sigRMS = sigRMS.evoke_getval(&CSignal::RMS);
		else
			sigRMS.RMS(); // this should be called before another Compute is called (then, refRMS.buf won't be valid)
	}
	//Reject dB if empty, if string, or if bool
	if (dB.IsEmpty() || dB.IsString() || dB.IsBool())
		throw exception_etc(*this, pnode, "Target_RMS_dB after @ must be a real value.").raise();
	if (dB.nSamples > 1 && !dB.next)
		dB.SetNextChan(CSignals(dB.buf[1]));
	sigRMS = dB - sigRMS;
	Sig | sigRMS;
	return &Sig;
}

float skope::find_endpoint(const AstNode* p, const CVar &var)
{  // p is the node the indexing starts (e.g., child of N_ARGS... wait is it also child of conditional p?
	if (p->next) // first index in 2D
		return (float)var.nGroups;
	else
		return (float)var.nSamples;
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

vector<string> skope::ClearVar(const char *var, CVar *psigBase)
{
	vector<string> out;
	vector<string> vars;
	if (!psigBase)
	{
		auto it = Vars.find(var);
		if (it != Vars.end())
		{
			out.push_back((*it).first);
			Vars.erase(it);
		}
	}
	else
	{
		auto it = psigBase->strut.find(var);
		if (it != psigBase->strut.end())
		{
			out.push_back((*it).first);
			psigBase->strut.erase(it);
		}
		else
		{
			auto it = psigBase->struts.find(var);
			if (it != psigBase->struts.end())
			{
				out.push_back((*it).first);
				psigBase->struts.erase(it);
			}
		}
	}
	return out;
}

UDF& UDF::operator=(const UDF& rhs)
{
	if (this != &rhs)
	{
		uxtree = rhs.uxtree;
		fullname = rhs.fullname;
		content = rhs.content;
		DebugBreaks = rhs.DebugBreaks;
		newrecruit = rhs.newrecruit;
	}
	return *this;
}

CAstSigEnv& CAstSigEnv::operator=(const CAstSigEnv& rhs)
{
	if (this != &rhs)
	{
		Fs = rhs.Fs;
		AuxPath = rhs.AuxPath;
		udf = rhs.udf;
		shutdown = rhs.shutdown;
		glovar = rhs.glovar;
	}
	return *this;
}
