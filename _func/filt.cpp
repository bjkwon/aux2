#include "functions_common.h"

Cfunction set_builtin_function_filt(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj", "numerator_array" };
	vector<string> desc_arg_opt = { "denominator_array", "initial_condition_array"};
	vector<CVar> default_arg = { CVar(1.f), CVar() };
	set<uint16_t> allowedTypes1 = { 2, 3, ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, 2 };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_conv(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "signal1", "signal2" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 1, 2, 3, ALL_AUDIO_TYPES };
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

int get_output_count(const AstNode* ptree, const AstNode* pnode);  // support.cpp

static void filterbase(float* buf, size_t len, const vector<float>& num, const vector<float>& den, vector<float>& initial, vector<float>& state)
{
	if (initial.size() < max(num.size(), den.size()) - 1)
	{
		for (auto k = initial.size(); k < max(num.size(), den.size()) - 1; k++)
			initial.push_back(0.);
	}
	state = initial;
	//if (IsComplex())
	//{
	//	complex<float>* out = new complex<float>[len];
	//	for (unsigned int m = 0; m < len; m++)
	//	{
	//		out[m] = num[0] * cbuf[m] + state.front();
	//		//DO THIS--------------------

	//		//size of state is always one less than num or den
	//		//int k = 1;
	//		//for (auto& v : state)
	//		//{
	//		//	v = num[k] * cbuf[m] - den[k] * out[m];
	//		//	if (k < initial.size())
	//		//		v += *((&v) + 1);
	//		//	k++;
	//		//}
	//	}
	//	delete[] cbuf;
	//	cbuf = out;
	//}
	//else
	{
		float* out = new float[len];
		for (unsigned int m = 0; m < len; m++)
		{
			out[m] = num[0] * buf[m] + state.front();
			//size of state is always one less than num or den
			int k = 1;
			for (auto& v : state)
			{
				// den and num may have different size
				if (k < num.size())
				{
					v = num[k] * buf[m];
					if (k < den.size()) v += -den[k] * out[m];
				}
				else
					v = -den[k] * out[m]; // should be k < den.size()
				if (k < state.size())
					v += *((&v) + 1);
				k++;
			}
		}
		memcpy(buf, out, sizeof(float) * len);
		delete[] out;
	}
}

CSignal __filt(const CSignal& base, void* parg1, void* parg2)
{
	vector<CVar> argin = *(vector<CVar>*)parg1;
	vector<float> num(argin[0].buf, argin[0].buf + argin[0].nSamples);
	vector<float> den(argin[1].buf, argin[1].buf + argin[1].nSamples);
	vector<float> init(argin[2].buf, argin[2].buf + argin[2].nSamples);
	vector<float> fin;
	filterbase(base.buf, base.nSamples, num, den, init, fin);
	// parg2 was declared as CVar in _filt
	CVar* cvarfin = (CVar*)parg2;
	cvarfin->UpdateBuffer(fin.size());
	memcpy(cvarfin->buf, fin.data(), sizeof(float) * fin.size());
	// final state after filtering is not only copied to parg2, but also used to update parg1
	// updating init won't do anything (init is a local variable)
	// instead update the content of the pointer used to initialize init
	// So that final state at this group can be used as an initial state in the successive call inside of CSignal::evoke_modsig2
	// Even if it's not necessary because nGroup=1, updating the content of this pointer won't do any harm. 01/20/2022
	if (argin[2].nSamples == 0)
		(*(((vector<CVar>*)parg1)->begin() + 2)).UpdateBuffer(fin.size());
	memcpy((*(((vector<CVar>*)parg1)->begin() + 2)).buf, fin.data(), sizeof(float) * fin.size());
	return base;
}

void _filt(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (args[2].nSamples >= max(args[0].nSamples, args[1].nSamples))
		throw exception_func(*past, pnode, "the size of state array must be less than the coefficient array").raise();
	if (fname == "filt")
	{
		CVar* extraOut = new CVar; // to carry a state array
		past->Sig = past->Sig.evoke_modsig2(__filt, (void*)&args, (void*)extraOut);
		if (get_output_count(past->node, pnode) > 1)
		{
			past->SigExt.push_back(move(make_unique<CVar*>(&past->Sig)));
			unique_ptr<CVar*> pt = make_unique<CVar*>(extraOut);
			past->SigExt.push_back(move(pt));
		}
		else
			delete extraOut;
	}
	else // filtfilt
	{
			CSignal temp(past->Sig.GetFs()), out(past->Sig.GetFs());
			size_t nfact = (size_t)(3 * (max(args[0].nSamples, args[1].nSamples) - 1));
			temp.UpdateBuffer(nfact + past->Sig.nSamples);
			memset(temp.buf, 0, sizeof(float) * nfact);
			memcpy(temp.buf + nfact, past->Sig.buf, sizeof(float) * past->Sig.nSamples);
			CSignal temp2(temp);
			temp += &temp2;
			CVar* extraOut = new CVar; // to carry a state array
			temp.evoke_modsig2(__filt, (void*)&args, (void*)extraOut);
			temp.ReverseTime();
			extraOut->ReverseTime();
			((vector<CVar>*) & args)->back().UpdateBuffer(extraOut->nSamples);
			memcpy(((vector<CVar>*) & args)->back().buf, extraOut->buf, sizeof(float) * extraOut->nSamples);
			temp.evoke_modsig2(__filt, (void*)&args, (void*)extraOut);
			temp.ReverseTime();
			extraOut->ReverseTime();
			memcpy(past->Sig.buf, temp.buf + nfact, sizeof(float) * past->Sig.nSamples);
			if (get_output_count(past->node, pnode) > 1)
			{
				past->SigExt.push_back(move(make_unique<CVar*>(&past->Sig)));
				unique_ptr<CVar*> pt = make_unique<CVar*>(extraOut);
				past->SigExt.push_back(move(pt));
			}
			else
				delete extraOut;
	}
}

CSignal __conv(const CSignal& base, void* parg1, void* parg2)
{
	vector<CVar> argin = *(vector<CVar>*)parg1;
	CSignal out(base.GetFs());
	out.UpdateBuffer(base.nSamples + argin.front().nSamples - 1);
	for (unsigned int k = 0; k < out.nSamples; k++)
	{
		float tp = 0.f;
		for (int p(0), q(0); p < base.nSamples; p++)
		{
			if ((q = k - p) < 0) continue;
			if (p < base.nSamples && q < argin.front().nSamples)
				tp += base.buf[p] * argin.front().buf[q];
		}
		out.buf[k] = tp;
	}
	return out;
}

void _conv(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	past->Sig = past->Sig.evoke_modsig2(__conv, (void*)&args, NULL);
}

