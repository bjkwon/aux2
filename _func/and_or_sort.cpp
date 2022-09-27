#include "functions_common.h"

// 9/24/2022 Todo----think about a good way to handle allowedTypes1 when arg2 is given instead of default arg
// example: and(a,b) where both a and b are an array
// Also, sort doesn't work
Cfunction set_builtin_function_andor(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array" };
	vector<string> desc_arg_opt = { "null or another array" };
	vector<CVar> default_arg = { CVar() };
	set<uint16_t> allowedTypes1 = { 0, 1, 2, 3, TYPEBIT_LOGICAL + 1, TYPEBIT_LOGICAL + 2, TYPEBIT_LOGICAL + 3, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_sort(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array" };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {  };
	set<uint16_t> allowedTypes1 = { 0xFFFF };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}


Cfunction set_builtin_function_mostleast(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array1", "array2" };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {  };
	set<uint16_t> allowedTypes1 = { 1, 2,};
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _andor(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (pnode->str == "and") {
		if (args.front().type() == 0) // single arg
		{
			double res(1.);
			if (past->Sig.IsLogical())
			{
				for (uint64_t k = 0; k < past->Sig.nSamples; k++)
					if (!past->Sig.logbuf[k]) { res = 0.; break; }
			}
			else
			{
				for (uint64_t k = 0; k < past->Sig.nSamples; k++)
					if (past->Sig.buf[k] == 0.) { res = 0.;	break; }
			}
			past->Sig.SetValue(res);
			past->Sig.MakeLogical();
		}
		else
		{
			CVar x1 = past->Sig;
			if (!args.front().IsLogical())
				throw exception_func(*past, pnode, "argument must be logical", pnode->str, 1).raise();
			past->Sig.Reset(1);
			past->Sig.UpdateBuffer(min(x1.nSamples, args.front().nSamples));
			past->Sig.MakeLogical();
			for (uint64_t k = 0; k < min(x1.nSamples, args.front().nSamples); k++)
				past->Sig.logbuf[k] = x1.logbuf[k] && args.front().logbuf[k];
		}
	}
	else {
		if (args.front().type() == 0) // single arg
		{
			double res(0.);
			if (past->Sig.IsLogical())
			{
				for (uint64_t k = 0; k < past->Sig.nSamples; k++)
					if (past->Sig.logbuf[k]) { res = 1.; break; }
			}
			else
			{
				for (uint64_t k = 0; k < past->Sig.nSamples; k++)
					if (past->Sig.buf[k] == 0.) { res = 1.;	break; }
			}
			past->Sig.SetValue(res);
			past->Sig.MakeLogical();
		}
		else
		{
			CVar x1 = past->Sig;
			if (!args.front().IsLogical())
				throw exception_func(*past, pnode, "argument must be logical", pnode->str, 1).raise();
			past->Sig.Reset(1);
			past->Sig.UpdateBuffer(min(x1.nSamples, args.front().nSamples));
			past->Sig.MakeLogical();
			for (uint64_t k = 0; k < min(x1.nSamples, args.front().nSamples); k++)
				past->Sig.logbuf[k] = x1.logbuf[k] || args.front().logbuf[k];
		}
	}
}

template <class T>
int dcomp(const void* arg1, const void* arg2)
{
	if (*(T*)arg1 > *(T*)arg2)	return 1;
	else if (*(T*)arg1 == *(T*)arg2) return 0;
	else	return -1;
}

template <class T>
int dcompR(const void* arg1, const void* arg2)
{
	if (*(T*)arg1 < *(T*)arg2)	return 1;
	else if (*(T*)arg1 == *(T*)arg2) return 0;
	else	return -1;
}

static void __sort(float* buf, uint64_t len, void* parg, void* parg2)
{
	int8_t bufBlockSize = *(int8_t*)parg;
	int8_t order = *(int8_t*)parg2;
	if (bufBlockSize == sizeof(float))
	{
		if (order > 0)
			qsort((void*)buf, len, bufBlockSize, dcomp<float>);
		else
			qsort((void*)buf, len, bufBlockSize, dcompR<float>);
	}
	else if (bufBlockSize == 1)
	{
		if (order > 0)
			qsort((void*)buf, len, bufBlockSize, dcomp<unsigned char>);
		else
			qsort((void*)buf, len, bufBlockSize, dcompR<unsigned char>);
	}
}

void _sort(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	int8_t order = 1;
	int8_t bufBlockSize = past->Sig.bufBlockSize;
	past->Sig.evoke_modsig(__sort, &bufBlockSize, &order);
}

//void _atmost(float* buf, uint64_t len, void* parg, void* parg2)
//(unsigned int id, int unsigned len, void* parg)
//{
//	double limit;
//	if (len == 0) len = nSamples;
//	CVar param = *(CVar*)parg;
//	if (param.IsScalar())
//		limit = param.value();
//	else
//		limit = (id / len < nSamples) ? param.buf[id / len] : std::numeric_limits<double>::infinity();
//	for (unsigned int k = id; k < id + len; k++)
//		if (buf[k] > limit) buf[k] = limit;
//	return *this;
//}
//
//CSignal& CSignal::_atleast(unsigned int id, int unsigned len, void* parg)
//{
//	double limit;
//	if (len == 0) len = nSamples;
//	CVar param = *(CVar*)parg;
//	if (param.IsScalar())
//		limit = param.value();
//	else
//		limit = (id / len < nSamples) ? param.buf[id / len] : -std::numeric_limits<double>::infinity();
//	for (unsigned int k = id; k < id + len; k++)
//		if (buf[k] < limit) buf[k] = limit;
//	return *this;
//}
//
//
//void _mostleast(skope* past, const AstNode* pnode, const vector<CVar>& args)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	if (past->Sig.IsEmpty()) return; //for empty input, empty output
//	past->checkVector(pnode, past->Sig);
//	string func = pnode->str;
//	CVar sig = past->Sig;
//	CVar param = past->Compute(p);
//	if (func == "atmost") past->Sig = sig.fp_mod(&CSignal::_atmost, &param);
//	else if (func == "atleast") past->Sig = sig.fp_mod(&CSignal::_atleast, &param);
//	if (past->Sig.type() & TYPEBIT_TEMPORAL) past->Sig.setsnap();
//}


