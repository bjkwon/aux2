
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 15 "psycon.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psycon.yacc.h"
#define YYPRINT(file, type, value) print_token_value (file, type, value)
/*#define DEBUG*/

char *ErrorMsg = NULL;
int yylex (void);
void yyerror (AstNode **pproot, char **errmsg, char const *s);


/* Line 189 of yacc.c  */
#line 87 "psycon.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 1
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_EOF = 0,
     T_UNKNOWN = 258,
     T_NEWLINE = 259,
     T_IF = 260,
     T_ELSE = 261,
     T_ELSEIF = 262,
     T_END = 263,
     T_WHILE = 264,
     T_FOR = 265,
     T_BREAK = 266,
     T_CONTINUE = 267,
     T_SWITCH = 268,
     T_CASE = 269,
     T_OTHERWISE = 270,
     T_FUNCTION = 271,
     T_STATIC = 272,
     T_RETURN = 273,
     T_SIGMA = 274,
     T_TRY = 275,
     T_CATCH = 276,
     T_CATCHBACK = 277,
     T_OP_SHIFT = 278,
     T_OP_CONCAT = 279,
     T_LOGIC_EQ = 280,
     T_LOGIC_NE = 281,
     T_LOGIC_LE = 282,
     T_LOGIC_GE = 283,
     T_LOGIC_AND = 284,
     T_LOGIC_OR = 285,
     T_REPLICA = 286,
     T_MATRIXMULT = 287,
     T_NUMBER = 288,
     T_STRING = 289,
     T_ID = 290,
     T_ENDPOINT = 291,
     T_FULLRANGE = 292,
     T_NEGATIVE = 294,
     T_POSITIVE = 295,
     T_LOGIC_NOT = 296,
     T_TRANSPOSE = 297
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 34 "psycon.y"

	double dval;
	char *str;
	AstNode *pnode;



/* Line 214 of yacc.c  */
#line 173 "psycon.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 118 "psycon.y"

AstNode *newAstNode(int type, YYLTYPE loc);
AstNode *makeFunctionCall(const char *name, AstNode *first, AstNode *second, YYLTYPE loc);
AstNode *makeBinaryOpNode(int op, AstNode *first, AstNode *second, YYLTYPE loc);
void print_token_value(FILE *file, int type, YYSTYPE value);
char *getT_ID_str(AstNode *p);
void handle_tilde(AstNode *proot, AstNode *pp, YYLTYPE loc);


/* Line 264 of yacc.c  */
#line 208 "psycon.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  78
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2095

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  82
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  32
/* YYNRULES -- Number of rules.  */
#define YYNRULES  149
/* YYNRULES -- Number of states.  */
#define YYNSTATES  281

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   311

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    63,     2,    49,    80,    47,     2,    39,
      59,    60,    50,    45,    57,    44,    77,    51,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    42,    58,
      40,    38,    41,     2,    48,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    78,     2,    79,    52,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    61,     2,    62,    43,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    46,    53,    54,    55,    56,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    81
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    13,    16,    18,
      21,    24,    27,    29,    31,    33,    35,    37,    39,    40,
      42,    44,    47,    52,    59,    67,    77,    78,    80,    86,
      94,    96,    98,   100,   106,   111,   118,   125,   132,   137,
     144,   152,   154,   156,   158,   160,   163,   165,   168,   169,
     174,   178,   180,   182,   186,   190,   194,   198,   202,   206,
     210,   214,   217,   221,   225,   226,   228,   232,   234,   236,
     238,   240,   244,   245,   247,   251,   254,   256,   259,   263,
     267,   273,   275,   277,   279,   281,   283,   285,   287,   289,
     291,   293,   295,   297,   299,   301,   303,   306,   309,   312,
     315,   317,   321,   326,   330,   333,   335,   340,   345,   353,
     358,   362,   364,   369,   374,   382,   385,   391,   398,   405,
     412,   419,   423,   427,   430,   433,   437,   441,   445,   447,
     449,   451,   453,   455,   458,   461,   470,   474,   478,   482,
     486,   490,   494,   498,   502,   506,   510,   514,   518,   522
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      83,     0,    -1,    -1,    84,    -1,    87,    -1,    84,    87,
      -1,    86,    -1,    85,    86,    -1,     4,    -1,     1,     4,
      -1,    94,    88,    -1,    94,    89,    -1,    86,    -1,    92,
      -1,    57,    -1,     4,    -1,     0,    -1,    58,    -1,    -1,
       0,    -1,    16,    -1,    17,    16,    -1,    91,    35,    85,
      90,    -1,    91,   110,    38,    35,    85,    90,    -1,    91,
      35,    59,   101,    60,    85,    90,    -1,    91,   110,    38,
      35,    59,   101,    60,    85,    90,    -1,    -1,     4,    -1,
      93,    14,   113,     4,    85,    -1,    93,    14,    61,   103,
      62,     4,    85,    -1,    97,    -1,   112,    -1,    99,    -1,
       5,    95,    85,    96,     8,    -1,    13,   113,    93,     8,
      -1,    13,   113,    93,    15,    85,     8,    -1,    20,    85,
      21,    35,    85,     8,    -1,    20,    85,    22,    35,    85,
       8,    -1,     9,    95,    85,     8,    -1,    10,    35,    38,
     107,    85,     8,    -1,    10,    35,    38,   107,    57,    85,
       8,    -1,    18,    -1,    11,    -1,    12,    -1,   100,    -1,
     100,    88,    -1,   113,    -1,   113,    88,    -1,    -1,     7,
      95,    85,    96,    -1,    96,     6,    85,    -1,    98,    -1,
     107,    -1,    61,   103,    62,    -1,    59,   100,    60,    -1,
     113,    40,   113,    -1,   113,    41,   113,    -1,   113,    25,
     113,    -1,   113,    26,   113,    -1,   113,    28,   113,    -1,
     113,    27,   113,    -1,    63,    97,    -1,    97,    29,    97,
      -1,    97,    30,    97,    -1,    -1,    35,    -1,   101,    57,
      35,    -1,    42,    -1,   107,    -1,    99,    -1,   102,    -1,
     103,    57,   102,    -1,    -1,   105,    -1,   104,    58,   105,
      -1,   104,    58,    -1,   107,    -1,   105,   107,    -1,   105,
      57,   107,    -1,   113,    42,   113,    -1,   113,    42,   113,
      42,   113,    -1,   113,    -1,   106,    -1,   100,    -1,    64,
      -1,    65,    -1,    66,    -1,    67,    -1,    68,    -1,    69,
      -1,    70,    -1,    71,    -1,    72,    -1,    73,    -1,    74,
      -1,    75,    -1,    38,   107,    -1,    76,   100,    -1,    76,
     113,    -1,   108,   107,    -1,    35,    -1,   111,    77,    35,
      -1,   110,    61,   113,    62,    -1,    78,   105,    79,    -1,
      80,   110,    -1,   110,    -1,    35,    59,   103,    60,    -1,
      35,    61,   113,    62,    -1,    35,    61,   113,    62,    59,
     103,    60,    -1,   110,    59,   103,    60,    -1,   110,    59,
      60,    -1,    31,    -1,    31,    59,   103,    60,    -1,    31,
      61,   113,    62,    -1,    31,    61,   113,    62,    59,   103,
      60,    -1,   111,    39,    -1,    78,   105,    79,    78,    79,
      -1,    78,   105,    79,    78,   105,    79,    -1,    78,   105,
      79,    78,   104,    79,    -1,    78,   104,    79,    78,   105,
      79,    -1,    78,   104,    79,    78,   104,    79,    -1,    78,
     104,    79,    -1,    59,   107,    60,    -1,   111,   109,    -1,
     110,   109,    -1,   110,    38,   112,    -1,   111,    38,   112,
      -1,   110,    38,    99,    -1,    99,    -1,   111,    -1,    33,
      -1,    34,    -1,    36,    -1,    44,   113,    -1,    45,   113,
      -1,    19,    59,   111,    38,   107,    57,   113,    60,    -1,
     113,    45,   113,    -1,   113,    44,   113,    -1,   113,    50,
     113,    -1,   113,    51,   113,    -1,   113,    32,   113,    -1,
     113,    52,   113,    -1,   113,    47,   113,    -1,   113,    43,
     113,    -1,   113,    49,   113,    -1,   113,    81,   113,    -1,
     113,    46,   113,    -1,   113,    48,   113,    -1,   113,    23,
     113,    -1,   113,    24,   113,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   144,   144,   145,   149,   150,   171,   178,   200,   202,
     207,   208,   215,   216,   219,   219,   219,   222,   225,   226,
     229,   234,   241,   248,   266,   273,   294,   295,   297,   312,
     329,   330,   331,   332,   358,   366,   377,   386,   395,   407,
     421,   435,   439,   441,   446,   446,   446,   446,   450,   453,
     466,   487,   490,   493,   502,   508,   510,   512,   514,   516,
     518,   520,   525,   527,   532,   535,   541,   549,   551,   552,
     555,   560,   571,   581,   588,   594,   597,   609,   615,   623,
     627,   634,   634,   634,   637,   641,   643,   645,   647,   649,
     654,   656,   658,   665,   672,   679,   688,   692,   701,   710,
     730,   735,   754,   760,   764,   773,   774,   780,   787,   796,
     801,   813,   817,   822,   828,   836,   841,   846,   852,   858,
     866,   874,   878,   887,   892,   897,   912,   927,   936,   937,
     938,   943,   948,   952,   957,   963,   971,   973,   975,   977,
     979,   981,   983,   985,   987,   989,   991,   993,   995,   997
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of text\"", "error", "$undefined", "T_UNKNOWN",
  "\"end of line\"", "\"if\"", "\"else\"", "\"elseif\"", "\"end\"",
  "\"while\"", "\"for\"", "\"break\"", "\"continue\"", "\"switch\"",
  "\"case\"", "\"otherwise\"", "\"function\"", "\"static\"", "\"return\"",
  "\"sigma\"", "\"try\"", "\"catch\"", "\"catchback\"", "\">>\"", "\"++\"",
  "\"==\"", "\"!=\"", "\"<=\"", "\">=\"", "\"&&\"", "\"||\"", "\"..\"",
  "\"**\"", "\"number\"", "\"string\"", "\"identifier\"", "T_ENDPOINT",
  "T_FULLRANGE", "'='", "'\\''", "'<'", "'>'", "':'", "'~'", "'-'", "'+'",
  "\"->\"", "'%'", "'@'", "'#'", "'*'", "'/'", "'^'", "T_NEGATIVE",
  "T_POSITIVE", "T_LOGIC_NOT", "T_TRANSPOSE", "','", "';'", "'('", "')'",
  "'{'", "'}'", "'!'", "\"+=\"", "\"-=\"", "\"*=\"", "\"/=\"", "\"@=\"",
  "\"@@=\"", "\">>=\"", "\"%=\"", "\"->=\"", "\"~=\"", "\"<>=\"", "\"#=\"",
  "\"++=\"", "'.'", "'['", "']'", "'$'", "\"<>\"", "$accept", "input",
  "block_func", "block", "line", "line_func", "eol", "eol2", "func_end",
  "func_decl", "funcdef", "case_list", "stmt", "conditional",
  "elseif_list", "expcondition", "csig", "initcell", "condition",
  "id_list", "arg", "arg_list", "matrix", "vector", "range", "exp_range",
  "compop", "assign2this", "varblock", "tid", "assign", "exp", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,    61,    39,
      60,    62,    58,   126,    45,    43,   293,    37,    64,    35,
      42,    47,    94,   294,   295,   296,   297,    44,    59,    40,
      41,   123,   125,    33,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,    46,    91,    93,
      36,   311
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    82,    83,    83,    84,    84,    85,    85,    86,    86,
      86,    86,    87,    87,    88,    88,    88,    89,    90,    90,
      91,    91,    92,    92,    92,    92,    93,    93,    93,    93,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    95,    95,    95,    95,    96,    96,
      96,    97,    98,    99,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   101,   101,   101,   102,   102,   102,
     103,   103,   104,   104,   104,   104,   105,   105,   105,   106,
     106,   107,   107,   107,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   109,   109,   109,   109,
     110,   110,   110,   110,   110,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   112,   112,   112,   112,   112,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     1,     2,     1,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     0,     1,
       1,     2,     4,     6,     7,     9,     0,     1,     5,     7,
       1,     1,     1,     5,     4,     6,     6,     6,     4,     6,
       7,     1,     1,     1,     1,     2,     1,     2,     0,     4,
       3,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     3,     0,     1,     3,     1,     1,     1,
       1,     3,     0,     1,     3,     2,     1,     2,     3,     3,
       5,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       1,     3,     4,     3,     2,     1,     4,     4,     7,     4,
       3,     1,     4,     4,     7,     2,     5,     6,     6,     6,
       6,     3,     3,     2,     2,     3,     3,     3,     1,     1,
       1,     1,     1,     2,     2,     8,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     8,     0,     0,     0,    42,    43,     0,    20,
       0,    41,     0,     0,   111,   130,   131,   100,   132,     0,
       0,     0,     0,     0,    72,     0,     0,     0,    12,     4,
       0,    13,     0,    30,    51,   128,    83,    82,    52,   105,
     129,    31,    81,     9,     0,     0,   128,    44,   105,   129,
      46,     0,     0,     0,    26,    21,     0,     0,     6,     0,
       0,     0,     0,   133,   134,    83,    52,    67,   128,    70,
       0,    68,    61,     0,    73,    76,   104,     0,     1,     5,
       0,   105,    16,    15,    14,    17,    10,    11,     0,     0,
       0,     0,     0,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,     0,     0,   124,     0,   115,
       0,   123,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    45,    47,     0,     0,    27,     0,
       0,     0,     0,     7,     0,     0,     0,     0,    54,   122,
       0,    53,    75,   121,     0,   103,    77,    64,     0,     0,
       0,    62,    63,   128,    96,   125,   110,     0,     0,    97,
      98,    99,   126,   101,   148,   149,    57,    58,    60,    59,
     140,    55,    56,    79,   143,   137,   136,   146,   142,   147,
     144,   138,   139,   141,   145,     0,     0,    38,     0,    34,
       0,     0,     0,     0,     0,   112,   113,   106,   107,    71,
      74,    72,    78,    72,   100,     0,    52,    81,    19,    22,
       0,   109,   102,     0,     0,     0,    33,     0,     0,     0,
       0,     0,    52,     0,     0,     0,     0,     0,    73,   116,
       0,    73,     0,     0,    64,     0,    80,     0,     0,     0,
      39,     0,     0,    35,     0,    36,    37,     0,     0,   120,
     119,   118,   117,    66,     0,     0,    23,    49,    40,    53,
       0,     0,   114,   108,    24,     0,     0,   135,     0,     0,
      25
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    26,    27,    57,    58,    29,    86,    87,   219,    30,
      31,   139,    32,    44,   196,    33,    34,    46,    36,   215,
      69,    70,    73,    74,    37,    38,   106,   107,    48,    49,
      41,    42
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -240
static const yytype_int16 yypact[] =
{
     823,    14,  -240,   178,   178,   -11,  -240,  -240,  1808,  -240,
      39,  -240,    16,  1651,    55,  -240,  -240,   134,  -240,  1808,
    1808,   178,   653,   178,   178,    76,    84,   860,  -240,  -240,
     224,  -240,    19,    63,  -240,    29,  -240,  -240,  -240,  2019,
    2000,  -240,  1922,  -240,  1651,    63,  -240,    52,   135,   -34,
     748,  1651,    47,   178,    22,  -240,    76,  1248,  -240,   653,
    1808,   653,  1808,    34,    34,    69,    77,  -240,    81,  -240,
      -4,    75,  -240,   -42,   306,    75,   135,   -34,  -240,  -240,
     969,   -21,  -240,  -240,  -240,  -240,  -240,  -240,   178,   178,
     178,   337,  1808,  -240,  -240,  -240,  -240,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,   178,   178,  -240,   178,  -240,
     109,  -240,  1808,  1808,  1808,  1808,  1808,  1808,  1808,  1808,
    1808,  1808,  1808,  1808,  1808,  1808,  1808,  1808,  1808,  1808,
    1808,  1808,  1808,  1032,  -240,  -240,  1349,   178,  -240,    87,
     -30,   112,   118,  -240,     3,   865,    88,  1961,  -240,  -240,
     653,  -240,   178,    97,   178,    98,    75,  1720,   653,   932,
     164,  -240,   177,    31,    75,  -240,  -240,    89,  2001,   132,
    1093,    75,  -240,  -240,    -2,   119,  1253,  1253,  1253,  1253,
     -45,  1253,  1253,   974,  1354,   119,   119,    -2,    -2,    -2,
      -2,   -45,   -45,   -45,  1253,   178,   182,  -240,  1069,  -240,
    1839,  1651,   178,  1651,  1651,  -240,    93,  -240,   149,  -240,
    1751,   178,    75,  1771,    71,   117,   123,  1880,  -240,  -240,
    1714,  -240,  -240,  1808,  1651,  1651,  -240,  1651,  1412,   653,
      74,  1450,   153,  1513,  1551,   653,   653,   -38,   422,  -240,
     -37,   646,   180,  1651,  1802,   932,  1253,  1032,  1311,  1614,
    -240,    37,  1651,  -240,  1808,  -240,  -240,   124,   125,  -240,
    -240,  -240,  -240,  -240,   932,   129,  -240,   212,  -240,   217,
    1147,  1190,  -240,  -240,  -240,  1651,  1651,  -240,   932,  1185,
    -240
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -240,  -240,  -240,   360,   160,   199,   -35,  -240,  -239,  -240,
    -240,  -240,  -240,    -3,   -17,   509,  -240,     0,     7,   -12,
      90,   -57,    -6,  -149,  -240,   609,  -240,   198,   244,    83,
     -76,   424
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -128
static const yytype_int16 yytable[] =
{
      35,    51,   144,   210,   146,   109,   266,   131,   202,   109,
      47,    47,   134,    35,   165,   135,   152,   160,    43,    82,
     152,   152,    68,    83,    52,   274,   138,    35,    65,   -32,
     118,  -127,   172,   -32,   167,  -127,   132,   153,    91,   280,
      92,   259,   261,   110,    35,   112,   113,   110,   129,   130,
     131,    35,    82,   150,   118,    55,    83,    35,   151,    68,
     150,    68,   238,   205,   241,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,    56,    84,    85,   252,   132,
      35,   -83,   -83,    40,    78,   137,   -32,   -32,  -127,  -127,
     163,    68,    88,    89,   150,   199,    40,   112,   113,   269,
     146,   200,   201,   132,   -52,   -52,   118,    14,    77,    84,
      40,    17,   169,    77,    59,   132,    60,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,    40,   -65,   148,
      61,   -65,    62,    35,    40,    53,    35,   149,   -69,   140,
      40,   -69,   112,   -69,   173,   150,   150,   203,   207,   221,
      68,   118,   235,   204,    24,   132,    25,    68,    68,    35,
      28,   -83,   -83,    40,    65,   125,   126,   127,   128,   129,
     130,   131,   251,    40,   242,   211,   213,   243,   257,   258,
     -68,   150,   150,   149,   272,   273,   242,    28,   225,   275,
     226,    40,   224,    61,    91,    62,    92,    12,    35,   220,
     132,    35,    47,    35,    35,   237,    88,   240,   236,    14,
     254,    15,    16,    17,    18,   263,    40,   143,   225,    40,
      35,   276,    19,    20,    35,    35,    79,    35,    35,    68,
     267,    35,   265,    35,    35,    68,    68,    21,   111,    22,
     209,    23,    40,    35,    39,    35,     0,    35,    35,    35,
       0,    65,    35,     0,     0,    14,    24,    39,    25,    80,
       0,     0,     0,     0,    35,     0,     0,     0,     0,    76,
      35,    39,     0,     0,    81,    35,    35,     0,    35,    35,
       0,    40,     0,    53,    40,     0,    40,    40,    39,     0,
       0,     0,     0,   143,     0,    39,   143,     0,     0,     0,
       0,    39,    24,    40,    25,     0,     0,    40,    40,     0,
      40,    40,     0,     0,    40,     0,    40,    40,     0,   143,
       0,     0,     0,     0,    39,    12,    40,     0,    40,     0,
      40,    40,    40,     0,    39,    40,     0,    14,     0,    15,
      16,    17,    18,     0,     0,     0,     0,    40,     0,     0,
      19,    20,    39,    40,     0,     0,    12,     0,    40,    40,
       0,    40,    40,   154,     0,    21,     0,    22,    14,    23,
      15,    16,    17,    18,     0,     0,     0,    39,     0,    67,
      39,    19,    20,     0,    24,   155,    25,     0,   143,     0,
       0,   143,     0,   143,   143,     0,    21,   166,    22,     0,
      23,     0,     0,    39,   133,   143,     0,   143,   143,   143,
       0,   136,     0,     0,     0,    24,     0,    25,     0,     0,
       0,     0,     0,     0,   143,     0,     0,    50,    50,     0,
     143,     0,    54,     0,     0,     0,     0,     0,   143,   143,
     159,    12,    39,    63,    64,    39,     0,    39,    39,     0,
       0,     0,     0,    14,     0,    15,    16,    17,    18,     0,
       0,     0,     0,     0,    39,     0,    19,    20,    39,    39,
       0,    39,    39,     0,     0,    39,     0,    39,    39,   154,
       0,    21,     0,    22,   145,    23,   147,    39,     0,    39,
       0,    39,    39,    39,     0,     0,    39,     0,     0,     0,
      24,   260,    25,     0,     0,     0,     0,     0,    39,     0,
       0,     0,    45,    45,    39,     0,   168,     0,     0,    39,
      39,     0,    39,    39,     0,     0,     0,     0,     0,   170,
      45,    45,    72,    45,     0,     0,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,     0,   228,     0,
       0,   231,    45,   233,   234,     0,     0,     0,    45,     0,
      45,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     245,     0,   217,    45,   247,   248,     0,   249,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   161,   162,    45,
      45,     0,     0,   264,     0,     0,     0,     0,     0,     0,
       0,     0,   270,     0,    45,    45,     0,    45,     0,    50,
       0,     0,     0,     0,   230,     0,     0,     0,     0,     0,
      66,    71,     0,    75,     0,   278,   279,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    45,   246,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    45,
       0,    45,    66,    45,     0,    12,    45,    45,    71,     0,
      71,     0,    12,     0,     0,     0,     0,    14,   271,    15,
      16,    17,    18,   156,    14,     0,    15,    16,    17,    18,
      19,    20,     0,     0,     0,    67,     0,    19,    20,   164,
      71,     0,     0,   154,    45,    21,     0,    22,     0,    23,
       0,    45,    21,     0,    22,   171,    23,   164,     0,    45,
      45,     0,    45,     0,    24,   262,    25,     0,     0,     0,
       0,    24,     0,    25,     0,     0,     0,     0,    45,     0,
       0,     0,     0,     0,    45,    45,   198,    45,    82,     0,
      45,     0,    83,    45,     0,     0,     0,     0,     0,    71,
       0,    75,     0,   212,     0,     0,   216,    71,     0,     0,
       0,   112,   113,   114,   115,   116,   117,   -81,   -81,     0,
     118,     0,     0,     0,     0,     0,     0,     0,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,     0,     0,     0,     0,    84,     0,     0,     0,     0,
       0,   232,     0,     0,     0,     0,     0,     0,     0,   156,
      75,     0,    75,    -2,     1,     0,     0,     2,     3,   132,
       0,     0,     4,     5,     6,     7,     8,     0,    71,     9,
      10,    11,    12,    13,    71,    71,     0,   156,     0,     0,
     156,     0,     0,    66,    14,     0,    15,    16,    17,    18,
      -3,     1,     0,     0,     2,     3,     0,    19,    20,     4,
       5,     6,     7,     8,     0,     0,     9,    10,    11,    12,
      13,     0,    21,     0,    22,     0,    23,     0,   112,   113,
       0,    14,     0,    15,    16,    17,    18,   118,     0,     0,
       0,    24,     0,    25,    19,    20,     0,     0,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,     0,    21,
       0,    22,     0,    23,     0,     0,     0,   206,     0,     0,
       0,     0,   218,     1,     0,     0,     2,     3,    24,     0,
      25,     4,     5,     6,     7,     8,   132,     0,   -18,   -18,
      11,    12,    13,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    14,     0,    15,    16,    17,    18,     0,
       1,     0,     0,     2,     3,     0,    19,    20,     4,     5,
       6,     7,     8,     0,     0,     0,     0,    11,    12,    13,
       0,    21,     0,    22,     0,    23,     0,   112,   113,     0,
      14,     0,    15,    16,    17,    18,   118,  -100,  -100,     0,
      24,     0,    25,    19,    20,     0,   223,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   157,     0,
     158,     0,    23,     1,     0,     0,     2,     3,   -48,   195,
     -48,     4,     5,     6,     7,     8,  -100,    24,     0,    25,
      11,    12,    13,     0,     0,   132,     0,     0,     0,     0,
       0,     0,     0,    14,     0,    15,    16,    17,    18,     0,
       1,     0,     0,     2,     3,     0,    19,    20,     4,     5,
       6,     7,     8,     0,     0,     0,     0,    11,    12,    13,
       0,    21,     0,    22,     0,    23,     0,     0,   -52,   -52,
      14,     0,    15,    16,    17,    18,     0,     0,     0,     0,
      24,     0,    25,    19,    20,     0,   112,   113,   114,   115,
     116,   117,   -81,   -81,     0,   118,   227,     0,    21,     0,
      22,     0,    23,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,    24,     1,    25,
       0,     2,     3,     0,     0,   -28,     4,     5,     6,     7,
       8,   -28,   -28,     0,     0,    11,    12,    13,     0,     0,
       0,     0,     0,     0,   132,     0,     0,     0,    14,     0,
      15,    16,    17,    18,     0,     0,     1,     0,     0,     2,
       3,    19,    20,   -29,     4,     5,     6,     7,     8,   -29,
     -29,     0,     0,    11,    12,    13,    21,     0,    22,     0,
      23,     0,     0,   112,   113,     0,    14,     0,    15,    16,
      17,    18,   118,     0,     0,    24,     0,    25,     0,    19,
      20,     0,     0,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,     0,    21,     0,    22,     0,    23,     1,
     277,     0,     2,     3,     0,     0,     0,     4,     5,     6,
       7,     8,     0,    24,     0,    25,    11,    12,    13,   141,
     142,   132,     0,     0,     0,     0,   112,   113,     0,    14,
       0,    15,    16,    17,    18,   118,     0,     0,     0,     0,
       0,     0,    19,    20,     0,     0,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,    21,     0,    22,
       0,    23,     1,     0,     0,     2,     3,   -50,     0,   -50,
       4,     5,     6,     7,     8,     0,    24,     0,    25,    11,
      12,    13,     0,     0,   132,     0,     0,     0,     0,     0,
       0,     0,    14,     0,    15,    16,    17,    18,     0,     0,
       1,     0,     0,     2,     3,    19,    20,   197,     4,     5,
       6,     7,     8,     0,     0,     0,     0,    11,    12,    13,
      21,     0,    22,     0,    23,     0,     0,   112,   113,     0,
      14,     0,    15,    16,    17,    18,   118,     0,     0,    24,
       0,    25,     0,    19,    20,     0,     0,     0,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,    21,     0,
      22,     0,    23,     1,     0,     0,     2,     3,     0,     0,
     250,     4,     5,     6,     7,     8,     0,    24,     0,    25,
      11,    12,    13,     0,     0,   132,     0,     0,     0,     0,
       0,     0,     0,    14,     0,    15,    16,    17,    18,     0,
       0,     1,     0,     0,     2,     3,    19,    20,   253,     4,
       5,     6,     7,     8,     0,     0,     0,     0,    11,    12,
      13,    21,     0,    22,     0,    23,     0,     0,     0,     0,
       0,    14,     0,    15,    16,    17,    18,     0,     0,     0,
      24,     0,    25,     0,    19,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    21,
       0,    22,     0,    23,     1,     0,     0,     2,     3,     0,
       0,   255,     4,     5,     6,     7,     8,     0,    24,     0,
      25,    11,    12,    13,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,    15,    16,    17,    18,
       0,     0,     1,     0,     0,     2,     3,    19,    20,   256,
       4,     5,     6,     7,     8,     0,     0,     0,     0,    11,
      12,    13,    21,     0,    22,     0,    23,     0,     0,     0,
       0,     0,    14,     0,    15,    16,    17,    18,     0,     0,
       0,    24,     0,    25,     0,    19,    20,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      21,     0,    22,     0,    23,     1,     0,     0,     2,     3,
       0,     0,   268,     4,     5,     6,     7,     8,     0,    24,
       0,    25,    11,    12,    13,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,     1,     0,     0,     2,     3,     0,    19,    20,
       4,     5,     6,     7,     8,     0,     0,     0,     0,    11,
      12,    13,     0,    21,     0,    22,     0,    23,     0,     0,
       0,     0,    14,     0,    15,    16,    17,    18,     0,     0,
       0,     0,    24,     0,    25,    19,    20,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      21,     0,    22,     0,    23,     1,     0,     0,     2,     3,
       0,     0,     0,     4,     5,     6,     7,     8,     0,    24,
       0,    25,    11,    12,    13,     0,     0,     0,     0,    12,
       0,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,    14,     0,    15,    16,   214,    18,     0,    19,    20,
       0,     0,    67,     0,    19,    20,     0,     0,     0,     0,
      12,     0,     0,   244,     0,    22,     0,    23,     0,    21,
       0,    22,    14,    23,    15,    16,    17,    18,     0,     0,
      12,     0,    24,     0,    25,    19,    20,     0,    24,     0,
      25,     0,    14,     0,    15,    16,    17,    18,   154,     0,
      21,     0,    22,     0,    23,    19,    20,     0,     0,     0,
       0,    12,     0,     0,     0,     0,     0,    12,     0,    24,
      21,    25,    22,    14,    23,    15,    16,   214,    18,    14,
       0,    15,    16,    17,    18,     0,    19,    20,     0,    24,
     239,    25,    19,    20,     0,     0,     0,     0,    12,     0,
       0,    21,     0,    22,     0,    23,     0,    53,     0,    22,
      14,     0,    15,    16,    17,    18,     0,     0,     0,     0,
      24,     0,    25,    19,    20,     0,    24,     0,    25,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    53,     0,
     229,     0,     0,   112,   113,   114,   115,   116,   117,     0,
       0,     0,   118,     0,     0,     0,     0,    24,     0,    25,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   208,     0,     0,   112,   113,   114,   115,   116,
     117,     0,     0,     0,   118,     0,     0,     0,     0,     0,
       0,   132,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   112,   113,     0,     0,     0,     0,
       0,     0,     0,   118,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   132,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   208,   112,   113,     0,     0,     0,     0,
       0,     0,     0,   118,     0,     0,     0,     0,   108,   109,
       0,     0,   132,     0,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,     0,     0,     0,    90,     0,     0,
       0,     0,     0,   222,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   110,    91,     0,
      92,     0,   132,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105
};

static const yytype_int16 yycheck[] =
{
       0,     4,    59,   152,    61,    39,   245,    52,    38,    39,
       3,     4,    47,    13,    90,    50,    58,    38,     4,     0,
      58,    58,    22,     4,    35,   264,     4,    27,    21,     0,
      32,     0,   108,     4,    91,     4,    81,    79,    59,   278,
      61,    79,    79,    77,    44,    23,    24,    77,    50,    51,
      52,    51,     0,    57,    32,    16,     4,    57,    62,    59,
      57,    61,   211,    60,   213,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    59,    57,    58,     4,    81,
      80,    29,    30,     0,     0,    38,    57,    58,    57,    58,
      90,    91,    29,    30,    57,     8,    13,    23,    24,    62,
     157,    14,    15,    81,    29,    30,    32,    31,    25,    57,
      27,    35,   105,    30,    59,    81,    61,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    44,    57,    60,
      59,    60,    61,   133,    51,    59,   136,    60,    57,    56,
      57,    60,    23,    62,    35,    57,    57,    35,    60,    60,
     150,    32,    59,    35,    78,    81,    80,   157,   158,   159,
       0,    29,    30,    80,   157,    46,    47,    48,    49,    50,
      51,    52,   229,    90,    57,    78,    78,    60,   235,   236,
      57,    57,    57,    60,    60,    60,    57,    27,     6,    60,
       8,   108,   195,    59,    59,    61,    61,    19,   198,    35,
      81,   201,   195,   203,   204,   211,    29,   213,    59,    31,
      57,    33,    34,    35,    36,    35,   133,    57,     6,   136,
     220,     4,    44,    45,   224,   225,    27,   227,   228,   229,
     247,   231,   244,   233,   234,   235,   236,    59,    40,    61,
     150,    63,   159,   243,     0,   245,    -1,   247,   248,   249,
      -1,   244,   252,    -1,    -1,    31,    78,    13,    80,    35,
      -1,    -1,    -1,    -1,   264,    -1,    -1,    -1,    -1,    25,
     270,    27,    -1,    -1,    30,   275,   276,    -1,   278,   279,
      -1,   198,    -1,    59,   201,    -1,   203,   204,    44,    -1,
      -1,    -1,    -1,   133,    -1,    51,   136,    -1,    -1,    -1,
      -1,    57,    78,   220,    80,    -1,    -1,   224,   225,    -1,
     227,   228,    -1,    -1,   231,    -1,   233,   234,    -1,   159,
      -1,    -1,    -1,    -1,    80,    19,   243,    -1,   245,    -1,
     247,   248,   249,    -1,    90,   252,    -1,    31,    -1,    33,
      34,    35,    36,    -1,    -1,    -1,    -1,   264,    -1,    -1,
      44,    45,   108,   270,    -1,    -1,    19,    -1,   275,   276,
      -1,   278,   279,    57,    -1,    59,    -1,    61,    31,    63,
      33,    34,    35,    36,    -1,    -1,    -1,   133,    -1,    42,
     136,    44,    45,    -1,    78,    79,    80,    -1,   228,    -1,
      -1,   231,    -1,   233,   234,    -1,    59,    60,    61,    -1,
      63,    -1,    -1,   159,    44,   245,    -1,   247,   248,   249,
      -1,    51,    -1,    -1,    -1,    78,    -1,    80,    -1,    -1,
      -1,    -1,    -1,    -1,   264,    -1,    -1,     3,     4,    -1,
     270,    -1,     8,    -1,    -1,    -1,    -1,    -1,   278,   279,
      80,    19,   198,    19,    20,   201,    -1,   203,   204,    -1,
      -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,
      -1,    -1,    -1,    -1,   220,    -1,    44,    45,   224,   225,
      -1,   227,   228,    -1,    -1,   231,    -1,   233,   234,    57,
      -1,    59,    -1,    61,    60,    63,    62,   243,    -1,   245,
      -1,   247,   248,   249,    -1,    -1,   252,    -1,    -1,    -1,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,   264,    -1,
      -1,    -1,     3,     4,   270,    -1,    92,    -1,    -1,   275,
     276,    -1,   278,   279,    -1,    -1,    -1,    -1,    -1,   105,
      21,    22,    23,    24,    -1,    -1,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,    -1,   198,    -1,
      -1,   201,    53,   203,   204,    -1,    -1,    -1,    59,    -1,
      61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     220,    -1,   158,    74,   224,   225,    -1,   227,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    90,
      91,    -1,    -1,   243,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   252,    -1,   105,   106,    -1,   108,    -1,   195,
      -1,    -1,    -1,    -1,   200,    -1,    -1,    -1,    -1,    -1,
      21,    22,    -1,    24,    -1,   275,   276,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   137,   223,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   150,
      -1,   152,    53,   154,    -1,    19,   157,   158,    59,    -1,
      61,    -1,    19,    -1,    -1,    -1,    -1,    31,   254,    33,
      34,    35,    36,    74,    31,    -1,    33,    34,    35,    36,
      44,    45,    -1,    -1,    -1,    42,    -1,    44,    45,    90,
      91,    -1,    -1,    57,   195,    59,    -1,    61,    -1,    63,
      -1,   202,    59,    -1,    61,   106,    63,   108,    -1,   210,
     211,    -1,   213,    -1,    78,    79,    80,    -1,    -1,    -1,
      -1,    78,    -1,    80,    -1,    -1,    -1,    -1,   229,    -1,
      -1,    -1,    -1,    -1,   235,   236,   137,   238,     0,    -1,
     241,    -1,     4,   244,    -1,    -1,    -1,    -1,    -1,   150,
      -1,   152,    -1,   154,    -1,    -1,   157,   158,    -1,    -1,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    -1,    -1,    -1,    -1,    57,    -1,    -1,    -1,    -1,
      -1,   202,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   210,
     211,    -1,   213,     0,     1,    -1,    -1,     4,     5,    81,
      -1,    -1,     9,    10,    11,    12,    13,    -1,   229,    16,
      17,    18,    19,    20,   235,   236,    -1,   238,    -1,    -1,
     241,    -1,    -1,   244,    31,    -1,    33,    34,    35,    36,
       0,     1,    -1,    -1,     4,     5,    -1,    44,    45,     9,
      10,    11,    12,    13,    -1,    -1,    16,    17,    18,    19,
      20,    -1,    59,    -1,    61,    -1,    63,    -1,    23,    24,
      -1,    31,    -1,    33,    34,    35,    36,    32,    -1,    -1,
      -1,    78,    -1,    80,    44,    45,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    -1,    59,
      -1,    61,    -1,    63,    -1,    -1,    -1,    62,    -1,    -1,
      -1,    -1,     0,     1,    -1,    -1,     4,     5,    78,    -1,
      80,     9,    10,    11,    12,    13,    81,    -1,    16,    17,
      18,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,
       1,    -1,    -1,     4,     5,    -1,    44,    45,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    -1,    18,    19,    20,
      -1,    59,    -1,    61,    -1,    63,    -1,    23,    24,    -1,
      31,    -1,    33,    34,    35,    36,    32,    38,    39,    -1,
      78,    -1,    80,    44,    45,    -1,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    -1,    59,    -1,
      61,    -1,    63,     1,    -1,    -1,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    77,    78,    -1,    80,
      18,    19,    20,    -1,    -1,    81,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,
       1,    -1,    -1,     4,     5,    -1,    44,    45,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    -1,    18,    19,    20,
      -1,    59,    -1,    61,    -1,    63,    -1,    -1,    29,    30,
      31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,
      78,    -1,    80,    44,    45,    -1,    23,    24,    25,    26,
      27,    28,    29,    30,    -1,    32,    57,    -1,    59,    -1,
      61,    -1,    63,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    -1,    78,     1,    80,
      -1,     4,     5,    -1,    -1,     8,     9,    10,    11,    12,
      13,    14,    15,    -1,    -1,    18,    19,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    81,    -1,    -1,    -1,    31,    -1,
      33,    34,    35,    36,    -1,    -1,     1,    -1,    -1,     4,
       5,    44,    45,     8,     9,    10,    11,    12,    13,    14,
      15,    -1,    -1,    18,    19,    20,    59,    -1,    61,    -1,
      63,    -1,    -1,    23,    24,    -1,    31,    -1,    33,    34,
      35,    36,    32,    -1,    -1,    78,    -1,    80,    -1,    44,
      45,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    59,    -1,    61,    -1,    63,     1,
      60,    -1,     4,     5,    -1,    -1,    -1,     9,    10,    11,
      12,    13,    -1,    78,    -1,    80,    18,    19,    20,    21,
      22,    81,    -1,    -1,    -1,    -1,    23,    24,    -1,    31,
      -1,    33,    34,    35,    36,    32,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    -1,    -1,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    -1,    59,    -1,    61,
      -1,    63,     1,    -1,    -1,     4,     5,     6,    -1,     8,
       9,    10,    11,    12,    13,    -1,    78,    -1,    80,    18,
      19,    20,    -1,    -1,    81,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,    -1,
       1,    -1,    -1,     4,     5,    44,    45,     8,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    -1,    18,    19,    20,
      59,    -1,    61,    -1,    63,    -1,    -1,    23,    24,    -1,
      31,    -1,    33,    34,    35,    36,    32,    -1,    -1,    78,
      -1,    80,    -1,    44,    45,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    -1,    59,    -1,
      61,    -1,    63,     1,    -1,    -1,     4,     5,    -1,    -1,
       8,     9,    10,    11,    12,    13,    -1,    78,    -1,    80,
      18,    19,    20,    -1,    -1,    81,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,
      -1,     1,    -1,    -1,     4,     5,    44,    45,     8,     9,
      10,    11,    12,    13,    -1,    -1,    -1,    -1,    18,    19,
      20,    59,    -1,    61,    -1,    63,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      78,    -1,    80,    -1,    44,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,    61,    -1,    63,     1,    -1,    -1,     4,     5,    -1,
      -1,     8,     9,    10,    11,    12,    13,    -1,    78,    -1,
      80,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,
      -1,    -1,     1,    -1,    -1,     4,     5,    44,    45,     8,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    -1,    18,
      19,    20,    59,    -1,    61,    -1,    63,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,    -1,
      -1,    78,    -1,    80,    -1,    44,    45,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    -1,    61,    -1,    63,     1,    -1,    -1,     4,     5,
      -1,    -1,     8,     9,    10,    11,    12,    13,    -1,    78,
      -1,    80,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,     1,    -1,    -1,     4,     5,    -1,    44,    45,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    -1,    18,
      19,    20,    -1,    59,    -1,    61,    -1,    63,    -1,    -1,
      -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,    -1,
      -1,    -1,    78,    -1,    80,    44,    45,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    -1,    61,    -1,    63,     1,    -1,    -1,     4,     5,
      -1,    -1,    -1,     9,    10,    11,    12,    13,    -1,    78,
      -1,    80,    18,    19,    20,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    31,    -1,    33,    34,    35,    36,    -1,    44,    45,
      -1,    -1,    42,    -1,    44,    45,    -1,    -1,    -1,    -1,
      19,    -1,    -1,    59,    -1,    61,    -1,    63,    -1,    59,
      -1,    61,    31,    63,    33,    34,    35,    36,    -1,    -1,
      19,    -1,    78,    -1,    80,    44,    45,    -1,    78,    -1,
      80,    -1,    31,    -1,    33,    34,    35,    36,    57,    -1,
      59,    -1,    61,    -1,    63,    44,    45,    -1,    -1,    -1,
      -1,    19,    -1,    -1,    -1,    -1,    -1,    19,    -1,    78,
      59,    80,    61,    31,    63,    33,    34,    35,    36,    31,
      -1,    33,    34,    35,    36,    -1,    44,    45,    -1,    78,
      79,    80,    44,    45,    -1,    -1,    -1,    -1,    19,    -1,
      -1,    59,    -1,    61,    -1,    63,    -1,    59,    -1,    61,
      31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,
      78,    -1,    80,    44,    45,    -1,    78,    -1,    80,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,
      61,    -1,    -1,    23,    24,    25,    26,    27,    28,    -1,
      -1,    -1,    32,    -1,    -1,    -1,    -1,    78,    -1,    80,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    62,    -1,    -1,    23,    24,    25,    26,    27,
      28,    -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    81,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    81,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    62,    23,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    38,    39,
      -1,    -1,    81,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    -1,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    62,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    59,    -1,
      61,    -1,    81,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     4,     5,     9,    10,    11,    12,    13,    16,
      17,    18,    19,    20,    31,    33,    34,    35,    36,    44,
      45,    59,    61,    63,    78,    80,    83,    84,    86,    87,
      91,    92,    94,    97,    98,    99,   100,   106,   107,   110,
     111,   112,   113,     4,    95,    97,    99,   100,   110,   111,
     113,    95,    35,    59,   113,    16,    59,    85,    86,    59,
      61,    59,    61,   113,   113,   100,   107,    42,    99,   102,
     103,   107,    97,   104,   105,   107,   110,   111,     0,    87,
      35,   110,     0,     4,    57,    58,    88,    89,    29,    30,
      38,    59,    61,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,   108,   109,    38,    39,
      77,   109,    23,    24,    25,    26,    27,    28,    32,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    81,    85,    88,    88,    85,    38,     4,    93,
     111,    21,    22,    86,   103,   113,   103,   113,    60,    60,
      57,    62,    58,    79,    57,    79,   107,    59,    61,    85,
      38,    97,    97,    99,   107,   112,    60,   103,   113,   100,
     113,   107,   112,    35,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,     7,    96,     8,   107,     8,
      14,    15,    38,    35,    35,    60,    62,    60,    62,   102,
     105,    78,   107,    78,    35,   101,   107,   113,     0,    90,
      35,    60,    62,    42,    95,     6,     8,    57,    85,    61,
     113,    85,   107,    85,    85,    59,    59,   104,   105,    79,
     104,   105,    57,    60,    59,    85,   113,    85,    85,    85,
       8,   103,     4,     8,    57,     8,     8,   103,   103,    79,
      79,    79,    79,    35,    85,   101,    90,    96,     8,    62,
      85,   113,    60,    60,    90,    60,     4,    60,    85,    85,
      90
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (pproot, errmsg, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, pproot, errmsg); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, AstNode **pproot, char **errmsg)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pproot, errmsg)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    AstNode **pproot;
    char **errmsg;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (pproot);
  YYUSE (errmsg);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, AstNode **pproot, char **errmsg)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, pproot, errmsg)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    AstNode **pproot;
    char **errmsg;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pproot, errmsg);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, AstNode **pproot, char **errmsg)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, pproot, errmsg)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    AstNode **pproot;
    char **errmsg;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , pproot, errmsg);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, pproot, errmsg); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, AstNode **pproot, char **errmsg)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, pproot, errmsg)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    AstNode **pproot;
    char **errmsg;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (pproot);
  YYUSE (errmsg);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 34: /* "\"string\"" */

/* Line 1000 of yacc.c  */
#line 111 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding string \"%s\"\n", (yyvaluep->str));
#endif
  free((yyvaluep->str));
};

/* Line 1000 of yacc.c  */
#line 1762 "psycon.tab.c"
	break;
      case 35: /* "\"identifier\"" */

/* Line 1000 of yacc.c  */
#line 111 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding string \"%s\"\n", (yyvaluep->str));
#endif
  free((yyvaluep->str));
};

/* Line 1000 of yacc.c  */
#line 1776 "psycon.tab.c"
	break;
      case 84: /* "block_func" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1790 "psycon.tab.c"
	break;
      case 85: /* "block" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1804 "psycon.tab.c"
	break;
      case 86: /* "line" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1818 "psycon.tab.c"
	break;
      case 87: /* "line_func" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1832 "psycon.tab.c"
	break;
      case 91: /* "func_decl" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1846 "psycon.tab.c"
	break;
      case 92: /* "funcdef" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1860 "psycon.tab.c"
	break;
      case 93: /* "case_list" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1874 "psycon.tab.c"
	break;
      case 94: /* "stmt" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1888 "psycon.tab.c"
	break;
      case 95: /* "conditional" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1902 "psycon.tab.c"
	break;
      case 96: /* "elseif_list" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1916 "psycon.tab.c"
	break;
      case 97: /* "expcondition" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1930 "psycon.tab.c"
	break;
      case 98: /* "csig" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1944 "psycon.tab.c"
	break;
      case 99: /* "initcell" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1958 "psycon.tab.c"
	break;
      case 100: /* "condition" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1972 "psycon.tab.c"
	break;
      case 101: /* "id_list" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1986 "psycon.tab.c"
	break;
      case 102: /* "arg" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2000 "psycon.tab.c"
	break;
      case 103: /* "arg_list" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2014 "psycon.tab.c"
	break;
      case 104: /* "matrix" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2028 "psycon.tab.c"
	break;
      case 105: /* "vector" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2042 "psycon.tab.c"
	break;
      case 106: /* "range" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2056 "psycon.tab.c"
	break;
      case 107: /* "exp_range" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2070 "psycon.tab.c"
	break;
      case 108: /* "compop" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2084 "psycon.tab.c"
	break;
      case 109: /* "assign2this" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2098 "psycon.tab.c"
	break;
      case 110: /* "varblock" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2112 "psycon.tab.c"
	break;
      case 111: /* "tid" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2126 "psycon.tab.c"
	break;
      case 112: /* "assign" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2140 "psycon.tab.c"
	break;
      case 113: /* "exp" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2154 "psycon.tab.c"
	break;

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (AstNode **pproot, char **errmsg);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (AstNode **pproot, char **errmsg)
#else
int
yyparse (pproot, errmsg)
    AstNode **pproot;
    char **errmsg;
#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

/* User initialization code.  */

/* Line 1242 of yacc.c  */
#line 95 "psycon.y"
{
  if (ErrorMsg) {
	free(ErrorMsg);
	ErrorMsg = NULL;
  }
  *errmsg = NULL;
}

/* Line 1242 of yacc.c  */
#line 2315 "psycon.tab.c"

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 144 "psycon.y"
    { *pproot = NULL;;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 146 "psycon.y"
    { *pproot = (yyvsp[(1) - (1)].pnode);;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 151 "psycon.y"
    {
		if ((yyvsp[(2) - (2)].pnode)) {
			if ((yyvsp[(1) - (2)].pnode) == NULL)
				(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
			else if ((yyvsp[(1) - (2)].pnode)->type == N_BLOCK)
			{
				(yyvsp[(1) - (2)].pnode)->tail->next = (yyvsp[(2) - (2)].pnode);
				(yyvsp[(1) - (2)].pnode)->tail = (yyvsp[(2) - (2)].pnode);
			}
			else
			{ // a=1; b=2; ==> $1->type is '='. So first, a N_BLOCK tree should be made.
				(yyval.pnode) = newAstNode(N_BLOCK, (yyloc));
				(yyval.pnode)->next = (yyvsp[(1) - (2)].pnode);
				(yyvsp[(1) - (2)].pnode)->next = (yyval.pnode)->tail = (yyvsp[(2) - (2)].pnode);
			}
		} else
			(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 172 "psycon.y"
    {
		if ((yyvsp[(1) - (1)].pnode)) // if cond1, x=1, end ==> x=1 comes here.
			(yyval.pnode) = (yyvsp[(1) - (1)].pnode);
		else
			(yyval.pnode) = newAstNode(N_BLOCK, (yyloc));
	;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 179 "psycon.y"
    {
		if ((yyvsp[(2) - (2)].pnode)) {
			if ((yyvsp[(1) - (2)].pnode)->type == N_BLOCK) {
				if ((yyval.pnode)->next) {
					(yyvsp[(1) - (2)].pnode)->tail->next = (yyvsp[(2) - (2)].pnode);
					(yyvsp[(1) - (2)].pnode)->tail = (yyvsp[(2) - (2)].pnode);
				} else {
					(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
					free((yyvsp[(1) - (2)].pnode));
				}
			} else { //if the second argument doesn't have N_BLOCK, make one
				(yyval.pnode) = newAstNode(N_BLOCK, (yyloc));
				(yyval.pnode)->next = (yyvsp[(1) - (2)].pnode);
				(yyvsp[(1) - (2)].pnode)->next = (yyval.pnode)->tail = (yyvsp[(2) - (2)].pnode);
			}
		}
		else // only "block" is given
			(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 201 "psycon.y"
    { (yyval.pnode) = NULL;;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 203 "psycon.y"
    {
		(yyval.pnode) = NULL;
		yyerrok;
	;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 209 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
		(yyval.pnode)->suppress=1;
	;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 230 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FUNCTION, (yyloc));
		(yyval.pnode)->suppress = 2;
	;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 235 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FUNCTION, (yyloc));
		(yyval.pnode)->suppress = 3;
	;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 242 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (4)].pnode);
		(yyval.pnode)->str = (yyvsp[(2) - (4)].str);
		(yyval.pnode)->child = newAstNode(N_IDLIST, (yyloc));
		(yyval.pnode)->child->next = (yyvsp[(3) - (4)].pnode);
	;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 249 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (6)].pnode);
		(yyval.pnode)->str = (yyvsp[(4) - (6)].str);
		(yyval.pnode)->child = newAstNode(N_IDLIST, (yyloc));
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
		if ((yyvsp[(2) - (6)].pnode)->type!=N_VECTOR)
		{
			(yyval.pnode)->alt = newAstNode(N_VECTOR, (yylsp[(2) - (6)]));
			AstNode *p = newAstNode(N_VECTOR, (yylsp[(2) - (6)]));
			p->alt = p->tail = (yyvsp[(2) - (6)].pnode);
			(yyval.pnode)->alt->str = (char*)p;
		}
		else
		{
			(yyval.pnode)->alt = (yyvsp[(2) - (6)].pnode);
		}
	;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 267 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (7)].pnode);
		(yyval.pnode)->str = (yyvsp[(2) - (7)].str);
		(yyval.pnode)->child = (yyvsp[(4) - (7)].pnode);
		(yyvsp[(4) - (7)].pnode)->next = (yyvsp[(6) - (7)].pnode);
	;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 274 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (9)].pnode);
		(yyval.pnode)->str = (yyvsp[(4) - (9)].str);
		(yyval.pnode)->child = (yyvsp[(6) - (9)].pnode);
		(yyvsp[(6) - (9)].pnode)->next = (yyvsp[(8) - (9)].pnode);
		if ((yyvsp[(2) - (9)].pnode)->type!=N_VECTOR)
		{
			(yyval.pnode)->alt = newAstNode(N_VECTOR, (yylsp[(2) - (9)]));
			AstNode *p = newAstNode(N_VECTOR, (yylsp[(2) - (9)]));
			p->alt = p->tail = (yyvsp[(2) - (9)].pnode);
			(yyval.pnode)->alt->str = (char*)p;
		}
		else
		{
			(yyval.pnode)->alt = (yyvsp[(2) - (9)].pnode);
		}
	;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 294 "psycon.y"
    { (yyval.pnode) = newAstNode(T_SWITCH, (yyloc));;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 296 "psycon.y"
    { (yyval.pnode) = newAstNode(T_SWITCH, (yyloc));;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 298 "psycon.y"
    {
		if ((yyvsp[(1) - (5)].pnode)->alt)
			(yyvsp[(1) - (5)].pnode)->tail->alt = (yyvsp[(3) - (5)].pnode);
		else
			(yyvsp[(1) - (5)].pnode)->alt = (yyvsp[(3) - (5)].pnode);
		AstNode *p = (yyvsp[(5) - (5)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(5) - (5)]));
			p->next = (yyvsp[(5) - (5)].pnode);
		}
		(yyvsp[(1) - (5)].pnode)->tail = (yyvsp[(3) - (5)].pnode)->next = p;
		(yyval.pnode) = (yyvsp[(1) - (5)].pnode);
	;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 313 "psycon.y"
    {
		if ((yyvsp[(1) - (7)].pnode)->alt)
			(yyvsp[(1) - (7)].pnode)->tail->alt = (yyvsp[(4) - (7)].pnode);
		else
			(yyvsp[(1) - (7)].pnode)->alt = (yyvsp[(4) - (7)].pnode);
		AstNode *p = (yyvsp[(7) - (7)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(7) - (7)]));
			p->next = (yyvsp[(7) - (7)].pnode);
		}
		(yyvsp[(1) - (7)].pnode)->tail = (yyvsp[(4) - (7)].pnode)->next = p;
		(yyval.pnode) = (yyvsp[(1) - (7)].pnode);
	;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 333 "psycon.y"
    { // This works, too, for "if cond, act; end" without else, because elseif_list can be empty
		(yyval.pnode) = newAstNode(T_IF, (yyloc));
		AstNode *p = (yyvsp[(3) - (5)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(3) - (5)]));
			p->next = (yyvsp[(3) - (5)].pnode);
		}
		(yyval.pnode)->child = (yyvsp[(2) - (5)].pnode);
		(yyvsp[(2) - (5)].pnode)->next = p;
		AstNode *pElse = (yyvsp[(4) - (5)].pnode);
		if (pElse->type!=N_BLOCK)
		{
			pElse = newAstNode(N_BLOCK, (yylsp[(4) - (5)]));
			pElse->next = (yyvsp[(4) - (5)].pnode);
		}
		(yyval.pnode)->alt = pElse;
		if ((yyvsp[(4) - (5)].pnode)->child==NULL && (yyvsp[(4) - (5)].pnode)->next==NULL) // When elseif_list is empty, T_IF is made, but no child and next
		{
			yydeleteAstNode((yyval.pnode)->alt, 1);
			(yyval.pnode)->alt=NULL;
		}
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 359 "psycon.y"
    { // case is cascaded through alt
		(yyval.pnode) = (yyvsp[(3) - (4)].pnode);
		(yyval.pnode)->alt = (yyvsp[(3) - (4)].pnode)->alt;
		(yyval.pnode)->child = (yyvsp[(2) - (4)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 367 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(3) - (6)].pnode);
		(yyval.pnode)->alt = (yyvsp[(3) - (6)].pnode)->alt;
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		AstNode *p = newAstNode(T_OTHERWISE, (yylsp[(5) - (6)]));
		p->next = (yyvsp[(5) - (6)].pnode);
		(yyval.pnode)->tail = (yyvsp[(3) - (6)].pnode)->tail->alt = p;
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 378 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_TRY, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->alt = newAstNode(T_CATCH, (yylsp[(4) - (6)]));
		(yyval.pnode)->alt->child = newAstNode(T_ID, (yylsp[(4) - (6)]));
		(yyval.pnode)->alt->child->str = (yyvsp[(4) - (6)].str);
		(yyval.pnode)->alt->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 387 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_TRY, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->alt = newAstNode(T_CATCHBACK, (yylsp[(4) - (6)]));
		(yyval.pnode)->alt->child = newAstNode(T_ID, (yylsp[(4) - (6)]));
		(yyval.pnode)->alt->child->str = (yyvsp[(4) - (6)].str);
		(yyval.pnode)->alt->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 396 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_WHILE, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (4)].pnode);
		AstNode *p = (yyvsp[(3) - (4)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(3) - (4)]));
			p->next = (yyvsp[(3) - (4)].pnode);
		}
		(yyval.pnode)->alt = p;
	;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 408 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FOR, (yyloc));
		(yyval.pnode)->child = newAstNode(T_ID, (yylsp[(2) - (6)]));
		(yyval.pnode)->child->str = (yyvsp[(2) - (6)].str);
		(yyval.pnode)->child->child = (yyvsp[(4) - (6)].pnode);
		AstNode *p = (yyvsp[(5) - (6)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(5) - (6)]));
			p->next = (yyvsp[(5) - (6)].pnode);
		}
		(yyval.pnode)->alt = p;
	;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 422 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FOR, (yyloc));
		(yyval.pnode)->child = newAstNode(T_ID, (yylsp[(2) - (7)]));
		(yyval.pnode)->child->str = (yyvsp[(2) - (7)].str);
		(yyval.pnode)->child->child = (yyvsp[(4) - (7)].pnode);
		AstNode *p = (yyvsp[(6) - (7)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(6) - (7)]));
			p->next = (yyvsp[(6) - (7)].pnode);
		}
		(yyval.pnode)->alt = p;
	;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 436 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_RETURN, (yyloc));
	;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 440 "psycon.y"
    { (yyval.pnode) = newAstNode(T_BREAK, (yyloc));;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 442 "psycon.y"
    { (yyval.pnode) = newAstNode(T_CONTINUE, (yyloc));;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 450 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_IF, (yyloc));
	;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 454 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_IF, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (4)].pnode);
		AstNode *p = (yyvsp[(3) - (4)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(3) - (4)]));
			p->next = (yyvsp[(3) - (4)].pnode);
		}
		(yyvsp[(2) - (4)].pnode)->next = p;
		(yyval.pnode)->alt = (yyvsp[(4) - (4)].pnode);
	;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 467 "psycon.y"
    {
		AstNode *p = (yyvsp[(3) - (3)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(3) - (3)]));
			p->next = (yyvsp[(3) - (3)].pnode);
		}
		if ((yyvsp[(1) - (3)].pnode)->child==NULL) // if there's no elseif; i.e., elseif_list is empty
		{
			yydeleteAstNode((yyvsp[(1) - (3)].pnode), 1);
			(yyval.pnode) = p;
		}
		else
		{
			(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
			(yyvsp[(1) - (3)].pnode)->alt = p;
		}
	;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 494 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->type = N_INITCELL;
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 503 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 509 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('<', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 511 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('>', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 513 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_EQ, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 515 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_NE, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 517 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_GE, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 519 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_LE, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 521 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_LOGIC_NOT, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 526 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_AND, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 528 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_OR, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 532 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_IDLIST, (yyloc));
	;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 536 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_IDLIST, (yyloc));
		(yyval.pnode)->child = (yyval.pnode)->tail = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->tail->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 542 "psycon.y"
    {
		(yyvsp[(1) - (3)].pnode)->tail = (yyvsp[(1) - (3)].pnode)->tail->next = newAstNode(T_ID, (yylsp[(3) - (3)]));
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		(yyval.pnode)->tail->str = (yyvsp[(3) - (3)].str);
	;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 550 "psycon.y"
    {	(yyval.pnode) = newAstNode(T_FULLRANGE, (yyloc)); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 556 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_ARGS, (yyloc));
		(yyval.pnode)->tail = (yyval.pnode)->child = (yyvsp[(1) - (1)].pnode);
	;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 561 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		if ((yyval.pnode)->tail)
			(yyval.pnode)->tail = (yyval.pnode)->tail->next = (yyvsp[(3) - (3)].pnode);
		else
			(yyval.pnode)->tail = (yyval.pnode)->next = (yyvsp[(3) - (3)].pnode);
	;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 571 "psycon.y"
    {
	// N_MATRIX consists of "outer" N_MATRIX--alt for dot notation
	// and "inner" N_VECTOR--alt for all successive items thru next
	// the str field of the outer N_MATRIX node is cast to the inner N_VECTOR.
	// this "fake" str pointer is freed during normal clean-up
	// 11/4/2019
		(yyval.pnode) = newAstNode(N_MATRIX, (yyloc));
		AstNode * p = newAstNode(N_VECTOR, (yyloc));
		(yyval.pnode)->str = (char*)p;
	;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 582 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_MATRIX, (yyloc));
		AstNode * p = newAstNode(N_VECTOR, (yyloc));
		p->alt = p->tail = (yyvsp[(1) - (1)].pnode);
		(yyval.pnode)->str = (char*)p;
	;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 589 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		AstNode * p = (AstNode *)(yyvsp[(1) - (3)].pnode)->str;
		p->tail = p->tail->next = (AstNode *)(yyvsp[(3) - (3)].pnode);
	;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 598 "psycon.y"
    {
	// N_VECTOR consists of "outer" N_VECTOR--alt for dot notation
	// and "inner" N_VECTOR--alt for all successive items thru next
	// Because N_VECTOR doesn't use str, the inner N_VECTOR is created there and cast for further uses.
	// this "fake" str pointer is freed during normal clean-up
	// 11/4/2019
		(yyval.pnode) = newAstNode(N_VECTOR, (yyloc));
		AstNode * p = newAstNode(N_VECTOR, (yyloc));
		p->alt = p->tail = (yyvsp[(1) - (1)].pnode);
		(yyval.pnode)->str = (char*)p;
	;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 610 "psycon.y"
    {
		AstNode * p = (AstNode *)(yyvsp[(1) - (2)].pnode)->str;
		p->tail = p->tail->next = (yyvsp[(2) - (2)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 616 "psycon.y"
    {
		AstNode * p = (AstNode *)(yyvsp[(1) - (3)].pnode)->str;
		p->tail = p->tail->next = (yyvsp[(3) - (3)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
	;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 624 "psycon.y"
    {
		(yyval.pnode) = makeFunctionCall(":", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));
	;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 628 "psycon.y"
    {
		(yyval.pnode) = makeFunctionCall(":", (yyvsp[(1) - (5)].pnode), (yyvsp[(5) - (5)].pnode), (yyloc));
		(yyvsp[(5) - (5)].pnode)->next = (yyvsp[(3) - (5)].pnode);
	;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 638 "psycon.y"
    {
		(yyval.pnode) = newAstNode('+', (yyloc));
	;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 642 "psycon.y"
    { 		(yyval.pnode) = newAstNode('-', (yyloc));	;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 644 "psycon.y"
    { 		(yyval.pnode) = newAstNode('*', (yyloc));	;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 646 "psycon.y"
    { 		(yyval.pnode) = newAstNode('/', (yyloc));	;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 648 "psycon.y"
    { 		(yyval.pnode) = newAstNode('@', (yyloc));	;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 650 "psycon.y"
    {
		(yyval.pnode) = newAstNode('@', (yyloc));
		(yyval.pnode)->child = newAstNode('@', (yyloc));
	;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 655 "psycon.y"
    { 		(yyval.pnode) = newAstNode(T_OP_SHIFT, (yyloc));	;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 657 "psycon.y"
    { 		(yyval.pnode) = newAstNode('%', (yyloc));	;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 659 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "movespec");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 666 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "respeed");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 673 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "timestretch");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 680 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "pitchscale");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 689 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 693 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_OP_CONCAT, (yyloc));
		AstNode *p = (yyval.pnode);
		if (p->alt)
			p = p->alt;
		p->child = 	newAstNode(T_REPLICA, (yylsp[(2) - (2)]));
		p->tail = p->child->next = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 702 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_OP_CONCAT, (yyloc));
		AstNode *p = (yyval.pnode);
		if (p->alt)
			p = p->alt;
		p->child = 	newAstNode(T_REPLICA, (yylsp[(2) - (2)]));
		p->tail = p->child->next = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 711 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
		if ((yyval.pnode)->child) // compop should be "@@=" and $$->child->type should be '@'  (64)
		{
			(yyval.pnode)->child->child = newAstNode(T_REPLICA, (yyloc));
			(yyval.pnode)->child->tail = (yyval.pnode)->child->child->next = newAstNode(T_REPLICA, (yyloc));
			(yyval.pnode)->tail = (yyval.pnode)->child->next = (yyvsp[(2) - (2)].pnode);
		}
		else
		{
			AstNode *p = (yyval.pnode);
			if (p->alt)
				p = p->alt;
			p->child = 	newAstNode(T_REPLICA, (yylsp[(2) - (2)]));
			p->tail = p->child->next = (yyvsp[(2) - (2)].pnode);
		}
	;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 731 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 736 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		AstNode *p = newAstNode(N_STRUCT, (yyloc));
		p->str = (yyvsp[(3) - (3)].str);
		if ((yyval.pnode)->type==N_CELL)
		{
			(yyval.pnode)->alt->alt = p; //always initiating
			(yyval.pnode)->tail = p; // so that next concatenation can point to p, even thought p is "hidden" underneath $$->alt
		}
		if ((yyval.pnode)->tail)
		{
			(yyval.pnode)->tail = (yyval.pnode)->tail->alt = p;
		}
		else
		{
			(yyval.pnode)->tail = (yyval.pnode)->alt = p;
		}
	;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 755 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (4)].pnode);
		(yyval.pnode)->tail = (yyval.pnode)->alt->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->tail->child = (yyvsp[(3) - (4)].pnode);
	;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 761 "psycon.y"
    {//tid-vector --> what's this comment? 12/30/2020
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
	;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 765 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_HOOK, (yyloc));
		(yyval.pnode)->str = (char*)calloc(1, strlen((yyvsp[(2) - (2)].pnode)->str)+1);
		strcpy((yyval.pnode)->str, (yyvsp[(2) - (2)].pnode)->str);
		(yyval.pnode)->alt = (yyvsp[(2) - (2)].pnode)->alt;
	;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 775 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (4)].str);
		handle_tilde((yyval.pnode), (yyvsp[(3) - (4)].pnode), (yylsp[(3) - (4)]));
	;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 781 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (4)].str);
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->alt->child = (yyvsp[(3) - (4)].pnode);
	;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 788 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (7)].str);
		(yyval.pnode)->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->alt->child = (yyvsp[(3) - (7)].pnode);
		handle_tilde((yyval.pnode)->alt, (yyvsp[(6) - (7)].pnode), (yylsp[(6) - (7)]));
		(yyval.pnode)->tail = (yyval.pnode)->alt->alt; // we need this; or tail is broken and can't put '.' tid at the end
	;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 797 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (4)].pnode);
		handle_tilde((yyval.pnode), (yyvsp[(3) - (4)].pnode), (yylsp[(3) - (4)]));
	;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 802 "psycon.y"
    {
		if ((yyval.pnode)->alt != NULL  && (yyval.pnode)->alt->type==N_STRUCT)
		{ // dot notation with a blank parentheses, e.g., a.sqrt() or (1:2:5).sqrt()
			(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		}
		else // no longer used.,,.. absorbed by tid:  T_ID
		{ // udf_func()
			(yyval.pnode) = newAstNode(N_CALL, (yyloc));
			(yyval.pnode)->str = getT_ID_str((yyvsp[(1) - (3)].pnode));
		}
	;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 814 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
	;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 818 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
		handle_tilde((yyval.pnode), (yyvsp[(3) - (4)].pnode), (yylsp[(3) - (4)]));
	;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 823 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->alt->child = (yyvsp[(3) - (4)].pnode);
	;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 829 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
		(yyval.pnode)->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->alt->child = (yyvsp[(3) - (7)].pnode);
		handle_tilde((yyval.pnode)->alt, (yyvsp[(6) - (7)].pnode), (yylsp[(6) - (7)]));
		(yyval.pnode)->tail = (yyval.pnode)->alt->alt; // we need this; or tail is broken and can't put '.' tid at the end
	;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 837 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_TRANSPOSE, (yyloc));
 		(yyval.pnode)->child = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 842 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (5)].pnode);
	;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 847 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 853 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 859 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->str = (char*)malloc(8);
		strcpy((yyval.pnode)->str, "R");
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 867 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->str = (char*)malloc(8);
		strcpy((yyval.pnode)->str, "R");
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 875 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
	;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 879 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 888 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
		(yyval.pnode)->child = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 893 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
		(yyval.pnode)->child = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 898 "psycon.y"
    { //c=a(2)=44
		if (!(yyvsp[(1) - (3)].pnode)->child)
			(yyvsp[(1) - (3)].pnode)->child = (yyvsp[(3) - (3)].pnode);
		else
			for (AstNode *p = (yyvsp[(1) - (3)].pnode)->child; p; p=p->child)
			{
				if (!p->child)
				{
					p->child = (yyvsp[(3) - (3)].pnode);
					break;
				}
			}
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
	;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 913 "psycon.y"
    { //a(2)=d=11
		if (!(yyvsp[(1) - (3)].pnode)->child)
			(yyvsp[(1) - (3)].pnode)->child = (yyvsp[(3) - (3)].pnode);
		else
			for (AstNode *p = (yyvsp[(1) - (3)].pnode)->child; p; p=p->child)
			{
				if (!p->child)
				{
					p->child = (yyvsp[(3) - (3)].pnode);
					break;
				}
			}
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
	;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 928 "psycon.y"
    { // x={"bjk",noise(300), 4.5555}
		(yyval.pnode)->str = getT_ID_str((yyvsp[(1) - (3)].pnode));
		(yyval.pnode)->child = (yyvsp[(3) - (3)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 939 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_NUMBER, (yyloc));
		(yyval.pnode)->dval = (yyvsp[(1) - (1)].dval);
	;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 944 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_STRING, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 949 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ENDPOINT, (yyloc));
	;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 953 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_NEGATIVE, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 958 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 964 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_SIGMA, (yyloc));
		(yyval.pnode)->child = newAstNode(T_ID, (yylsp[(3) - (8)]));
		(yyval.pnode)->child->str = getT_ID_str((yyvsp[(3) - (8)].pnode));
		(yyval.pnode)->child->child = (yyvsp[(5) - (8)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(7) - (8)].pnode);
	;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 972 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('+', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 974 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('-', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 976 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('*', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 978 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('/', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 980 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_MATRIXMULT, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 982 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("^", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 984 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("mod", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 986 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("respeed", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 988 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("pitchscale", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 990 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("timestretch", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 992 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("movespec", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 994 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('@', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 996 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_OP_SHIFT, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 998 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_OP_CONCAT, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;



/* Line 1455 of yacc.c  */
#line 3875 "psycon.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (pproot, errmsg, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (pproot, errmsg, yymsg);
	  }
	else
	  {
	    yyerror (pproot, errmsg, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, pproot, errmsg);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, pproot, errmsg);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (pproot, errmsg, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, pproot, errmsg);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, pproot, errmsg);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1016 "psycon.y"


/* Called by yyparse on error. */
void yyerror (AstNode **pproot, char **errmsg, char const *s)
{
  static size_t errmsg_len = 0;
#define ERRMSG_MAX 999
  char msgbuf[ERRMSG_MAX], *p;
  size_t msglen;

  sprintf(msgbuf, "Invalid syntax: Line %d, Col %d: %s.\n", yylloc.first_line, yylloc.first_column, s + (strncmp(s, "syntax error, ", 14) ? 0 : 14));
  if ((p=strstr(msgbuf, "$undefined"))) {
	sprintf(p, "'%c'(%d)", yychar, yychar);
    strcpy(p+strlen(p), p+10);
  }
  if ((p=strstr(msgbuf, "end of text or ")))
    strcpy(p, p+15);
  if ((p=strstr(msgbuf, " or ','")))
    strcpy(p, p+7);
  msglen = strlen(msgbuf);
  if (ErrorMsg == NULL)
    errmsg_len = 0;
  ErrorMsg = (char *)realloc(ErrorMsg, errmsg_len+msglen+1);
  strcpy(ErrorMsg+errmsg_len, msgbuf);
  errmsg_len += msglen;
  *errmsg = ErrorMsg;
}


int getTokenID(const char *str)
{
	size_t len, i;
	len = strlen(str);
	for (i = 0; i < YYNTOKENS; i++) {
		if (yytname[i] != 0
			&& yytname[i][0] == '"'
			&& !strncmp (yytname[i] + 1, str, len)
			&& yytname[i][len + 1] == '"'
			&& yytname[i][len + 2] == 0)
				break;
	}
	if (i < YYNTOKENS)
		return yytoknum[i];
	else
		return T_UNKNOWN;
}


void print_token_value(FILE *file, int type, YYSTYPE value)
{
	if (type == T_ID)
		fprintf (file, "%s", value.str);
	else if (type == T_NUMBER)
		fprintf (file, "%f", value.dval);
}


char *getAstNodeName(AstNode *p)
{
#define N_NAME_MAX 99
  static char buf[N_NAME_MAX];

  if (!p)
	return NULL;
  switch (p->type) {
  case '=':
    sprintf(buf, "[%s=]", p->str);
    break;
  case T_ID:
    sprintf(buf, "[%s]", p->str);
    break;
  case T_STRING:
    sprintf(buf, "\"%s\"", p->str);
    break;
  case N_CALL:
    sprintf(buf, "%s()", p->str);
    break;
  case N_CELL:
    sprintf(buf, "%s()", p->str);
    break;
  case T_NUMBER:
    sprintf(buf, "%.1f", p->dval);
    break;
  case N_BLOCK:
    sprintf(buf, "BLOCK");
    break;
  case N_ARGS:
    sprintf(buf, "ARGS");
    break;
  case N_MATRIX:
    sprintf(buf, "MATRIX");
    break;
  case N_VECTOR:
    sprintf(buf, "VECTOR");
    break;
  case N_IDLIST:
    sprintf(buf, "ID_LIST");
    break;
  case N_TIME_EXTRACT:
    sprintf(buf, "TIME_EXTRACT");
    break;
  case N_CELLASSIGN:
    sprintf(buf, "INITCELL");
    break;
  case N_IXASSIGN:
    sprintf(buf, "ASSIGN1");
    break;
  default:
    if (YYTRANSLATE(p->type) == 2)
      sprintf(buf, "[%d]", p->type);
    else
      sprintf(buf, "%s", yytname[YYTRANSLATE(p->type)]);
  }
  return buf;
}

/* As of 4/17/2018
In makeFunctionCall and makeBinaryOpNode,
node->tail is removed, because it caused conflict with tail made in
tid: tid '.' T_ID or tid: tid '(' arg_list ')'
or possibly other things.
The only downside from this change is, during debugging, the last argument is not seen at the top node where several nodes are cascaded: e.g., a+b+c
*/

AstNode *makeFunctionCall(const char *name, AstNode *first, AstNode *second, YYLTYPE loc)
{
	AstNode *node;

	node = newAstNode(T_ID, loc);
	node->str = (char*)calloc(1, strlen(name)+1);
	strcpy(node->str, name);
	node->tail = node->alt = newAstNode(N_ARGS, loc);
	node->alt->child = first;
	first->next = second;
	return node;
}

AstNode *makeBinaryOpNode(int op, AstNode *first, AstNode *second, YYLTYPE loc)
{
	AstNode *node;

	node = newAstNode(op, loc);
	node->child = first;
	first->next = second;
	return node;
}

AstNode *newAstNode(int type, YYLTYPE loc)
{
#ifdef DEBUG
    static int cnt=0;
#endif
  AstNode *node;

  node = (AstNode *)malloc(sizeof(AstNode));
  if (node==NULL)
    exit(2);
  memset(node, 0, sizeof(AstNode));
  node->type = type;
#ifdef DEBUG
    printf("created node %d: %s\n", ++cnt, getAstNodeName(node));
#endif
  node->line = loc.first_line;
  node->col = loc.first_column;
  return node;
}

char *getT_ID_str(AstNode *p)
{
	if (p->type==T_ID)
		return p->str;
	printf("Must be T_ID\n");
	return NULL;
}

void handle_tilde(AstNode *proot, AstNode *pp, YYLTYPE loc)
{
	AstNode *p = pp->child;
	if (p->type==T_ID && !strcmp(p->str,"respeed"))
	{ // x{2}(t1~t2) checks here because t1~t2 can be arg_list through
        AstNode *q = newAstNode(N_TIME_EXTRACT, loc);
		q->child = p->alt->child;
		q->child->next = p->alt->child->next;
        if (proot->tail)
            proot->tail = proot->tail->alt = q;
		else
			proot->tail = proot->alt = q;
		p->alt->child = NULL;
		yydeleteAstNode(p, 1);
	}
	else
	{
	    if (proot->tail)
			proot->tail = proot->tail->alt = pp;
		else
			proot->tail = proot->alt = pp;
	}
}

int yydeleteAstNode(AstNode *p, int fSkipNext)
{
#ifdef DEBUG
    static int cnt=0;
#endif
  AstNode *tmp, *next;

  if (!p)
	return 0;
#ifdef DEBUG
    printf("deleting node %d: %s\n", ++cnt, getAstNodeName(p));
#endif
  if (p->str)
    free(p->str);
  if (p->child)
    yydeleteAstNode(p->child, 0);
  if (!fSkipNext && p->next) {
	for (tmp=p->next; tmp; tmp=next) {
      next = tmp->next;
      yydeleteAstNode(tmp, 1);
    }
  }
  free(p);
  return 0;
}

int yyPrintf(const char *msg, AstNode *p)
{
	if (p)
		printf("[%16s]token type: %d, %s, \n", msg, p->type, p->str);
	return 1;
}
