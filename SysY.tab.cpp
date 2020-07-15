/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.2"

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
#line 1 ".\\SysY.y"

/**
文法分析编写过程

1. 参考第一版文法，绝大多数非终结符与文法说明相同。由于bison需要BNF，而文法说明为EBNF
因此有一些改动。具体为，由list结尾的一些非终结符是EBNF转BNF时添加的

2. EBNF转BNF过程：
    2.1. 首先根据一些基本规则进行转化，具体为：
        + S->{A} 转化为 S->A`; A`->A`A | /null/;
        + S->[A] 转化为 S->A`; A`->A | /null/;
    2.2. 在之上的基础进行一些简化，主要的简化掉空规则

3. 语义动作内生成对应的AST节点。分析结束时，将AST赋值给全局变量absyn_root，供之后使用。
语义动作内包含三类函数：1)A_开头的AST构造函数 2)S_开头的将str转为symbol 3)特殊的U_reverseList

4. U_reverseList使用目的
    文法分析中一些规则我写成了最左推导(主要是以list结尾的文法规则)，这使得生成的AST顺序与真实情况相反，
因此需要翻转一下链表。
    改成最左推导的原因如下：
        1) 冲突较少(感觉，没证明)
        2) bison维护的解析栈相对较浅

*/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "util.h"
#include "errormsg.h"
#include "ast.h"
#include "symbol.h"

extern int yylex();
extern int yylineno;
A_decList absyn_root;

static int calculate(int left, A_binOp op, int right);
void yyerror(char*);

#define A_POS(p) A_Pos(p.first_line, p.first_column, p.last_line, p.last_column)




/* Line 189 of yacc.c  */
#line 117 "SysY.tab.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
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
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ADD = 258,
     SUB = 259,
     MUL = 260,
     DIV = 261,
     MOD = 262,
     LT = 263,
     LE = 264,
     GT = 265,
     GE = 266,
     NOT = 267,
     AND = 268,
     OR = 269,
     EQ = 270,
     NE = 271,
     ASSIGNMENT = 272,
     CONST = 273,
     INT = 274,
     CHAR = 275,
     VOID = 276,
     IF = 277,
     ELSE = 278,
     WHILE = 279,
     SWITCH = 280,
     CASE = 281,
     RETURN = 282,
     BREAK = 283,
     CONTINUE = 284,
     COMMA = 285,
     SEMICOLON = 286,
     COLON = 287,
     L_PARENTHESIS = 288,
     R_PARENTHESIS = 289,
     L_BRACKETS = 290,
     R_BRACKETS = 291,
     L_BRACE = 292,
     R_BRACE = 293,
     ID = 294,
     INTEGER = 295,
     STRING = 296,
     CHARACTER = 297
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 44 ".\\SysY.y"

    int integer;
    char* string;
    char character;
    A_var var;
    A_dec dec;
    A_stm stm;
    A_exp exp;
    A_field field;
    A_fieldList fieldList;
    A_decList decList;
    A_expList expList;
    S_symbol sym;
    A_binOp binOp;
    A_arrayInitList arrayInitList;
    A_comStmList comStmList;



/* Line 214 of yacc.c  */
#line 215 "SysY.tab.cpp"
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
#line 240 "SysY.tab.cpp"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   283

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  43
/* YYNRULES -- Number of rules.  */
#define YYNRULES  105
/* YYNRULES -- Number of states.  */
#define YYNSTATES  181

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   297

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    11,    13,    15,    17,    19,
      24,    28,    30,    32,    36,    41,    46,    50,    54,    57,
      61,    65,    67,    69,    73,    77,    79,    81,    85,    88,
      93,    97,   100,   104,   108,   110,   112,   119,   125,   132,
     138,   142,   144,   147,   151,   156,   159,   163,   167,   170,
     173,   176,   178,   180,   185,   188,   190,   192,   198,   206,
     212,   215,   218,   222,   225,   229,   231,   233,   238,   240,
     242,   246,   248,   252,   254,   258,   260,   264,   266,   268,
     270,   272,   274,   276,   278,   280,   282,   284,   286,   288,
     290,   292,   294,   296,   300,   302,   306,   308,   313,   317,
     320,   324,   326,   328,   330,   332
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      44,     0,    -1,    45,    -1,    45,    46,    -1,    45,    59,
      -1,    46,    -1,    59,    -1,    47,    -1,    54,    -1,    18,
      49,    48,    31,    -1,    48,    30,    50,    -1,    50,    -1,
      19,    -1,    85,    17,    83,    -1,    85,    51,    17,    52,
      -1,    51,    35,    83,    36,    -1,    35,    83,    36,    -1,
      37,    53,    38,    -1,    37,    38,    -1,    53,    30,    52,
      -1,    53,    30,    83,    -1,    52,    -1,    83,    -1,    49,
      55,    31,    -1,    55,    30,    56,    -1,    56,    -1,    85,
      -1,    85,    17,    78,    -1,    85,    51,    -1,    85,    51,
      17,    57,    -1,    37,    58,    38,    -1,    37,    38,    -1,
      58,    30,    78,    -1,    58,    30,    57,    -1,    78,    -1,
      57,    -1,    21,    85,    33,    60,    34,    63,    -1,    21,
      85,    33,    34,    63,    -1,    49,    85,    33,    60,    34,
      63,    -1,    49,    85,    33,    34,    63,    -1,    60,    30,
      61,    -1,    61,    -1,    49,    85,    -1,    49,    85,    62,
      -1,    62,    35,    78,    36,    -1,    35,    36,    -1,    35,
      78,    36,    -1,    37,    64,    38,    -1,    37,    38,    -1,
      64,    46,    -1,    64,    65,    -1,    46,    -1,    65,    -1,
      67,    17,    78,    31,    -1,    78,    31,    -1,    31,    -1,
      63,    -1,    22,    33,    68,    34,    65,    -1,    22,    33,
      68,    34,    65,    23,    65,    -1,    24,    33,    68,    34,
      65,    -1,    28,    31,    -1,    29,    31,    -1,    27,    78,
      31,    -1,    27,    31,    -1,    66,    30,    78,    -1,    78,
      -1,    85,    -1,    67,    35,    78,    36,    -1,    69,    -1,
      70,    -1,    69,    14,    70,    -1,    71,    -1,    70,    13,
      71,    -1,    72,    -1,    71,    74,    72,    -1,    79,    -1,
      72,    73,    79,    -1,     8,    -1,     9,    -1,    10,    -1,
      11,    -1,    15,    -1,    16,    -1,     3,    -1,     4,    -1,
       5,    -1,     6,    -1,     7,    -1,     3,    -1,     4,    -1,
      12,    -1,    79,    -1,    80,    -1,    79,    75,    80,    -1,
      81,    -1,    80,    76,    81,    -1,    82,    -1,    85,    33,
      66,    34,    -1,    85,    33,    34,    -1,    77,    81,    -1,
      33,    78,    34,    -1,    67,    -1,    84,    -1,    79,    -1,
      40,    -1,    39,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   123,   123,   127,   128,   129,   130,   134,   135,   139,
     145,   148,   152,   156,   159,   165,   168,   172,   173,   179,
     182,   185,   188,   194,   198,   199,   203,   204,   205,   208,
     214,   215,   221,   224,   227,   230,   236,   239,   242,   245,
     251,   252,   256,   257,   261,   262,   264,   268,   269,   273,
     274,   275,   276,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   294,   295,   299,   300,   304,   308,
     309,   313,   314,   318,   319,   323,   324,   328,   329,   330,
     331,   335,   336,   340,   341,   345,   346,   347,   351,   352,
     353,   357,   361,   362,   366,   367,   371,   372,   375,   378,
     384,   385,   386,   390,   394,   398
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ADD", "SUB", "MUL", "DIV", "MOD", "LT",
  "LE", "GT", "GE", "NOT", "AND", "OR", "EQ", "NE", "ASSIGNMENT", "CONST",
  "INT", "CHAR", "VOID", "IF", "ELSE", "WHILE", "SWITCH", "CASE", "RETURN",
  "BREAK", "CONTINUE", "COMMA", "SEMICOLON", "COLON", "L_PARENTHESIS",
  "R_PARENTHESIS", "L_BRACKETS", "R_BRACKETS", "L_BRACE", "R_BRACE", "ID",
  "INTEGER", "STRING", "CHARACTER", "$accept", "program", "CompUnit",
  "Decl", "ConstDecl", "ConstDefList", "BType", "ConstDef",
  "ConstSubscripts", "ConstInitVal", "ConstInitValList", "VarDecl",
  "VarDefList", "VarDef", "InitVal", "InitValList", "FuncDef",
  "FuncFParams", "FuncFParam", "ExpSubscripts", "Block", "BlockItem",
  "Stmt", "FuncRParams", "LVal", "Cond", "LOrExp", "LAndExp", "EqExp",
  "RelExp", "RelOp", "EqOp", "addOp", "mulOp", "UnaryOp", "Exp", "AddExp",
  "MulExp", "UnaryExp", "PrimaryExp", "ConstExp", "Number", "Ident", 0
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
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    43,    44,    45,    45,    45,    45,    46,    46,    47,
      48,    48,    49,    50,    50,    51,    51,    52,    52,    53,
      53,    53,    53,    54,    55,    55,    56,    56,    56,    56,
      57,    57,    58,    58,    58,    58,    59,    59,    59,    59,
      60,    60,    61,    61,    62,    62,    62,    63,    63,    64,
      64,    64,    64,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    66,    66,    67,    67,    68,    69,
      69,    70,    70,    71,    71,    72,    72,    73,    73,    73,
      73,    74,    74,    75,    75,    76,    76,    76,    77,    77,
      77,    78,    79,    79,    80,    80,    81,    81,    81,    81,
      82,    82,    82,    83,    84,    85
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     1,     1,     1,     1,     4,
       3,     1,     1,     3,     4,     4,     3,     3,     2,     3,
       3,     1,     1,     3,     3,     1,     1,     3,     2,     4,
       3,     2,     3,     3,     1,     1,     6,     5,     6,     5,
       3,     1,     2,     3,     4,     2,     3,     3,     2,     2,
       2,     1,     1,     4,     2,     1,     1,     5,     7,     5,
       2,     2,     3,     2,     3,     1,     1,     4,     1,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3,     1,     4,     3,     2,
       3,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    12,     0,     0,     2,     5,     7,     0,     8,
       6,     0,   105,     0,     1,     3,     4,     0,    25,    26,
       0,    11,     0,     0,     0,    23,     0,     0,     0,    28,
       0,     9,     0,     0,     0,     0,     0,    41,    24,    26,
      88,    89,    90,     0,   104,   101,     0,    27,    91,    92,
      94,    96,   102,    66,     0,     0,   103,     0,     0,     0,
      10,    13,     0,     0,    37,    42,     0,     0,     0,     0,
      99,    83,    84,     0,    85,    86,    87,     0,     0,    39,
       0,    16,     0,    29,     0,     0,    14,     0,     0,     0,
       0,     0,    55,    48,    51,     0,    56,     0,    52,   101,
       0,     0,    43,    40,    36,   100,     0,    93,    95,    98,
       0,    65,    38,    31,    35,     0,    34,    15,    18,    21,
       0,    22,     0,     0,    63,     0,    60,    61,    47,    49,
      50,     0,    54,    45,     0,     0,    67,     0,    97,     0,
      30,     0,    17,     0,    68,    69,    71,    73,    75,     0,
      62,     0,    46,     0,    64,    33,    32,    19,    20,     0,
       0,     0,    81,    82,     0,    77,    78,    79,    80,     0,
       0,    53,    44,    57,    70,    72,    74,    76,    59,     0,
      58
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     6,     7,    20,    35,    21,    29,    86,
     120,     9,    17,    18,    83,   115,    10,    36,    37,   102,
      96,    97,    98,   110,    45,   143,   144,   145,   146,   147,
     169,   164,    73,    77,    46,   100,    48,    49,    50,    51,
      57,    52,    53
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -95
static const yytype_int16 yypact[] =
{
     103,    21,   -95,    19,    23,   103,   -95,   -95,    19,   -95,
     -95,    19,   -95,     9,   -95,   -95,   -95,    68,   -95,    -1,
      79,   -95,    -2,    27,    19,   -95,   243,    37,   243,     2,
      19,   -95,   243,     4,    36,    19,    -8,   -95,   -95,     8,
     -95,   -95,   -95,   243,   -95,    40,   243,   -95,   111,    42,
     -95,   -95,   -95,    47,    36,    54,   111,    55,    49,   243,
     -95,   -95,    56,   125,   -95,    72,    21,    36,    85,   243,
     -95,   -95,   -95,   243,   -95,   -95,   -95,   243,   198,   -95,
      36,   -95,   167,   -95,    87,   178,   -95,    94,    97,   209,
     100,   104,   -95,   -95,   -95,    19,   -95,   156,   -95,    10,
     107,   221,    99,   -95,   -95,   -95,   106,    42,   -95,   -95,
      62,   -95,   -95,   -95,   -95,   -10,   -95,   -95,   -95,   -95,
      22,   -95,   243,   243,   -95,   108,   -95,   -95,   -95,   -95,
     -95,   243,   -95,   -95,   109,   243,   -95,   243,   -95,   231,
     -95,   241,   -95,   112,   126,   135,   110,    95,   111,   116,
     -95,   120,   -95,   119,   -95,   -95,   -95,   -95,   -95,    50,
     243,   243,   -95,   -95,   243,   -95,   -95,   -95,   -95,   243,
      50,   -95,   -95,   134,   135,   110,    95,   111,   -95,    50,
     -95
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -95,   -95,   -95,     3,   -95,   -95,     5,   131,   144,   -74,
     -95,   -95,   -95,   143,   -70,   -95,   164,   145,   122,   -95,
     -16,   -95,   -94,   -95,   -62,    53,   -95,    13,    16,    28,
     -95,   -95,   -95,   -95,   -95,   -19,   -28,   113,   -33,   -95,
     -30,   -95,     6
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      56,    99,    61,   130,    56,     8,    11,    47,    15,    13,
       8,   119,   114,    70,    19,    32,    26,    22,    64,    58,
     139,    62,    66,    14,    68,    26,    67,   131,   140,    84,
      39,    56,    27,    28,    28,    99,    22,    59,    79,    59,
       2,    65,    23,    28,   108,    69,     2,    74,    75,    76,
     106,   104,   141,    40,    41,   121,     2,    56,    12,   111,
     142,    34,    42,   116,   112,   173,    94,   157,    95,   155,
     125,    54,    87,    63,    88,    69,   178,    89,    90,    91,
      78,    92,   134,    43,    66,   180,    82,    63,    80,    12,
      44,    81,   137,    85,   148,   148,   138,    99,    24,    25,
     129,    39,    95,   165,   166,   167,   168,   101,    99,    30,
      31,   158,   151,    56,    71,    72,   153,    99,   154,   105,
     156,     1,     2,   117,     3,   162,   163,   122,    40,    41,
     123,   126,   148,   148,   135,   127,   148,    42,   132,   150,
     160,   177,   136,     1,     2,   152,   159,    87,   161,    88,
     170,   171,    89,    90,    91,   172,    92,   179,    43,    40,
      41,    60,    63,    93,    12,    44,    33,    38,    42,    16,
      40,    41,    55,   174,     1,     2,   149,   175,    87,    42,
      88,    40,    41,    89,    90,    91,   107,    92,   103,    43,
      42,     0,   176,    63,   128,    12,    44,     0,     0,     0,
      43,    40,    41,     0,    82,   113,    12,    44,     0,     0,
      42,    43,    40,    41,     0,    85,   118,    12,    44,     0,
       0,    42,     0,     0,    40,    41,     0,     0,     0,     0,
       0,    43,   109,    42,    40,    41,     0,    12,    44,     0,
     124,     0,    43,    42,    40,    41,    40,    41,    12,    44,
       0,     0,     0,    42,    43,    42,     0,   133,     0,     0,
      12,    44,     0,     0,    43,     0,     0,     0,    82,     0,
      12,    44,     0,     0,    43,     0,    43,     0,    85,     0,
      12,    44,    12,    44
};

static const yytype_int16 yycheck[] =
{
      28,    63,    32,    97,    32,     0,     1,    26,     5,     3,
       5,    85,    82,    46,     8,    17,    17,    11,    34,    17,
      30,    17,    30,     0,    43,    17,    34,    17,    38,    59,
      24,    59,    33,    35,    35,    97,    30,    35,    54,    35,
      19,    35,    33,    35,    77,    35,    19,     5,     6,     7,
      69,    67,    30,     3,     4,    85,    19,    85,    39,    78,
      38,    34,    12,    82,    80,   159,    63,   141,    63,   139,
      89,    34,    22,    37,    24,    35,   170,    27,    28,    29,
      33,    31,   101,    33,    30,   179,    37,    37,    34,    39,
      40,    36,    30,    37,   122,   123,    34,   159,    30,    31,
      97,    95,    97,     8,     9,    10,    11,    35,   170,    30,
      31,   141,   131,   141,     3,     4,   135,   179,   137,    34,
     139,    18,    19,    36,    21,    15,    16,    33,     3,     4,
      33,    31,   160,   161,    35,    31,   164,    12,    31,    31,
      14,   169,    36,    18,    19,    36,    34,    22,    13,    24,
      34,    31,    27,    28,    29,    36,    31,    23,    33,     3,
       4,    30,    37,    38,    39,    40,    22,    24,    12,     5,
       3,     4,    27,   160,    18,    19,   123,   161,    22,    12,
      24,     3,     4,    27,    28,    29,    73,    31,    66,    33,
      12,    -1,   164,    37,    38,    39,    40,    -1,    -1,    -1,
      33,     3,     4,    -1,    37,    38,    39,    40,    -1,    -1,
      12,    33,     3,     4,    -1,    37,    38,    39,    40,    -1,
      -1,    12,    -1,    -1,     3,     4,    -1,    -1,    -1,    -1,
      -1,    33,    34,    12,     3,     4,    -1,    39,    40,    -1,
      31,    -1,    33,    12,     3,     4,     3,     4,    39,    40,
      -1,    -1,    -1,    12,    33,    12,    -1,    36,    -1,    -1,
      39,    40,    -1,    -1,    33,    -1,    -1,    -1,    37,    -1,
      39,    40,    -1,    -1,    33,    -1,    33,    -1,    37,    -1,
      39,    40,    39,    40
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    18,    19,    21,    44,    45,    46,    47,    49,    54,
      59,    49,    39,    85,     0,    46,    59,    55,    56,    85,
      48,    50,    85,    33,    30,    31,    17,    33,    35,    51,
      30,    31,    17,    51,    34,    49,    60,    61,    56,    85,
       3,     4,    12,    33,    40,    67,    77,    78,    79,    80,
      81,    82,    84,    85,    34,    60,    79,    83,    17,    35,
      50,    83,    17,    37,    63,    85,    30,    34,    78,    35,
      81,     3,     4,    75,     5,     6,     7,    76,    33,    63,
      34,    36,    37,    57,    83,    37,    52,    22,    24,    27,
      28,    29,    31,    38,    46,    49,    63,    64,    65,    67,
      78,    35,    62,    61,    63,    34,    78,    80,    81,    34,
      66,    78,    63,    38,    57,    58,    78,    36,    38,    52,
      53,    83,    33,    33,    31,    78,    31,    31,    38,    46,
      65,    17,    31,    36,    78,    35,    36,    30,    34,    30,
      38,    30,    38,    68,    69,    70,    71,    72,    79,    68,
      31,    78,    36,    78,    78,    57,    78,    52,    83,    34,
      14,    13,    15,    16,    74,     8,     9,    10,    11,    73,
      34,    31,    36,    65,    70,    71,    72,    79,    65,    23,
      65
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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
      yyerror (YY_("syntax error: cannot back up")); \
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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
		  Type, Value, Location); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
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
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

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
int yyparse (void);
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
yyparse (void)
#else
int
yyparse ()

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

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

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

/* Line 1464 of yacc.c  */
#line 123 ".\\SysY.y"
    {absyn_root = (A_decList)U_reverseList((yyvsp[(1) - (1)].decList));;}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 127 ".\\SysY.y"
    {(yyval.decList) = A_castDecList((yyvsp[(2) - (2)].decList), (yyvsp[(1) - (2)].decList));;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 128 ".\\SysY.y"
    {(yyval.decList) = A_DecList((yyvsp[(2) - (2)].dec), (yyvsp[(1) - (2)].decList));;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 129 ".\\SysY.y"
    {(yyval.decList) = (yyvsp[(1) - (1)].decList);;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 130 ".\\SysY.y"
    {(yyval.decList) = A_DecList((yyvsp[(1) - (1)].dec), NULL);;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 134 ".\\SysY.y"
    {(yyval.decList) = (yyvsp[(1) - (1)].decList);;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 135 ".\\SysY.y"
    {(yyval.decList) = (yyvsp[(1) - (1)].decList);;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 139 ".\\SysY.y"
    {
        (yyval.decList) = A_conform((yyvsp[(3) - (4)].decList), (yyvsp[(2) - (4)].sym));
    ;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 145 ".\\SysY.y"
    {
        (yyval.decList) = A_DecList((yyvsp[(3) - (3)].dec), (yyvsp[(1) - (3)].decList));
    ;}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 148 ".\\SysY.y"
    {(yyval.decList) = A_DecList((yyvsp[(1) - (1)].dec), NULL);;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 152 ".\\SysY.y"
    {(yyval.sym) = S_Symbol("int");;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 156 ".\\SysY.y"
    {
        (yyval.dec) = A_VariableDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (3)].sym), (yyvsp[(3) - (3)].exp), true);
    ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 159 ".\\SysY.y"
    {
        (yyval.dec) = A_ArrayDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (4)].sym), (A_expList)U_reverseList((yyvsp[(2) - (4)].expList)), (yyvsp[(4) - (4)].arrayInitList), true);
    ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 165 ".\\SysY.y"
    {
        (yyval.expList) = A_ExpList((yyvsp[(3) - (4)].exp), (yyvsp[(1) - (4)].expList));
    ;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 168 ".\\SysY.y"
    {(yyval.expList) = A_ExpList((yyvsp[(2) - (3)].exp), NULL);;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 172 ".\\SysY.y"
    {(yyval.arrayInitList) = (A_arrayInitList)U_reverseList((yyvsp[(2) - (3)].arrayInitList));;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 173 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yyloc)), A_IntExp(A_POS((yyloc)), 0)), NULL);
    ;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 179 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[(3) - (3)])), (yyvsp[(3) - (3)].arrayInitList)), (yyvsp[(1) - (3)].arrayInitList));
    ;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 182 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[(1) - (3)])), (yyvsp[(3) - (3)].exp)), (yyvsp[(1) - (3)].arrayInitList));
    ;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 185 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[(1) - (1)])), (yyvsp[(1) - (1)].arrayInitList)), NULL);
    ;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 188 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[(1) - (1)])), (yyvsp[(1) - (1)].exp)), NULL);
    ;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 194 ".\\SysY.y"
    {(yyval.decList) = A_conform((yyvsp[(2) - (3)].decList), (yyvsp[(1) - (3)].sym));;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 198 ".\\SysY.y"
    {(yyval.decList) = A_DecList((yyvsp[(3) - (3)].dec), (yyvsp[(1) - (3)].decList));;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 199 ".\\SysY.y"
    {(yyval.decList) = A_DecList((yyvsp[(1) - (1)].dec), NULL);;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 203 ".\\SysY.y"
    {(yyval.dec) = A_VariableDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (1)].sym), NULL, false);;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 204 ".\\SysY.y"
    {(yyval.dec) = A_VariableDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (3)].sym), (yyvsp[(3) - (3)].exp), false);;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 205 ".\\SysY.y"
    {
        (yyval.dec) = A_ArrayDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (2)].sym), (A_expList)U_reverseList((yyvsp[(2) - (2)].expList)), NULL, false);
    ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 208 ".\\SysY.y"
    {
        (yyval.dec) = A_ArrayDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (4)].sym), (A_expList)U_reverseList((yyvsp[(2) - (4)].expList)), (yyvsp[(4) - (4)].arrayInitList), false);
    ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 214 ".\\SysY.y"
    {(yyval.arrayInitList) = (A_arrayInitList)U_reverseList((yyvsp[(2) - (3)].arrayInitList));;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 215 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yyloc)), A_IntExp(A_POS((yyloc)), 0)), NULL);
    ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 221 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[(3) - (3)])), (yyvsp[(3) - (3)].exp)), (yyvsp[(1) - (3)].arrayInitList));
    ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 224 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[(3) - (3)])), (yyvsp[(3) - (3)].arrayInitList)), (yyvsp[(1) - (3)].arrayInitList));
    ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 227 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[(1) - (1)])), (yyvsp[(1) - (1)].exp)), NULL);
    ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 230 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[(1) - (1)])), (yyvsp[(1) - (1)].arrayInitList)), NULL);
    ;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 236 ".\\SysY.y"
    {
        (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[(2) - (6)].sym), (A_fieldList)U_reverseList((yyvsp[(4) - (6)].fieldList)), S_Symbol("void"), (yyvsp[(6) - (6)].stm));
    ;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 239 ".\\SysY.y"
    {
        (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[(2) - (5)].sym), NULL, S_Symbol("void"), (yyvsp[(5) - (5)].stm));
    ;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 242 ".\\SysY.y"
    {
        (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[(2) - (6)].sym), (A_fieldList)U_reverseList((yyvsp[(4) - (6)].fieldList)), (yyvsp[(1) - (6)].sym), (yyvsp[(6) - (6)].stm));
    ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 245 ".\\SysY.y"
    {
        (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[(2) - (5)].sym), NULL, (yyvsp[(1) - (5)].sym), (yyvsp[(5) - (5)].stm));
    ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 251 ".\\SysY.y"
    {(yyval.fieldList) = A_FieldList((yyvsp[(3) - (3)].field), (yyvsp[(1) - (3)].fieldList));;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 252 ".\\SysY.y"
    {(yyval.fieldList) = A_FieldList((yyvsp[(1) - (1)].field), NULL);;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 256 ".\\SysY.y"
    {(yyval.field) = A_Field(A_POS((yyloc)), (yyvsp[(2) - (2)].sym), (yyvsp[(1) - (2)].sym), NULL);;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 257 ".\\SysY.y"
    {(yyval.field) = A_Field(A_POS((yyloc)), (yyvsp[(2) - (3)].sym), (yyvsp[(1) - (3)].sym), (A_expList)U_reverseList((yyvsp[(3) - (3)].expList)));;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 261 ".\\SysY.y"
    {(yyval.expList) = A_ExpList((yyvsp[(3) - (4)].exp), (yyvsp[(1) - (4)].expList));;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 262 ".\\SysY.y"
    {(yyval.expList) = A_ExpList(A_IntExp(A_POS((yyloc)), 0), NULL);;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 264 ".\\SysY.y"
    {(yyval.expList) = A_ExpList((yyvsp[(2) - (3)].exp), NULL);;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 268 ".\\SysY.y"
    {(yyval.stm) = A_BlockStm(A_POS((yyloc)), (A_comStmList)U_reverseList((yyvsp[(2) - (3)].comStmList)));;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 269 ".\\SysY.y"
    {(yyval.stm) = A_BlockStm(A_POS((yyloc)),NULL);;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 273 ".\\SysY.y"
    {(yyval.comStmList) = A_ComStmList(A_ComStm((yyvsp[(2) - (2)].decList), NULL), (yyvsp[(1) - (2)].comStmList));;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 274 ".\\SysY.y"
    {(yyval.comStmList) = A_ComStmList(A_ComStm(NULL, (yyvsp[(2) - (2)].stm)), (yyvsp[(1) - (2)].comStmList));;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 275 ".\\SysY.y"
    {(yyval.comStmList) = A_ComStmList(A_ComStm((yyvsp[(1) - (1)].decList), NULL), NULL);;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 276 ".\\SysY.y"
    {(yyval.comStmList) = A_ComStmList(A_ComStm(NULL, (yyvsp[(1) - (1)].stm)), NULL);;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 280 ".\\SysY.y"
    {(yyval.stm) = A_AssignStm(A_POS((yyloc)), (yyvsp[(1) - (4)].var), (yyvsp[(3) - (4)].exp));;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 281 ".\\SysY.y"
    {(yyval.stm) = A_ExpStm(A_POS((yyloc)), (yyvsp[(1) - (2)].exp));;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 282 ".\\SysY.y"
    {(yyval.stm) = A_ExpStm(A_POS((yyloc)),NULL);;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 283 ".\\SysY.y"
    {(yyval.stm) = (yyvsp[(1) - (1)].stm);;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 284 ".\\SysY.y"
    {(yyval.stm) = A_IfStm(A_POS((yyloc)), (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stm), NULL);;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 285 ".\\SysY.y"
    {(yyval.stm) = A_IfStm(A_POS((yyloc)), (yyvsp[(3) - (7)].exp), (yyvsp[(5) - (7)].stm), (yyvsp[(7) - (7)].stm));;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 286 ".\\SysY.y"
    {(yyval.stm) = A_WhileStm(A_POS((yyloc)), (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stm));;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 287 ".\\SysY.y"
    {(yyval.stm) = A_BreakStm(A_POS((yyloc)));;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 288 ".\\SysY.y"
    {(yyval.stm) = A_ContinueStm(A_POS((yyloc)));;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 289 ".\\SysY.y"
    {(yyval.stm) = A_ReturnStm(A_POS((yyloc)), (yyvsp[(2) - (3)].exp));;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 290 ".\\SysY.y"
    {(yyval.stm) = A_ReturnStm(A_POS((yyloc)), NULL);;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 294 ".\\SysY.y"
    {(yyval.expList) = A_ExpList((yyvsp[(3) - (3)].exp), (yyvsp[(1) - (3)].expList));;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 295 ".\\SysY.y"
    {(yyval.expList) = A_ExpList((yyvsp[(1) - (1)].exp), NULL);;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 299 ".\\SysY.y"
    {(yyval.var) = A_SimpleVar(A_POS((yylsp[(1) - (1)])), (yyvsp[(1) - (1)].sym));;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 300 ".\\SysY.y"
    {(yyval.var) = A_ArrayVar(A_POS((yylsp[(1) - (4)])), (yyvsp[(1) - (4)].var), (yyvsp[(3) - (4)].exp));;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 304 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 308 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 309 ".\\SysY.y"
    {(yyval.exp) = A_OpExp(A_POS((yyloc)), (yyvsp[(1) - (3)].exp), A_and, (yyvsp[(3) - (3)].exp));}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 313 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 314 ".\\SysY.y"
    {(yyval.exp) = A_OpExp(A_POS((yyloc)), (yyvsp[(1) - (3)].exp), A_or, (yyvsp[(3) - (3)].exp));}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 318 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 319 ".\\SysY.y"
    {(yyval.exp) = A_OpExp(A_POS((yyloc)), (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].binOp), (yyvsp[(3) - (3)].exp));}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 323 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 324 ".\\SysY.y"
    {(yyval.exp) = A_OpExp(A_POS((yyloc)), (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].binOp), (yyvsp[(3) - (3)].exp));}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 328 ".\\SysY.y"
    {(yyval.binOp) = A_lt;;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 329 ".\\SysY.y"
    {(yyval.binOp) = A_le;;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 330 ".\\SysY.y"
    {(yyval.binOp) = A_gt;;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 331 ".\\SysY.y"
    {(yyval.binOp) = A_ge;;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 335 ".\\SysY.y"
    {(yyval.binOp) = A_eq;;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 336 ".\\SysY.y"
    {(yyval.binOp) = A_ne;;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 340 ".\\SysY.y"
    {(yyval.binOp) = A_add;;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 341 ".\\SysY.y"
    {(yyval.binOp) = A_sub;;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 345 ".\\SysY.y"
    {(yyval.binOp) = A_mul;;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 346 ".\\SysY.y"
    {(yyval.binOp) = A_div;;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 347 ".\\SysY.y"
    {(yyval.binOp) = A_mod;;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 351 ".\\SysY.y"
    {(yyval.binOp) = A_add;;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 352 ".\\SysY.y"
    {(yyval.binOp) = A_sub;;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 353 ".\\SysY.y"
    {(yyval.binOp) = A_not;;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 357 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 361 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 362 ".\\SysY.y"
    {A_OpExp(A_POS((yyloc)), (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].binOp), (yyvsp[(3) - (3)].exp));}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 366 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 367 ".\\SysY.y"
    {A_OpExp(A_POS((yyloc)), (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].binOp), (yyvsp[(3) - (3)].exp));}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 371 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 372 ".\\SysY.y"
    {
        (yyval.exp) = A_CallExp(A_POS((yyloc)), (yyvsp[(1) - (4)].sym), (A_expList)U_reverseList((yyvsp[(3) - (4)].expList)));
    ;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 375 ".\\SysY.y"
    {
        (yyval.exp) = A_CallExp(A_POS((yyloc)), (yyvsp[(1) - (3)].sym), NULL);
    ;}
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 378 ".\\SysY.y"
    {
        (yyval.exp) = A_OpExp(A_POS((yyloc)), A_IntExp(A_POS((yyloc)),0), (yyvsp[(1) - (2)].binOp), (yyvsp[(2) - (2)].exp))
    ;}
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 384 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(2) - (3)].exp);}
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 385 ".\\SysY.y"
    {(yyval.exp) = A_VarExp(A_POS((yyloc)), (yyvsp[(1) - (1)].var));;}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 386 ".\\SysY.y"
    {(yyval.exp) = A_IntExp(A_POS((yyloc)), (yyvsp[(1) - (1)].integer));;}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 390 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);;}
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 394 ".\\SysY.y"
    {(yyval.integer) = (yyvsp[(1) - (1)].integer);;}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 398 ".\\SysY.y"
    {(yyval.sym) = S_Symbol((yyvsp[(1) - (1)].string));;}
    break;



/* Line 1464 of yacc.c  */
#line 2449 "SysY.tab.cpp"
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
      yyerror (YY_("syntax error"));
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
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
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
		      yytoken, &yylval, &yylloc);
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
		  yystos[yystate], yyvsp, yylsp);
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
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
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



/* Line 1684 of yacc.c  */
#line 401 ".\\SysY.y"


void yyerror(char* msg){
    fprintf(stderr, "%s:%d:%d: error: %s\n", FILE_NAME, yylineno, EM_token_pos, msg);
    char** code_msg = EM_error_code();
    fprintf(stderr, "%d| %s\n", yylineno, code_msg[0]);

    char line_no[32];
    sprintf(line_no,"%d",yylineno);
    unsigned len = strlen(line_no);
    for(int i = 0; i<len; i++){
        fprintf(stderr, " ");
    }
    fprintf(stderr, "| %s\n", code_msg[1]);
}

int yywrap(){
    return 1;
}


