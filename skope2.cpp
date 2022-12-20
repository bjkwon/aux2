// AUXLAB 
//
// Copyright (c) 2009-2019 Bomjun Kwon (bjkwon at gmail)
// Licensed under the Academic Free License version 3.0
//
// Project: sigproc
// Signal Generation and Processing Library
// Platform-independent (hopefully) 
// 
// Version: 1.7
// Date: 5/24/2020

#include <sstream>
#include <list>
#include <algorithm>
#include <exception>
#include <math.h>
#include <time.h>
#include "aux_classes.h"
#include "skope.h"
#include "typecheck.h"
#include "deque"

#include <algorithm> // for lowercase
#include <assert.h>

#include "psycon.tab.h"
#include "skope_exception.h"


bool skope::builtin_func_call(CNodeProbe &diggy, AstNode *p)
{
	if (p->type == T_ID || p->type == N_STRUCT)
	{
		if (pEnv->IsValidBuiltin(p->str)) // hook bypasses IsValidBuiltin
		{
			HandleAuxFunction(p);
			return true;
		}
	}
	return false;
}

CNodeProbe::CNodeProbe(skope *past, AstNode *pnode, CVar *psig)
{
	psigBase = NULL;
	pbase = past;
	root = pnode; //what's this?
	psigBase = psig; // NULL except for T_REPLICA
	lhsref_single = false;
	lhsref = NULL;
}
