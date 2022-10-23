// AUXLAB 
//
// Copyright (c) 2009-2019 Bomjun Kwon (bjkwon at gmail)
// Licensed under the Academic Free License version 3.0
//
// Project: sigproc
// Signal Generation and Processing Library
// Platform-independent (hopefully) 
// 
// Version: 1.704
// Date: 1/1/2021

#include <sstream>
#include <list>
#include <algorithm>
#include <exception>
#include <math.h>
#include <time.h>
#include "aux_classes.h"
#include "skope.h"
#include "skope_exception.h"
#include "psycon.tab.h"

extern vector<skope*> xscope;

AstNode* goto_try_line(const AstNode* pnode, int line)
{
	AstNode* p = (AstNode*)pnode;
	for (; p; p = p->next)
	{
		if (p->type == T_IF || p->type == T_FOR || p->type == T_WHILE || p->type == T_TRY) // line should be inside of block, i.e., T_FOR T_IF or T_WHILE
		{
//			pp = skope::goto_line((const AstNode*)p->child->next, line);
//			if (pp) return pp;
		}
		if (p->line == line) return p;
	}
	return p;
}

const AstNode* get_try_node(const AstNode* pnode)
{
	const AstNode* p = pnode;
	// for a base udf, pnode is N_BLOCK; otherwise, it is T_FUNCTION
	if (p->type == T_FUNCTION) p = p->child->next;
	for (; p; p = p->next)
		if (p->type == T_TRY)
			return p;
	return NULL;
}

const AstNode* get_base_node_for_try (const AstNode* pnode, int line)
{// Get the node of try from the t_func node of the base udf
	// First, find out in which (local) function try was called
	const AstNode* p = pnode;
	for (; p; p = p->next)
	{
		if (!p->next)
			return p;
		else if (line < p->next->line)
			return p;
	}
	return NULL;
}

CVar* skope::Try_here(const AstNode* pnode, AstNode* p)
{ // to be used only for udf 
	try {
		process_statement(p);
	}
	catch (const skope_exception& e) {
		// If an exception is thrown inside of a try in a udf, e carries the information 
		// at the time of exception, including where it occurred, the xtree node info, etc,
		// with inTryCatch set. That information is copied to the variable specified in
		// catch, and the udf proceeds to the bottom of CallUDF(), to the next line
		// i.e., the first line of catch.
		// For other exceptions, this is bypassed and e is further thrown to the user
		// to be captured by xcom 1/1/2021
		if (pEnv->inTryCatch)
		{ // Make a new struct variable from child of e.pTarget (which is T_CATCH)
			auto baseudf = get_base_node_for_try(e.pCtx->u.t_func_base, p->line);
			auto pnode_try = get_try_node(baseudf);
			const char* name = pnode_try->alt->child->str; // the variable name of catch (as "catchme" in catch "catchme")
			string errmsg = e.outstr;
			size_t id = errmsg.find("[GOTO_BASE]");
			if (id != string::npos) errmsg = errmsg.substr(id + string("[GOTO_BASE]").size());
			CVar msg(errmsg);
			SetVar("full", &msg, &Vars[name]);
			msg = e.basemsg;
			SetVar("base", &msg, &Vars[name]);
			errmsg = e.msgonly;
			if (id != string::npos) errmsg = errmsg.substr(id + string("[GOTO_BASE]").size());
			msg = errmsg;
			SetVar("body", &msg, &Vars[name]);
			msg = e.udffile;
			SetVar("source", &msg, &Vars[name]);
			msg.SetValue((float)e.line);
			SetVar("errline", &msg, &Vars[name]);
			msg.SetValue((float)e.col);
			SetVar("errcol", &msg, &Vars[name]);
			process_statement(pnode_try->alt);
			skope* pbaskope = NULL;
			for (auto xs : xscope) {
				if (xs->level == e.pCtx->level)
					pbaskope = xs;
			}
			if (pnode_try->alt->type == T_CATCHBACK)
			{	//	e.pCtx->pEnv->BLOCK((skope*)pbaskope, pbaskope->pTryLast->next);
				auto tblock = (AstNode*)malloc(sizeof(AstNode));
				memset(tblock, 0, sizeof(AstNode));
				tblock->type = N_BLOCK;
				tblock->next = pbaskope->pTryLast->next->next;
				auto ttry = (AstNode*)malloc(sizeof(AstNode));
				memset(ttry, 0, sizeof(AstNode));
				ttry->type = T_TRY;
				ttry->line = pbaskope->pTryLast->next->line;
				ttry->col = pbaskope->pTryLast->next->col;
				ttry->child = tblock;
				ttry->alt = pnode_try->alt;
				e.pCtx->pEnv->TRY((skope*)pbaskope, ttry);
				ttry->child = ttry->alt = NULL;
				free(tblock);
				free(ttry);
			}
		}
		else
			throw e;
	}
	return &Sig;
}