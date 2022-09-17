#include "functions_common.h"
#include "..\psycon.tab.h"

// This function datatype will eventually eliminate the use of CSIG_ series constants 
// and all the is____ functions 5/25/2020


Cfunction set_builtin_function_veq(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "object", "object", };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 0xFFFF };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 0xFFFF };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_datatype(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "object", };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 0xFFFF };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

int __datatype(const CVar& sig, WORD& out)
{
	if (sig.IsEmpty())				out = 0xffff;
	else if (sig.IsGO())		out = 0x2000;
	else if (sig.GetFs() == 1)		out = 0;
	else if (sig.GetFs() == 2)		out = 0x0010;
	else if (sig.GetFs() > 500)		out = 0x0020;
	else if (sig.IsLogical())		out = 0x0040;
	else if (!sig.cell.empty())		out = 0x1000;
	else if (!sig.strut.empty())		out = 0x2000;
	else
		return -1;
	if (sig.snap) out += 0x0008;
	if (sig.nSamples > 0) out++;
	if (sig.nSamples > 1) out++;
	return 1;
}

void _datatype(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	uint16_t out;
	if ((out = past->Sig.type()) == 0xffff)
		throw exception_func(*past, pnode, "this particular data type has not been ready to handle.", "otype()", 1).raise();
	//if (out & 0x2000)
	//{
	//	past->pgo = NULL;
	//	past->Sig.Reset();
	//}
	past->Sig.SetValue((double)out);
}

void _veq(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	skope br(past);
	WORD type1, type2;
	CVar arg1 = past->Sig;
	CVar arg2 = args[0];
	//if (pnode->type == N_STRUCT)
	//{ // x.issame(y)
	//	arg2 = past->Compute(pnode->alt->child);
	//}
	//else if (pnode->type == T_ID)
	//{ // issame(x,y)
	//	if (pnode->alt->type == N_ARGS)
	//	{
	//		arg1 = past->Compute(pnode->alt->child);
	//		arg2 = past->Compute(pnode->alt->tail);
	//	}
	//	else
	//		throw exception_func(*past, pnode, "Invalid syntax parsing(you're not supposed to see this error msg).", "issame()", 1).raise();
	//}
	//else
	//	throw exception_func(*past, pnode, "Invalid syntax parsing(you're not supposed to see this error msg).", "issame()", 1).raise();

	if (__datatype(arg1, type1) < 0)
		throw exception_func(*past, pnode, "this particular data type has not been ready to handle.", "issame()", 1).raise();
	if (__datatype(arg2, type2) < 0)
		throw exception_func(*past, pnode, "this particular data type has not been ready to handle.", "issame()", 1).raise();
	try {
		// throw 0 for false
		if (type1 != type2) throw 0;
		else if (arg1.nSamples != arg2.nSamples) throw 0;
		else if (type1 & 0x2000) // GO
		{
			if (arg1.value() != arg2.value()) throw 0;
		}
		else
		{
			if (arg1.bufBlockSize == 8)
				for (unsigned k = 0; k < arg1.nSamples; k++)
				{
					if (arg1.buf[k] != arg2.buf[k]) throw 0;
				}
			else if (arg1.bufBlockSize == 16)
				for (unsigned k = 0; k < arg1.nSamples; k++)
				{
					if (arg1.cbuf[k] != arg2.cbuf[k]) throw 0;
				}
			else
				for (unsigned k = 0; k < arg1.nSamples; k++)
				{
					if (arg1.logbuf[k] != arg2.logbuf[k]) throw 0;
				}
		}
		past->Sig.Reset(1);
		past->Sig.MakeLogical();
		past->Sig.UpdateBuffer(1);
		past->Sig.logbuf[0] = true;
		return;
	}
	catch (int k)
	{
		//k should be 0 and it doesn't matter what k is.
		k = 0; // just to avoid warning C4101
		past->Sig.Reset(1);
		past->Sig.MakeLogical();
		past->Sig.UpdateBuffer(1);
		past->Sig.logbuf[0] = false;
		return;
	}
}

