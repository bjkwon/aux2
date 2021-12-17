#include "aux_classes.h"
#include "skope.h"

static int countVectorItems(const AstNode* pnode)
{
	if (pnode->type != N_VECTOR) return 0;
	AstNode* p = ((AstNode*)pnode->str)->alt;
	int res = 0;
	for (; p; p = p->next)
		res++;
	return res;
}

// get_output_count: needed to handle a statment with a function with multiple output vars
int get_output_count(const AstNode* proot, const AstNode* pnode)
{
	//we need tree pointer to the one calling the current statement
	// To do...check the logic flow----12/29/2020
	// pCurLine should be one of the following:
	// 1) no child 
	// 2) child 
	// 3) N_BLOCK
	// 4) T_IF, T_FOR, T_WHILE, or T_SWITCH

	// Check the logic for cases 
	// i) command line: x=[2 1 0 8 10]\n x.max
	// ii) command line in block: x=[2 1 0 8 10]\n str="bk",ff=sqrt(2); x.max
	// iii) used in udf

	/* 1) no child --checked
	*  2) child --checked
	*  3) N_BLOCK --checked
	*  4) 4) T_IF, T_FOR, T_WHILE, or T_SWITCH -- not yet
	* i) command line -- checked
	* ii) command line in block -- checked
	* iii) used in udf -- checked
	* 03/22/2021
	*/

	auto pCurLine = skope::goto_line(proot, pnode->line);
	int nOutVars = 0;
	// if pCurLine is looping such as T_IF, T_FOR, or T_WHILE
	if (skope::IsLooping(pCurLine))
	{
		// if a.max == val, output should be one 
		// if b = a.max == val or if [a,b] = a.max should have been rejected as a syntax error in psycon.l
			nOutVars = 1;
	}
	else if (pnode->type == N_STRUCT)
	{
		auto body = skope::findDadNode(pCurLine, pnode);
		auto lhs = skope::findDadNode(pCurLine, body);
		if (lhs->type == N_ARGS)
			nOutVars = 0;
		else if (lhs->type == N_VECTOR)
			nOutVars = countVectorItems(lhs);
		else if (lhs == body) // no LHS
			nOutVars = 1;
		//		else
//			throw "unchecked logic flow";
	}
	else
	{
		auto lhs = skope::findDadNode(pCurLine, pnode);
		if (lhs == pnode) // no LHS
			nOutVars = 1;
		else  if (lhs->type == N_ARGS)
			nOutVars = 0;
		else if (lhs->type == N_VECTOR)
			nOutVars = countVectorItems(lhs);
		//		else
//			throw "unchecked logic flow";
	}
	return nOutVars;
}

const AstNode* get_first_arg(const AstNode* pnode, bool struct_call)
{
	const AstNode* p = NULL;
	if (pnode->alt)
	{
		if (struct_call)
			p = pnode;
		else
			p = pnode->alt->child;
	}
	return p;
}

const AstNode* get_second_arg(const AstNode* pnode, bool struct_call)
{
	const AstNode* p = NULL;
	if (!struct_call)
	{
		if (pnode->alt)
			p = pnode->alt->child->next;
	}
	else
	{
		if (pnode->alt)
			p = pnode->alt->child;
		else
			p = NULL;
	}
	return p;
}
