#include "skope.h"

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


void _figure(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _axes(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _text(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _plot(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _line(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _delete_graffy(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _repaint(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _showrms(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _replicate(skope* past, const AstNode* pnode, const vector<CVar>& args);

DECL_GATE(_movespec)

void _minmax(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _filt(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _iir(skope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_tparamonly)
void _rand(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _irand(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _randperm(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _sprintf(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _record(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _play(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _pause_resume(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _stop(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _inputdlg(skope* past, const AstNode* pnode, const vector<CVar>& args);
void aux_input(skope* past, const AstNode* pnode, const vector<CVar>& args);
void udf_error(skope* past, const AstNode* pnode, const vector<CVar>& args);
void udf_warning(skope* past, const AstNode* pnode, const vector<CVar>& args);
void udf_rethrow(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _msgbox(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _dir(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _include(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _eval(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _zeros(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _ones(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _cell(skope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_group)
DECL_GATE(_ungroup)
//DECL_GATE(_buffer)
void _interp1(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _fdelete(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _ismember(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _isaudioat(skope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_fft)
DECL_GATE(_ifft)
DECL_GATE(_tone)
void _fm(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _tsq_getvalues(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _tsq_setvalues(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _tsq_gettimes(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _tsq_settimes(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _tsq_isrel(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _str2num(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _esc(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _datatype(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _veq(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _varcheck(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _and(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _or(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _sort(skope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_ramp)
void _std(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _size(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _mostleast(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _setnextchan(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _getfs(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _setfs(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _erase(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _head(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _cumsum(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _diff(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _conv(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _imaginary_unit(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _pi(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _natural_log_base(skope* past, const AstNode* pnode, const vector<CVar>& args);

DECL_GATE(_pow)
DECL_GATE(_mod)
DECL_GATE(_minmax)
DECL_GATE(_sums)
DECL_GATE(_lens)
DECL_GATE(_rmsetc)
DECL_GATE(_sam)
DECL_GATE(_blackman)
DECL_GATE(_hamming)
DECL_GATE(_filt)
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

DECL_GATE(_resample)

void _sqrt(skope* past, const AstNode* pnode, const vector<CVar>& args);
void _sin(skope* past, const AstNode* pnode, const vector<CVar>& args);

//void _wavwrite(skope* past, const AstNode* pnode, const vector<CVar>& args);
