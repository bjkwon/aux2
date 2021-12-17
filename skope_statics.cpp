#include "skope.h"

const AstNode* skope::findDadNode(const AstNode* p, const AstNode* pME)
{
	if (!p) return NULL;
	if (p == pME) return p;
	if (p->type == N_BLOCK)
	{
		p = p->next;
		const AstNode* res = findDadNode(p, pME);
		while (!res)
		{
			if (!p) break;
			p = p->next;
			res = findDadNode(p, pME);
		}
		return res;
	}
	if (!p->child && !p->alt) return NULL;
	if (p->child == pME) return p;
	const AstNode* res = NULL;
	do
	{
		if (p->child)
			res = findParentNode(p->child, res);
		else
			res = findParentNode(p->alt, res);
		if (!res) break;
	} while (res->type == N_STRUCT);
	if (res) return res;
	if (p->type == N_VECTOR)
	{
		auto pp = (const AstNode*)(p->str);
		if (!pp)
		{// p is "true" N_VECTOR -- where p->alt and the success on nexts are actual elements
			for (auto p2 = p->alt; p2; p2 = p2->next)
			{
				if (p2 == pME) return p;
			}
			return NULL;
		}
		else
		{
			if (pp == pME) return p;
			res = findDadNode(pp, pME);
			if (res) return res;
		}
	}
	return NULL;
}

const AstNode* skope::findParentNode(const AstNode* p, const AstNode* pME, bool altonly)
{
	if (p)
	{
		if (!p->child && !p->alt) return NULL;
		if (!altonly && p->child == pME) return p;
		if (p->alt == pME) return p;
		if (!altonly && p->child)
			return findParentNode(p->child, pME, altonly);
		else
			return findParentNode(p->alt, pME, altonly);
	}
	return NULL;
}

AstNode* skope::goto_line(const AstNode* pnode, int line)
{
	AstNode* pp, * p = (AstNode*)pnode;
	for (; p; p = p->next)
	{
		if (p->line == line) return p;
		if (p->type == T_FOR || p->type == T_WHILE) // line should be inside of block, i.e., T_FOR T_IF or T_WHILE
		{
			pp = skope::goto_line((const AstNode*)p->alt, line);
			if (pp) return pp;
		}
		else if (p->type == T_IF)
		{
			pp = skope::goto_line((const AstNode*)p->child->next, line);
			if (pp) return pp;
		}
	}
	return p;
}

bool skope::IsLooping(const AstNode* pnode)
{
	if (pnode->type == T_IF || pnode->type == T_FOR || pnode->type == T_WHILE) return true;
	return false;
}
