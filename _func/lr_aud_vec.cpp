#include "functions_common.h"

Cfunction set_builtin_function_audio(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "vector/matrix" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 2, 3 };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_vector(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { AUDIO_TYPES_1D };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_leftright(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _audio(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	switch (past->Sig.nGroups)
	{
	case 1:
		past->Sig.SetFs(past->GetFs());
		break;
	case 2:
	{
		past->Sig.SetFs(past->GetFs());
		past->Sig.nSamples /= 2;
		past->Sig.nGroups = 1;
		CSignals nextgh(past->Sig.GetFs());
		nextgh <= past->Sig;
		nextgh.buf += past->Sig.nSamples;
		past->Sig.SetNextChan(nextgh);
	}
	break;
	default:
		exception_func(*past, pnode, "Cannot apply to a matrix with rows > 2.", "audio").raise();
		break;
	}
}

void _vector(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (fname == "vector") {
		past->Sig.SetFs(1);
		past->Sig.snap = false;
		if (past->Sig.next)
		{
			CSignal out = CSignal(1, past->Sig.nSamples * 2);
			memcpy(out.logbuf, past->Sig.logbuf, past->Sig.nSamples * past->Sig.bufBlockSize);
			memcpy(out.logbuf + past->Sig.nSamples * past->Sig.bufBlockSize, past->Sig.next->logbuf, past->Sig.nSamples * past->Sig.bufBlockSize);
			out.nGroups = 2;
			past->Sig = (CVar)(CSignals)out;
		}
	}
	else if (fname == "squeeze") {
		past->Sig.Squeeze();
	}
}

void _leftright(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (fname == "left")
	{
		delete past->Sig.next;
		past->Sig.next = NULL;
	}
	else if (fname == "right")
	{
		if (past->Sig.next)
			past->Sig.bringnext();
	}
}