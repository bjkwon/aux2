#include <iostream>
#include <fstream>
#include <sstream>
#include "aux_classes.h"
#include "skope.h"
#include <array>

#ifdef _WIN32
#include <direct.h>
#else
#include <glob.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#endif

using namespace std;

int str2vector(vector<string>& out, const string& in, const string& delim_chars); // utils.cpp
string get_current_dir(); // utils.cpp

#include "nlohmann/json.hpp"
using json = nlohmann::json;
void json2CVar(CVar& out, const json& in, skope* past, const AstNode* pnode, const string& fname); // json.cpp

void auxenv_path(CAstSigEnv* pEnv, const vector<string>& cmd)
{
	if (cmd.empty()) {
		for (auto s : pEnv->AuxPath)
			cout << s << endl;
	}
	else if (cmd.front() == "-a" || cmd.front() == "--add")
	{
		string newpath = cmd[1];
		bool dir_exists = false;
		if (cmd[1].back() == DIRMARKER) 
			newpath = cmd[1].substr(0, cmd[1].size() - 1);
#ifdef _WIN32
		WIN32_FIND_DATA ls;
		dir_exists = FindFirstFile(newpath.c_str(), &ls) != INVALID_HANDLE_VALUE;
#else
		glob_t gbuf = { 0 };
		glob(newpath.c_str(), GLOB_DOOFFS, NULL, &gbuf);
		dir_exists = gbuf.gl_pathc > 0;
		globfree(&gbuf);
#endif 
		if (dir_exists)
			pEnv->AddPath(newpath);
		else
			cout << "Invalid path or path not found in AuxPath: " << newpath << endl;
	}
	else if (cmd.front() == "-r" || cmd.front() == "--remove")
	{
		string newpath = cmd[1];
		if (cmd[1].back() != DIRMARKER) newpath = cmd[1] + DIRMARKER;
		auto fd = find(pEnv->AuxPath.begin(), pEnv->AuxPath.end(), newpath);
		if (fd != pEnv->AuxPath.end())
			pEnv->AuxPath.erase(fd);
		else
			cout << "Invalid path or path not found in AuxPath: " << newpath << endl;
	}
	else
	{
		cout << "Format: ##path [-a|r] path \nor ##path [--add|--remove] path \nor ##path to show paths" << endl;
	}
}

void auxenv_precision(CAstSigEnv* pEnv, const AstNode *p)
{
	if (!p) {
		cout << "Current number display precision:" << pEnv->display_precision << endl;
	}
	else {
		int prec = (int)p->dval;
		if (p->type != T_NUMBER || prec > 16 || prec < 1) {
			cout << ">precision (number_precision)" << endl;
		}
		else {
			pEnv->display_precision = prec;
			cout << "Number display precision adjusted to " << pEnv->display_precision << endl;
		}
	}
}

void auxenv_setfs(CAstSigEnv* pEnv, const AstNode* p)
{
	if (!p) {
		cout << "Current sampling rate:" << pEnv->Fs << endl;
	}
	else {
		int fs = (int)p->dval;
		if (p->type != T_NUMBER || fs > 48000 || fs < 100) {
			cout << ">setfs (new_sampling_rate_in_Hz)" << endl;
		}
		else {
			cout << ">setfs (new_sampling_rate_in_Hz)--to be implemented." << endl;
			//pEnv->Fs = (int)p->dval;
		}
	}
}

void auxenv_cd(CAstSigEnv* pEnv, const AstNode* p)
{
	int res;
	array<char, 128> buffer;
	string result;
	string destdir = "cd ";
	if (p && p->next) {
		cout << "Error: only one arg should be presented." << endl;
		return;
	}

	if (p)
	{
		if (p->type== T_REPLICA)
			destdir += "..";
		else
			destdir += p->str;
		destdir += " & if %errorlevel%==0 (cd) else (echo ERROR inside auxenv_cd !!!)";
	}

#ifdef _WIN32
	unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(destdir.c_str(), "r"), _pclose);
#else
	unique_ptr<FILE, decltype(&pclose)> pipe(popen(destdir.c_str(), "r"), pclose);
#endif
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	
	pEnv->AppPath.clear();
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		pEnv->AppPath += buffer.data();
	}
	if (pEnv->AppPath.back() == '\n')
		pEnv->AppPath.pop_back();
#ifdef _WIN32
	res = _chdir(pEnv->AppPath.c_str());
#else
	chdir(pEnv->AppPath.c_str());
#endif
	if (p)
		cout << "Current directory changed to " << pEnv->AppPath << endl;
	else
		cout << "Current directory: " << pEnv->AppPath << endl;
}

void auxenv_load(CAstSigEnv* pEnv, const vector<string>& cmd)
{

}

void auxenv_save(CAstSigEnv* pEnv, const vector<string>& cmd)
{

}

void auxenv_setdebug(CAstSigEnv* pEnv, const AstNode* p)
{
	if (!p) {
		cout << "To set/clear debugger breakpoints: >debug udf_name line1 line2 line2...  (negatiev line number to clear it; 0 to clear all):" << endl;
	}
	else {
		if (p->type!=T_ID) {
			cout << "To set/clear debugger breakpoints: >debug udf_name line1 line2 line2...  (negatiev line number to clear it; 0 to clear all):" << endl;
		}
		else {
			vector<int> breakpoints;
			if (!p->next) {
				cout << "breakpoints for " << p->str << ": ";
				if (pEnv->udf.find(p->str) == pEnv->udf.end()) {
					cout << "(not set)" << endl;
					cout << "To set/clear debugger breakpoints: >debug udf_name line1 line2 line2...  (negatiev line number to clear it; 0 to clear all):" << endl;
				}
				else {
					if (pEnv->udf[p->str].DebugBreaks.empty())
						cout << "(not set)";
					else
						for (auto v : pEnv->udf[p->str].DebugBreaks)
							cout << v << ' ';
					cout << endl;
				}
				return;
			}
			for (auto q = p->next; q; q = q->next) {
				if (q->type != T_NUMBER && q->type != T_NEGATIVE) {
					cout << "Line number is expected for breakpoint" << endl;
					cout << "To set/clear debugger breakpoints: >debug udf_name line1 line2 line2...  (negatiev line number to clear it; 0 to clear all):" << endl;
					return;
				}
				breakpoints.push_back(q->type == T_NEGATIVE ? -(int)q->child->dval: (int)q->dval);
			}
			for (auto v : breakpoints)
			{
				if (v == 0) {
					pEnv->udf[p->str].DebugBreaks.clear();
				}
				else {
					auto fd = find(pEnv->udf[p->str].DebugBreaks.begin(), pEnv->udf[p->str].DebugBreaks.end(), v);
					if (fd == pEnv->udf[p->str].DebugBreaks.end() && v > 0)
						pEnv->udf[p->str].DebugBreaks.push_back(v);
					else {
						if (v < 0) {
							fd = find(pEnv->udf[p->str].DebugBreaks.begin(), pEnv->udf[p->str].DebugBreaks.end(), -v);
							if (fd != pEnv->udf[p->str].DebugBreaks.end())
								pEnv->udf[p->str].DebugBreaks.erase(fd);
						}
					}
				}

			}

		}
	}
}


void auxenv_cd(CAstSigEnv * pEnv, string &destdir)
{
	array<char, 128> buffer;
	string result;
	destdir = destdir.substr(1);
	destdir += " & if %errorlevel%==0 (cd) else (echo ERROR inside auxenv_cd !!!)";
#ifdef _WIN32
	unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(destdir.c_str(), "r"), _pclose);
#else
	unique_ptr<FILE, decltype(&pclose)> pipe(popen(destdir.c_str(), "r"), pclose);
#endif
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	pEnv->AppPath.clear();
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		pEnv->AppPath += buffer.data();
	}
	if (pEnv->AppPath.back() == '\n') 
		pEnv->AppPath.pop_back();
#ifdef _WIN32
	_chdir(pEnv->AppPath.c_str());
#else
	chdir(pEnv->AppPath.c_str());
#endif
}

static void display_vars(CAstSigEnv* pEnv, skope* psk)
{
	for (auto it : psk->Vars) {
		cout << it.first << ' ' << "0x" << setw(4) << setfill('0') << hex << it.second.type() << ' ' << endl;
	}
}

void auxenv(CAstSigEnv* pEnv, const AstNode *pnode, skope *psk)
{
	if (!strcmp(pnode->str, "path")) {

	} 
	else if (!strcmp(pnode->str, "precision")) {
		auxenv_precision(pEnv, pnode->next);
	}
	else if (!strcmp(pnode->str, "setfs")) {
		auxenv_setfs(pEnv, pnode->next);
	}
	else if (!strcmp(pnode->str, "load")) {
	}
	else if (!strcmp(pnode->str, "save")) {
	}
	else if (!strcmp(pnode->str, "vars")) {
		display_vars(pEnv, psk);
	}
	else if (!strcmp(pnode->str, "debug")) {
		// >debug my_udf line1 line2 line3...
		auxenv_setdebug(pEnv, pnode->next);
	}
	else if (!strcmp(pnode->str, "cd")) {
		auxenv_cd(pEnv, pnode->next);
	}
	else {

	}

	string cmd;
	size_t nItems, k(0);
	vector<string> cmdl; // command delimitered with space
	nItems = str2vector(cmdl, cmd.c_str(), " ");
	if (nItems > 0) {

		if (cmdl[0] == "path") {
			cmdl.erase(cmdl.begin());
			auxenv_path(pEnv, cmdl);
		}
		else if (cmdl[0] == "setfs") {
			cmdl.erase(cmdl.begin());
		}
		else if (cmdl[0] == "load") {
			cmdl.erase(cmdl.begin());
			auxenv_load(pEnv, cmdl);
		}
		else if (cmdl[0] == "save") {
			cmdl.erase(cmdl.begin());
			auxenv_save(pEnv, cmdl);
		}
		else if (cmdl[0] == "vars") {
		}
		//else if (cmdl[0] == "debug") {
		//	auxenv_setdbg(pEnv, cmdl, psk);
		//}
		else if (cmdl[0] == "cd") {
			cmdl.erase(cmdl.begin());
		//	auxenv_cd(pEnv, cmdl);
		}
		else {
			cout << "Unrecognized Aux Env command: " << cmdl[0] << endl;
		}
		return;
	}
}

void read_auxenv(int& fs0, int& precision, vector<string>& auxpathfromenv, const string& envfilename)
{
	ifstream envfstream;
	json jenv;
	try {
		envfstream.open(envfilename);
		if (envfstream.fail())
			throw envfilename;
		stringstream buffer;
		buffer << envfstream.rdbuf();
		jenv = json::parse(buffer.str());
		auto fd = jenv.find("fs");
		if (fd != jenv.end())
			fs0 = jenv["fs"].get<int>();
		fd = jenv.find("precision");
		if (fd != jenv.end())
			precision = jenv["precision"].get<int>();
		fd = jenv.find("AuxPath");
		if (fd != jenv.end()) {
			if ((*fd).type_name() == "array") {
				for (auto el : (*fd)) {
					auxpathfromenv.push_back(el);
				}
			}
		}
	}
	catch (const string& fname) {
		cout << "Environment file not found--- " << fname << endl;
	}
	catch (json::parse_error e) {
		cout << e.what() << endl;
	}
	catch (json::type_error e) {
		cout << e.what() << endl;
	}
}

void save_auxenv(CAstSigEnv* pEnv, const string& envfilename)
{
	ofstream envfstream;
	json jenv; 
	jenv["fs"] = pEnv->Fs;
	jenv["precision"] = pEnv->display_precision;
	for (auto s : pEnv->AuxPath)
		jenv["AuxPath"].push_back(s);
	try {
		envfstream.open(envfilename);
		if (envfstream.fail())
			throw envfilename;
		envfstream << jenv << endl;
	}
	catch (const string& fname) {
		cout << "Environment file cannot be opened for writing--- " << fname << endl;
	}
	catch (json::parse_error e) {
		cout << e.what() << endl;
	}
	catch (json::type_error e) {
		cout << e.what() << endl;
	}
}