#include "functions_common.h"
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

Cfunction set_builtin_function_json(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "json_filename", };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

static void json2CVar2(CVar& out, const json& in)
{
	auto uvect = in.get<vector<string>>();
	for (auto st : uvect)
	{
		std::cout << st << endl;
	}
}
static void json2CVar(CVar& out, const json& in)
{
	auto umap = in.get<unordered_map<string, json>>();
	for (auto v : umap) {
		auto vfirst = v.first;
		float conf = 0;
		{
			int j = 5;
		}
		if (v.second.type_name() == "object") {
			CVar childobj;
			json2CVar(childobj, v.second);
			if (v.first=="confidence" && childobj.strut.find("confidence") != childobj.strut.end())
			{
				if (childobj.strut.find("confidence")->second.strut.empty())
					conf = childobj.strut["confidence"].value();
			}
			out.strut[v.first] = childobj;
		}
		else if (v.second.type_name() == "array") {
			if (vfirst == "model") {
				for (auto st : v.second.get<vector<string>>())
					out.strut[v.first].cell.push_back(CVar(st));
				return;
			}
			CVar cellarrayobj;
			string word;
			for (auto element : v.second) {
				CVar tempobj;
				if (vfirst == "model")
					json2CVar2(tempobj, element);
				else
					json2CVar(tempobj, element);
				if (tempobj.strut.find("confidence") != tempobj.strut.end())
					conf = tempobj.strut["confidence"].value();
				if (tempobj.strut.find("word")!= tempobj.strut.end())
					word = tempobj.strut["word"].str();
				cellarrayobj.cell.push_back(tempobj);
			}
			if (v.first == "utterances") {
				auto prev = out.strut[v.first];
				out.strut[v.first] = cellarrayobj;
			}
			else
				out.strut[v.first] = cellarrayobj;
		}
		else if (v.second.type_name() == "string")
			out.strut[v.first] = v.second.get<string>();
		else if (v.second.type_name() == "number")
			out.strut[v.first] = v.second.get<float>();
		else if (v.second.type_name() == "boolean") {
			out.strut[v.first] = (float)v.second.get<bool>();
			out.strut[v.first].MakeLogical();
		}
		else if (v.second.type_name() == "null")
			out.strut[v.first] = CVar();
		else
			printf("ERROR!!!!!!!!!\n"); // Clean up later 9/14/2022
	}
}

void _json(skope* past, const AstNode* pnode, const vector<CVar>& args)
{
	char errStr[16];
	string filename = past->Sig.str();
	string errstr, content;
	size_t len, nLines;
	CVar out;

	ifstream infc; // input file stream carrier
	try {
		infc.open(filename);
		if (infc.fail())
			throw filename; 
		stringstream buffer;
		buffer << infc.rdbuf();
		json jdata = json::parse(buffer.str());
		json2CVar(out, jdata);
		past->Sig = out;
	}
	catch (const string& fname) {
		exception_func(*past, pnode, "Error reading file", fname).raise();
	}
}

