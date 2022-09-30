#pragma once
#include <cstring>
#include <memory> // unique_ptr
#include <set>
#include "aux_classes.h"
#include "psycon.yacc.h"
#include "psycon.tab.h"

#include <algorithm>
#define PI	3.141592f

#ifdef _WIN32
#include <windows.h>
#else
typedef uint64_t INT_PTR;
#endif

using namespace std;

class skope;
class CNodeProbe;

const AstNode* get_first_arg(const AstNode* pnode, bool staticfunc = false); // from support.cpp
const AstNode* get_second_arg(const AstNode* , bool); // from support.cpp

enum DEBUG_STATUS
{
	null = -1,
	entering,
	progress,
	stepping,
	stepping_in,
	continuing,
	exiting,
	cleanup,
	aborting,
	purgatory,
	refresh,
	typed_line,
};

class UDF
{
public:
	AstNode* uxtree; // the syntax tree for the content of the UDF; begining with T_FUNCTION
	string fullname;
	string content;
	vector<int> DebugBreaks;
	map<string, UDF> local;
	bool newrecruit;
	UDF& operator=(const UDF& rhs);
	UDF() { uxtree = NULL; newrecruit = false; };
	multimap<CVar, AstNode*> switch_case; // why multimap instead of map? Because the key for case may appear in multiple switch blocks
	vector<int> switch_case_undefined;
	virtual ~UDF() {};
};

class Cfunction
{
public:
	Cfunction() {
		funcsignature = ""; 
		alwaysstatic = false;
		narg1 = 0;
		narg2 = 0;
	};
	virtual ~Cfunction() {};
	string funcsignature;
	vector<string> desc_arg_req;
	vector<string> desc_arg_opt;
	bool alwaysstatic; // true means the function cannot be called as a member function
	int narg1, narg2; // narg1 is min number of args; narg2 is max number of args
	vector<CVar> defaultarg;
	vector<set<uint16_t>> allowed_arg_types;
	void(*func)(skope*, const AstNode*, const vector<CVar>&);
};
class CAstSigEnv
{
public:
	static string AppPath;
	static map<string, vector<CVar*>> glovar;
	static map<string, Cfunction> pseudo_vars;
	map<string, UDF> udf;
	int Fs;
	int curLine; // used for control F10
	vector<string> AuxPath;
	bool shutdown;
	void InitBuiltInFunctions();
	bool IsValidBuiltin(const string& funcname);
	void InitErrorCodes();
	string path_delimited_semicolon();
	vector<string> InitBuiltInFunctionsExt(const vector<string>& externalModules);
	multimap<string, Cfunction> builtin;

	CAstSigEnv(const int fs = 1);
	virtual ~CAstSigEnv();
	CAstSigEnv& operator=(const CAstSigEnv& rhs);
	void AddPath(string path);
	AstNode* checkout_udf(const string& udf_filename, const string& filecontent);
	AstNode* checkin_udf(const string& udf_filename, const string& fullpath, const string& filecontent, string& emsg);
};
class CDebugStatus
{
public:
	DEBUG_STATUS status;
	bool GUI_running;
	bool inPurgatory;
	bool local;
	CDebugStatus() { status = null;  GUI_running = inPurgatory = local = false; };
	virtual ~CDebugStatus() {};
};

class CUDF
{
public:
	string title;
	string base;
	AstNode* t_func; // T_FUNCTION node
	AstNode* t_func_base; // T_FUNCTION node for base udf
	vector<string> argout; // formal output argument list; to be filled out in PrepareAndCallUDF()
	int nextBreakPoint;
	int currentLine;
	int nargin, nargout;
	map<string, CVar*> static_vars;
	CDebugStatus debug;
//	map<HWND, RECT> rt2validate;
	string application;
	bool repaint;
	CUDF() { nextBreakPoint = currentLine = -1; pLastRead = NULL; repaint = false; };
	virtual ~CUDF() {};
	AstNode* pLastRead; //used for isthisUDFscope only, to mark the last pnode processed in 
};

class skope
{
public:
	static const AstNode* find_parent(const AstNode* p, const AstNode* pME);
	static AstNode* goto_line(const AstNode* pnode, int line);
	static bool IsLooping(const AstNode* p);

	skope(string instr = "");
	skope(CAstSigEnv* env);
	skope(const skope* src);
	virtual ~skope();
	void init();
	unsigned long tic();
	unsigned long toc(const AstNode* p);
	void setstring(const string& instr) { script = instr; };
	AstNode* makenodes(const string& instr);
	CVar* Compute(const AstNode* pnode);
	vector<CVar*> Compute();
	CVar* TID(AstNode* pnode, AstNode* pRHS, CVar* psig = NULL);
	skope& SetVar(const char* name, CVar* psig, CVar* pBase = NULL);
	AstNode* read_node(CNodeProbe& np, AstNode* ptree, AstNode* ppar, bool& RHSpresent);
	AstNode* read_nodes(CNodeProbe& np, bool bRHS = false);
	void throw_LHS_lvalue(const AstNode* pn, bool udf);
	AstNode* searchtree(const AstNode* pTarget, AstNode* pStart);
	AstNode* searchtree(AstNode* p, int type);
	multimap<CVar, AstNode*> register_switch_cvars(const AstNode* pnode, vector<int>& undefined);
	AstNode* ReadUDF(string& emsg, const char* udf_filename);
	AstNode* RegisterUDF(const AstNode* p, const char* fullfilename, const string& filecontent);
	CVar* GetGlobalVariable(const AstNode* pnode, const char* varname);
	CVar* GetVariable(const char* varname, CVar* pvar = NULL);
	bool PrepareAndCallUDF(const AstNode* pCalling, CVar* pBase, CVar* pStaticVars = NULL);
	size_t CallUDF(const AstNode* pnode4UDFcalled, CVar* pBase);
	FILE* fopen_from_path(const string& fname, const string& ext, string& fullfilename);
	bool builtin_func_call(CNodeProbe& diggy, AstNode* p);
	void HandleAuxFunctions(const AstNode* pnode, AstNode* pRoot = NULL);
	CVar* pseudoVar(const AstNode* pnode, AstNode* p, CSignals* pout);
	CVar* TSeq(const AstNode* pnode, AstNode* p);
	bool HandlePseudoVar(const AstNode* pnode);
	void HandleMathFunc(string& fname, const body& arg);
	vector<float> gettimepoints(const AstNode* pnode, AstNode* p);
	CVar* NodeVector(const AstNode* pn);
	CVar* NodeMatrix(const AstNode* pnode);
	CVar* Dot(AstNode* p);
	void Transpose(const AstNode* pnode, AstNode* p);
	CVar* Try_here(const AstNode* pnode, AstNode* p);
	void Concatenate(const AstNode* pnode, AstNode* p);
	CVar* ConditionalOperation(const AstNode* pnode, AstNode* p);
	CVar* SetLevel(const AstNode* pnode, AstNode* p);
	float find_endpoint(const AstNode* p, const CVar& var);
	void interweave_indices(CVar& isig, CVar& isig2, unsigned int len);
	void index_array_satisfying_condition(CVar& isig);
	CVar* InitCell(const AstNode* pnode, AstNode* p);
	bool checkcond(const AstNode* p);
	void switch_case_handler(const AstNode* pnode);
	int GetFs(void) { return pEnv->Fs; }
	void outputbinding_for_eval_lhs(const AstNode* pnode);
	void outputbinding(const AstNode* pnode);
	void outputbinding(const AstNode* pnode, size_t nArgout);
	void bind_psig(AstNode* pn, CVar* psig);
	string makefullfile(const string& fname, string extension = "");
	string ComputeString(const AstNode* p);
	vector<string> ClearVar(const char* var, CVar* psigBase=NULL);
	void replica_prep(CVar* psig);

	string script;
	string emsg;
	CVar Sig; // placeholder for the output of Compute(); used in various ways
	vector<unique_ptr<CVar*>> SigExt; // placeholder for multiple output arguments
	AstNode* node;
	CVar* pgo; // pointer, not a copy, of the last computed object; used for graffy functions
	int level;
	vector<int> baselevel;
	map<string, CVar> Vars;
	map<string, vector<CVar*>> GOvars;
	AstNode* lhs;
	CAstSigEnv* pEnv;
	CUDF u;
	int inTryCatch;
	CVar replica;
	unique_ptr<skope> son;
	skope* dad;
	const AstNode* pLast;
	string statusMsg; // to display any message during processing inside of AstSig.cpp in the application
	unsigned long Tick0, Tick1;
	vector<float> ends;
	CVar* process_block(const AstNode* pnode);
	CVar* process_statement(const AstNode* pnode);
private:
	bool done;
	bool nodeAllocated;
	bool fBreak;
	bool fExit;
	vector<CVar> make_check_args(const AstNode* pnode, const Cfunction& func, void* pskope_exception); // using void* because "skope_exception.h" can't be included here
	void make_check_args_math(const AstNode* pnode);
	void get_nodes_left_right_sides(const AstNode* pnode, const AstNode** plhs, const AstNode** prhs);
	void eval_lhs(const AstNode* plhs, const AstNode* prhs, CVar& lhs_index, CVar& RHS, uint16_t& typelhs, bool& contig, const CVar* cell_item = NULL);
	void right_to_left(const AstNode* plhs, const CVar& lhs_index, const CVar& robj, uint16_t typelhs, bool contig, CVar* lobj = NULL);
	void eval_index(const AstNode* pInd, const CVar& varLHS, CVar& index);
	void insertreplace(const AstNode* pnode, const CVar& sec, const CVar& indsig, CVar* lobj);
	const CVar* get_cell_item(const AstNode* plhs, const CVar& cellobj);
	void assign_adjust(const AstNode* pn, CVar* lobj, const CVar& lhs_index, const CVar& robj, bool contig);
	void assign_struct(CVar* lobj, const AstNode* plhs, const AstNode* pstruct, const CVar& robj);
	const CVar* get_available_struct_item(const AstNode* plhs, const AstNode** pstruct);
	void sanitize_cell_node(const AstNode* p);
};

class CNodeProbe
{
	friend class skope;
public:
	CNodeProbe(skope* past, AstNode* pnode, CVar* psig);
	virtual ~CNodeProbe() {};
	skope* pbase;
	AstNode* root;
	CVar* psigBase;
	double* lhsref; // actual double buffer location that the lhs is supposed to modify
	bool lhsref_single; // true if lhs indicates a scalar--this guarantees a valid lhsref.
	string varname; // tracks the "full" name of variable including the index, the dot or { }, etc.
	char status[8]; // limit to 7 characters

	CVar* TID_condition(const AstNode* pnode, AstNode* pLHS, AstNode* pRHS);
	CVar* TID_RHS2LHS(const AstNode* pnode, AstNode* p, AstNode* pRHS);
	CVar& ExtractByIndex(const AstNode* pnode, AstNode* p);
	CVar& eval_indexing(const AstNode* pInd, CVar& indSig);
	CVar* TID_indexing(const AstNode* pnode, AstNode* p, AstNode* pRHS);
	CVar* TID_assign(const AstNode* pnode, AstNode* p, AstNode* pRHS);
	CVar* extract(const AstNode* pnode, CTimeSeries& isig);
	CVar* TID_time_extract(const AstNode* pnode, AstNode* p, AstNode* pRHS);
	CVar* TimeExtract(const AstNode* pnode, AstNode* p);
	void insertreplace(const AstNode* pnode, CVar& sec, CVar& indsig);
	CTimeSeries& replace(const AstNode* pnode, CTimeSeries* pobj, CSignal& sec, int id1, int id2);
	CTimeSeries& replace(const AstNode* pnode, CTimeSeries* pobj, body& sec, body& index);
	//CAstException ExceptionMsg(const AstNode *pnode, const string s1, const string s2);
	//CAstException ExceptionMsg(const AstNode *pnode, const char *msg);
	void tree_NARGS(const AstNode* pnode, AstNode* ppar);
	CVar* cell_indexing(CVar* pBase, AstNode* pn);
}; 
