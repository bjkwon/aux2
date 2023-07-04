#include "skope.h"

/* How to add new functions to the list of built in functions

typedef void(*fGate) (skope* past, const AstNode* pnode, const vector<CVar>& args);

Case 1) One gate function to one built-in, Cfunction decl (exclusive)
Given gate function decl:
   void _tone(skope* past, const AstNode* pnode, const vector<CVar>& args);
and Cfunction decl:
   Cfunction set_builtin_function_tone(fGate fp); --> arg number/types of tone() are registered here.

In CAstSigEnv::InitBuiltInFunctions(),
	builtin.emplace("tone", set_builtin_function_tone(&_tone)); 
simplified to -->
	SET_BUILTIN_FUNC("tone", tone)
    (1st arg: aux func name in quotation;  2nd arg: gate function in the source code, no quotation)

    DECL_GATE(_tone) in builtin_functions.h

Case 2) Multiple gate functions to multiple built-in's, shared Cfunction decl
Multiple gate functions:
    void _imaginary_unit(skope* past, const AstNode* pnode, const vector<CVar>& args);
    void _pi(skope* past, const AstNode* pnode, const vector<CVar>& args);
    void _natural_log_base(skope* past, const AstNode* pnode, const vector<CVar>& args);
One Cfunction decl:
    Cfunction set_builtin_function_constant(fGate fp);

In CAstSigEnv::InitBuiltInFunctions(),
	pseudo_vars.emplace("i", set_builtin_function_constant(&_imaginary_unit));
	pseudo_vars.emplace("pi", set_builtin_function_constant(&_pi));
	pseudo_vars.emplace("e", set_builtin_function_constant(&_natural_log_base));

	DECL_GATE(_constant) in builtin_functions.h

Case 3) One gate function to multiple built-in's, shared Cfunction decl
Gate function:
    void _tparamonly(skope* past, const AstNode* pnode, const vector<CVar>& args);
One Cfunction decl:
	Cfunction set_builtin_function_tparamonly(fGate fp)
In CAstSigEnv::InitBuiltInFunctions(),
	builtin.emplace("noise", set_builtin_function_tone(&_tparamonly));
	builtin.emplace("gnoise", set_builtin_function_tone(&_tparamonly));
Simplify -->
	SET_BUILTIN_FUNC("noise", tparamonly);
	SET_BUILTIN_FUNC("gnoise", tparamonly);

    DECL_GATE(_tparamonly) in builtin_functions.h

Case 4) One AUX build-in function, multiple calling prototype (i.e., multiple gate functions)
If an aux built-in function offers multiple prototypes---
Gate functions:
void _andor(skope* past, const AstNode* pnode, const vector<CVar>& args)
void _andor2(skope* past, const AstNode* pnode, const vector<CVar>& args)
Cfunction decls:
Cfunction set_builtin_function_andor(fGate fp)
Cfunction set_builtin_function_andor2(fGate fp)
In CAstSigEnv::InitBuiltInFunctions(),
	builtin.emplace("and", set_builtin_function_tone(&_andor));
	builtin.emplace("and", set_builtin_function_tone(&_andor2));
Simplify -->
	SET_BUILTIN_FUNC("and", andor);
	SET_BUILTIN_FUNC("and", andor2);
Add to builtin_functions.h
	DECL_GATE(_andor)
	DECL_GATE(_andor2) 

	9/27/2022
*/

/* types
* 0 NULL
* 1 scalar
* 2 vector
* TYPEBIT_AUDIO + 2 audio
* TYPEBIT_SIZE1 + TYPEBIT_FS2 string
*/

//these functions are defined in _file.cpp
void _fopen(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _fclose(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _fprintf(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _fread(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _fwrite(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _write(skope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_wavwrite)
DECL_GATE(_wave)
void _file(skope* past, const AstNode* pnode, const vector<CVar>& args);

//string CAstSigEnv::AppPath = "";
map<string, Cfunction> dummy_pseudo_vars;
map<string, Cfunction> CAstSigEnv::pseudo_vars = dummy_pseudo_vars;

//#ifdef NO_PLAYSND // for aux_builtin_ext
//skope::play_block_ms = 0;
//skope::record_block_ms = 0;
//skope::record_bytes = 0;
//#endif


DECL_GATE(_run)


DECL_GATE(_movespec)

DECL_GATE(_tparamonly)
DECL_GATE(_rand)
DECL_GATE(_irand)
DECL_GATE(_randperm)
DECL_GATE(_andor)
DECL_GATE(_andor2)
DECL_GATE(_mostleast)
DECL_GATE(_sort)
void _sprintf(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _inputdlg(skope* past, const AstNode* pnode, const vector<CVar>& args);
void aux_input(skope* past, const AstNode* pnode, const vector<CVar>& args);
void udf_error(skope* past, const AstNode* pnode, const vector<CVar>& args);
void udf_warning(skope* past, const AstNode* pnode, const vector<CVar>& args);
void udf_rethrow(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _msgbox(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _include(skope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_eval)
DECL_GATE(_include)
DECL_GATE(_group)
DECL_GATE(_ungroup)
DECL_GATE(_clear)
DECL_GATE(_dir)
DECL_GATE(_veq)
DECL_GATE(_datatype)
DECL_GATE(_noargs)
void _interp1(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _fdelete(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _isaudioat(skope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_play)
DECL_GATE(_play2)
DECL_GATE(_stop_pause_resume)
DECL_GATE(_fft)
DECL_GATE(_ifft)
DECL_GATE(_tone)
DECL_GATE(_diff)
DECL_GATE(_cumsum)
DECL_GATE(_cellstruct)
DECL_GATE(_structbase)
DECL_GATE(_tseqget)
DECL_GATE(_tseqset)
DECL_GATE(_error_warning)
DECL_GATE(_test)
DECL_GATE(_str2num)

void _fm(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _str2num(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _esc(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _varcheck(skope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_ramp)
void _std(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _size(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _mostleast(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _setnextchan(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _setfs(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _conv(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _imaginary_unit(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _pi(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _natural_log_base(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _boolconst(skope* past, const AstNode* pnode, const vector<CVar>& args);

DECL_GATE(_pow)
DECL_GATE(_mod)
DECL_GATE(_minmax)
DECL_GATE(_sums)
DECL_GATE(_lens)
DECL_GATE(_rmsetc)
DECL_GATE(_onezero)
DECL_GATE(_sam)
DECL_GATE(_blackman)
DECL_GATE(_hamming)
DECL_GATE(_filt)
DECL_GATE(_iir)
DECL_GATE(_conv)
DECL_GATE(_audio)
DECL_GATE(_vector)
DECL_GATE(_leftright)
DECL_GATE(_hilbenvlope)
DECL_GATE(_printf)
DECL_GATE(_fprintf)
DECL_GATE(_fopen)
DECL_GATE(_fclose)
DECL_GATE(_fwrite)
DECL_GATE(_fread)
DECL_GATE(_write)
DECL_GATE(_file)
DECL_GATE(_json)
DECL_GATE(_objchecker)
DECL_GATE(_setnextchan)

DECL_GATE(_resample)

DECL_GATE(_constant)

void _sqrt(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _sin(skope* past, const AstNode* pnode, const vector<CVar>& args);

