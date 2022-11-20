#include "echo.h"
#include <iomanip>      // std::setw

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

string echo_object::tmarks(const CTimeSeries& sig, bool unit)
{
	// unit is to be used in the future 8/15/2018
	// Get the timepoints
	ostringstream out;
	streamsize org_precision = out.precision();
	out.setf(ios::fixed);
	out.precision(1);
	int kk(0), tint(sig.CountChains());
	for (const CTimeSeries* xp = &sig; kk < tint; kk++, xp = xp->chain) {
		out << "(" << xp->tmark;
		if (unit) out << "ms";
		out << "~" << xp->tmark + 1000. * xp->nSamples / xp->GetFs();
		if (unit) out << "ms";
		out << ") ";
	}
	kk = 0;
	for (const CTimeSeries* xp = &sig; kk < tint; kk++, xp = xp->chain) {
		out << " " << xp->nSamples;
		if (xp->nGroups > 1)
			out << " (" << xp->nGroups << "x" << xp->Len() << ")";
	}
	out << endl;
	out.unsetf(ios::floatfield);
	out.precision(org_precision);
	return out.str();
}

string echo_object::make(const CTimeSeries& sig, bool unit, int offset)
{
	ostringstream out;
	streamsize org_precision = out.precision();
	out.setf(ios::fixed);
	if (unit) out.precision(1);
	else out.precision(2);
	int kk(0), tint(sig.CountChains());
	for (const CTimeSeries* xp = &sig; kk < tint; kk++, xp = xp->chain)
	{
		for (int k = 0; k < offset + 1; k++) out << " ";
		out << "(" << xp->tmark;
		if (unit) out << "ms";
		out << ") ";
		out << print_vector(*xp, offset);
	}
	out.unsetf(ios::floatfield);
	out.precision(org_precision);
	return out.str();
}

void echo_struct::print(const CVar& obj, const string& head)
{
	echo_object::header(head);
	cout << "[Structure] ..." << endl;
	for (map<string, CVar>::const_iterator it = obj.strut.begin(); it != obj.strut.end(); it++)
	{
		ostringstream var0;
		var0 << '.' << it->first;
		echo_object::print(var0.str(), it->second, offset + 1);
	}
	for (map<string, vector<CVar*>>::const_iterator it = obj.struts.begin(); it != obj.struts.end(); it++)
	{
		ostringstream var0;
		var0 << '.' << it->first;
		if ((*it).second.empty())
			echo_object::print(var0.str(), CVar(), offset + 1);
		else
		{
			CVar temp;
			auto j = 1;
			//for (vector<CVar*>::const_iterator jt = (*it).second.begin(); jt != (*it).second.end() && j < 10; jt++)
			//{
			//	if (!skope::HandleSig(&temp, *jt)) temp.SetString("(internal error)");
			//	echo_object::print(var0.str(), temp, offset + 1);// , " [Handle]");
			//}
		}
	}
}

void echo_cell::print(const CVar& obj, const string& head)
{
	echo_object::header(head);
	cout << "[Cell] ..." << endl;
	auto j = 1;
	for (vector<CVar>::const_iterator it = obj.cell.begin(); it != obj.cell.end(); it++)
	{
		ostringstream _varname;
		_varname << name << '{' << j++ << '}' ;
		echo_object::print(_varname.str(), *it, offset);
	}
}

string echo_object::row(const CTimeSeries& obj, unsigned int id0, int offset, int prec)
{
	ostringstream out;
	streamsize org_precision = out.precision();
	out.precision(prec);
	unsigned int k = 0;
	vector<int> idx;
	idx.reserve(obj.Len());
	if (tbht == "head")
		for (int k = 0; k < min(tbht_count, obj.Len()); k++) idx.push_back(k);
	else if (tbht == "tail")
		for (int k = obj.Len() - tbht_count; k < obj.Len(); k++) idx.push_back(k);
	else
		for (int k = 0; k < min(10, obj.Len()); k++) idx.push_back(k);
	if (idx.front() != 0) {
		if (ISSTRINGG(obj.type())) out << "...";
		else	out << "... ";
	}
	else if (ISSTRINGG(obj.type()))
		out << "\"";
	if (obj.IsComplex()) {
		for (; k < min(10, obj.Len()); k++, out << " ")
		{
			for (int m = 0; m < offset; m++) out << " ";
			out << obj.cbuf[k + id0];
		}
	}
	else
	{
		for (int m = 0; m < offset; m++) out << " ";
		if (obj.IsLogical())
		{
			for (auto k : idx)
				out << obj.logbuf[k + id0] << " ";
		}
		else if (ISSTRINGG(obj.type())) {
			for (auto k : idx)
				out << obj.strbuf[k + id0];
		}
		else 
			for (auto k : idx)
				out << obj.buf[k + id0] << " ";
	}
	if (idx.back() < obj.Len() - 1) {
		if (ISSTRINGG(obj.type())) out << "... ";
		else out << " ... ";
	}
	else if (ISSTRINGG(obj.type()))
		out << "\"";
	out.precision(org_precision);	
	out << postscript << "\n";
	return out.str();
}

string echo_object::print_vector(const CTimeSeries& obj, int offset)
{
	stringstream ss;
	vector<int> idx;
	idx.reserve(obj.nGroups);
	if (tbht == "top")
		for (int k = 0; k < min(tbht_count, obj.nGroups); k++) idx.push_back(k);
	else if (tbht == "bottom")
		for (int k = obj.nGroups - tbht_count; k < obj.nGroups; k++) idx.push_back(k);
	else
		for (int k = 0; k < min(10, obj.nGroups); k++) idx.push_back(k);
	if (idx.front() != 0)
	{
		ss << endl;
		for (int m = 0; m < offset; m++) ss << " ";
		ss << "... ";
	}
	if (obj.nGroups == 1) {
		if (obj.IsLogical()) cout << "(bool) ";
		ss << row(obj, 0, 0, precision);
	}
	else
	{
		if (obj.IsLogical()) cout << "(bool) ";
		ss << endl;
		for (auto k : idx)
			ss << row(obj, obj.Len() * k, offset + 1, precision);
		if (idx.back() < obj.nGroups - 1)
			ss << " ... ";
	}
	return ss.str();
}

void echo_object::print_temporal(const string& title, const CVar& obj, int offset)
{
	for (int k = 0; k < offset + 1; k++) cout << " ";
	cout << title << " " << tmarks(obj, true);
	if (tbht_count > 1) {
		int k = 0;
		for (const CTimeSeries* xp = &obj; xp ; k++, xp = xp->chain) {
			for (int m = 0; m < offset + 1; m++) cout << " ";
			cout << print_vector(*xp, offset);
		}
	}
}

void echo_object::print(const string& head, const CVar& obj, int offset)
{
	auto tp = obj.type();
	for (int k = 0; k < offset; k++) cout << " ";
	cout << "type = " << "0x" << setw(4) << setfill('0') << hex << tp << ", " << dec;
	if (tp & TYPEBIT_STRUT)
		echo_struct(head, offset).print(obj, head);
	else if (tp & TYPEBIT_CELL)
		echo_cell(head, offset).print(obj, head);
	else if (ISAUDIO(tp))
	{
		header(name=head);
		cout << endl;
		if (obj.IsStereo())
		{
			print_temporal("audio(L)", obj, offset);
			print_temporal("audio(R)", *(obj.next), offset);
		}
		else
		{
			print_temporal("audio", obj, offset);
		}
	}
	else if (ISTEMPORAL(tp) || ISSTEREO(tp))
	{
		header(head);
		cout << endl;
		if (obj.next) cout << "[L] " << endl;
		cout << make(obj, obj.GetFs() > 0, offset);
		if (obj.next)
		{
			cout << "[R] " << endl;
			cout << make(*(obj.next), obj.next->GetFs() > 0, offset);
		}
	}
	else if (!tp) // Don't do (type & TYPEBIT_NULL) unless you want to be funny!
	{
		header(head);
		cout << "[]" << postscript << endl;
	}
	else if (ISSTRING(tp) || ISBYTE(tp))
	{
		header(head);
		if (tbht_count < 0) {
			if (obj.bufType == 'S')
				cout << "\"" << obj.str() << "\"" << postscript << endl;
			else if (obj.bufType == 'B')
				cout << "(" << obj.nSamples << " bytes)" << postscript << endl;
		}
		else 
			cout << print_vector(obj, offset);
	}
	else if (ISSCALAR(tp) || ISVECTOR(tp) || IS2D(tp))
	{
		if (obj.nSamples > 1) {
			cout << "size=";
			if (IS2D(tp)) cout << obj.nGroups << "X";
			cout << obj.Len() << endl;
		}
		header(head);
		cout << print_vector(obj, offset);
	}
}