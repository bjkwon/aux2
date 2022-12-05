#include "skope.h"
#include "psycon.tab.h"
#include "skope_exception.h"

CVar* CNodeProbe::cell_indexing(CVar* pBase, AstNode* pn)
{
	size_t cellind = (size_t)(int)pbase->Compute(pn->alt->child)->value(); // check the validity of ind...probably it will be longer than this.
	ostringstream oss;
	if (pBase->type() & TYPEBIT_CELL)
	{
		if (cellind > pBase->cell.size())
		{
			oss << "Cell index " << cellind;
			throw exception_range(*pbase, pn->alt, oss.str().c_str(), pn->str);
		}
		pbase->Sig = *(psigBase = &pBase->cell.at(cellind - 1));
	}
	else
	{ // in this case x{2} means second chain
		if (cellind > pBase->CountChains())
		{
			oss << "Cell index " << cellind;
			throw exception_range(*pbase, pn->alt, oss.str().c_str(), pn->str);
		}
		CTimeSeries* pout = pBase;
		for (size_t k = 0; k < cellind; k++, pout = pout->chain) {}
		psigBase = &(pbase->Sig = *pout);
	}
	return psigBase;
}

