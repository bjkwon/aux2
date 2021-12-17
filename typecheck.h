#pragma once
void blockCellStruct1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname="", int id=0);
void blockCellStruct2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id = 0);
void blockString1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname = "", int id = 0);
void blockString2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id = 0);
void blockLogical1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname = "", int id = 0);
void blockLogical2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id = 0);
void ensureVector1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname = "", int id = 0);
void ensureVector2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id = 0);
void ensureVector3(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname);

void ensureScalar1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname = "", int id = 0);
void ensureScalar2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id = 0);
void ensureAudio1(const skope& sc, const AstNode* pnode, const CVar& checkthis, const string& fname = "", int id = 0);
void ensureAudio2(const skope& sc, const AstNode* pnode, const CVar& checkthis, int id = 0);

