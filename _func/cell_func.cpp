#include "functions_common.h"

Cfunction set_builtin_function_cellstruct(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "struct_object", "prop_name"};
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {  };
	set<uint16_t> allowedTypes1 = { TYPEBIT_CELL, TYPEBIT_STRUT, TYPEBIT_STRUTS, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2};
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}
//
//void _cell(skope* past, const AstNode* pnode, const vector<CVar>& args)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	if (!past->Sig.IsScalar())
//		throw CAstException(FUNC_SYNTAX, *past, p).proc("argument must be a scalar.");
//	int n = (int)round(past->Sig.value());
//	if (n <= 0)
//		throw CAstException(FUNC_SYNTAX, *past, p).proc("argument must be a positive number.");
//
//	past->Sig.Reset();
//	CVar tp;
//	for (int k = 0; k < n; k++)
//		past->Sig.appendcell(tp);
//}

void _cellstruct(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!strcmp(pnode->str, "head")) {
		past->Sig.set_class_head(args.front());
	}
	else if (!strcmp(pnode->str, "erase") || !strcmp(pnode->str, "ismember")) {
		past->Sig.Reset();
		const AstNode* arg0 = arg0node(pnode, past->node);
		CVar* psig = past->GetVariable(arg0->str);
		auto it = psig->strut.find(args.front().str());
		if (it != psig->strut.end()) {
			if (!strcmp(pnode->str, "erase"))
				psig->strut.erase(it);
			past->Sig.SetValue(1);
		}
		else {
			past->Sig.SetValue(0);
		}
		past->Sig.MakeLogical();
	}
	else /* ismember */ {
	}
}
