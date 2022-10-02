#include <iostream>
#include "aux_classes.h"
#include "skope.h"
#include "audstr.h"

using namespace std;

int str2vector(vector<string>& out, const string& in, const string& delim_chars); // utils.cpp

void auxenv_path(CAstSigEnv* pEnv, const vector<string>& cmd)
{
	if (cmd.empty()) {
		for (auto s : pEnv->AuxPath)
			cout << s << endl;
	}
	else if (cmd.front() == "-a" || cmd.front() == "--add")
	{
		string newpath;
		if (cmd[1].back() == '\\') newpath = cmd[1].substr(0, cmd[1].size() - 1);
		WIN32_FIND_DATA ls;
		HANDLE hFind = FindFirstFile(newpath.c_str(), &ls);
		if (hFind == INVALID_HANDLE_VALUE)
			cout << "Invalid path or path not found in AuxPath: " << newpath << endl;
		else
		{
			pEnv->AddPath(newpath);
			//string paths = pEnv->path_delimited_semicolon();
			//if (!printfINI(errstr, iniFile, "PATH", "%s", paths.c_str())) {
			//	cout << "AuxPath updated, but failed while updating the INI file." << endl;
			//	return 0;
			//}
		}
	}
	else if (cmd.front() == "-r" || cmd.front() == "--remove")
	{
		string newpath;
		if (cmd[1].back() != '\\') newpath = cmd[1] + '\\';
		auto fd = find(pEnv->AuxPath.begin(), pEnv->AuxPath.end(), newpath);
		if (fd != pEnv->AuxPath.end())
		{
			pEnv->AuxPath.erase(fd);
			string paths = pEnv->path_delimited_semicolon();
			//if (!printfINI(errstr, mainSpace.iniFile, "PATH", "%s", paths.c_str())) {
			//	cout << "AuxPath updated, but failed while updating the INI file." << endl;
			//	return 0;
			//}
		}
		else
			cout << "Invalid path or path not found in AuxPath: " << newpath << endl;
	}
	else
	{
		cout << "Format: #path [-a|r|s] path \nor #path [--add|--remove|--show] path" << endl;
	}
}

void auxenv_setfs(CAstSigEnv* pEnv, const vector<string>& cmd)
{

}

void auxenv_load(CAstSigEnv* pEnv, const vector<string>& cmd)
{

}

void auxenv_save(CAstSigEnv* pEnv, const vector<string>& cmd)
{

}

void auxenv(CAstSigEnv* pEnv, const string& cmd)
{
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
			auxenv_setfs(pEnv, cmdl);
		}
		else if (cmdl[0] == "load") {
			cmdl.erase(cmdl.begin());
			auxenv_load(pEnv, cmdl);
		}
		else if (cmdl[0] == "save") {
			cmdl.erase(cmdl.begin());
			auxenv_save(pEnv, cmdl);
		}
		else {
			cout << "Unrecognized Aux Env command: " << cmdl[0] << endl;
		}
		return;
	}


}