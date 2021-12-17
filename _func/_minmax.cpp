#include "functions_common.h"

int get_output_count(const AstNode* ptree, const AstNode* pnode);  // support.cpp

void _minmax(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
	if (past->Sig.IsEmpty()) return; //for empty input, empty output
	blockLogical1(*past, pnode, past->Sig, fname);
	blockString1(*past, pnode, past->Sig, fname);
	CVar sig = past->Sig;
	int nOutVars = get_output_count(past->node, pnode);

	CVar* popt = NULL;
	if (nOutVars > 1)
		popt = new CVar(sig.GetFs());
	auto out = max_element(sig.strbuf, sig.strbuf + sig.nSamples);

	if (fname == "max") past->Sig = sig.evoke_getval(&CSignal::_max, popt);
	else if (fname == "min") past->Sig = sig.evoke_getval(&CSignal::_min, popt);
	if (nOutVars > 1)
	{
		past->SigExt.push_back(move(make_unique<CVar*>(&past->Sig)));
		unique_ptr<CVar*> pt = make_unique<CVar*>(popt); // popt carries maximum/minimum indices
		past->SigExt.push_back(move(pt));
	}
	if (past->Sig.type() & TYPEBIT_TEMPORAL) past->Sig.setsnap();
}

// DO NOT CALL this function with empty buf
float body::_max(unsigned int id, int unsigned len, void* pind) const
{ // pind is an output carrier, a pointer to a float object
	// For a complex array, this does not return a complex value (it only returns real portion of the intended max element
 // Therefore, to get the intended max element, you need to get it from the max index through parg.
	unsigned int mid;
	float out = -std::numeric_limits<float>::infinity();
	if (len == 0) len = nSamples;
	if (bufBlockSize == sizeof(float))
	{
		for (unsigned int k = id; k < id + len; k++)
		{
			if (buf[k] > out)
				out = buf[k], mid = k;
		}
	}
	else if (bufBlockSize == 2 * sizeof(float))
	{
		for (unsigned int k = id; k < id + len; k++)
		{
			if (abs(cbuf[k]) > out)
				out = buf[k], mid = k;
		}
	}
	else
		throw "Invalid bufBlockSize";
	if (pind) *(float*)pind = (float)mid + 1 - id;
	return out;
}

float body::_min(unsigned int id, unsigned int len, void* pind) const
{
	unsigned int mid;
	float out = std::numeric_limits<float>::infinity();
	if (len == 0) len = nSamples;
	if (bufBlockSize == sizeof(float))
	{
		for (unsigned int k = id; k < id + len; k++)
		{
			if (buf[k] < out)
				out = buf[k], mid = k;
		}
	}
	else if (bufBlockSize == 2 * sizeof(float))
	{
		for (unsigned int k = id; k < id + len; k++)
		{
			if (buf[k] < out)
				out = buf[k], mid = k;
		}
	}
	else
		throw "Invalid bufBlockSize";
	if (pind) *(float*)pind = (float)mid + 1 - id;
	return out;
}
