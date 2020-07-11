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
     CHARACTER = 297,
     UAddOp = 298,
     UMulOp = 299,
     UUnary = 300
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
#line 218 "SysY.tab.cpp"
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
#line 243 "SysY.tab.cpp"

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
#define YYLAST   336

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  46
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  95
/* YYNRULES -- Number of states.  */
#define YYNSTATES  175

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   300

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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45
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
     212,   215,   218,   222,   225,   229,   231,   233,   238,   242,
     246,   249,   253,   257,   259,   261,   263,   265,   267,   269,
     271,   273,   275,   277,   279,   281,   283,   287,   291,   294,
     298,   300,   305,   307,   309,   311
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      47,     0,    -1,    48,    -1,    48,    49,    -1,    48,    62,
      -1,    49,    -1,    62,    -1,    50,    -1,    57,    -1,    18,
      52,    51,    31,    -1,    51,    30,    53,    -1,    53,    -1,
      19,    -1,    79,    17,    77,    -1,    79,    54,    17,    55,
      -1,    54,    35,    77,    36,    -1,    35,    77,    36,    -1,
      37,    56,    38,    -1,    37,    38,    -1,    56,    30,    55,
      -1,    56,    30,    77,    -1,    55,    -1,    77,    -1,    52,
      58,    31,    -1,    58,    30,    59,    -1,    59,    -1,    79,
      -1,    79,    17,    76,    -1,    79,    54,    -1,    79,    54,
      17,    60,    -1,    37,    61,    38,    -1,    37,    38,    -1,
      61,    30,    76,    -1,    61,    30,    60,    -1,    76,    -1,
      60,    -1,    21,    79,    33,    63,    34,    66,    -1,    21,
      79,    33,    34,    66,    -1,    52,    79,    33,    63,    34,
      66,    -1,    52,    79,    33,    34,    66,    -1,    63,    30,
      64,    -1,    64,    -1,    52,    79,    -1,    52,    79,    65,
      -1,    65,    35,    76,    36,    -1,    35,    36,    -1,    35,
      76,    36,    -1,    37,    67,    38,    -1,    37,    38,    -1,
      67,    49,    -1,    67,    68,    -1,    49,    -1,    68,    -1,
      70,    17,    76,    31,    -1,    76,    31,    -1,    31,    -1,
      66,    -1,    22,    33,    71,    34,    68,    -1,    22,    33,
      71,    34,    68,    23,    68,    -1,    24,    33,    71,    34,
      68,    -1,    28,    31,    -1,    29,    31,    -1,    27,    76,
      31,    -1,    27,    31,    -1,    69,    30,    76,    -1,    76,
      -1,    79,    -1,    70,    35,    76,    36,    -1,    76,    72,
      76,    -1,    33,    71,    34,    -1,    12,    71,    -1,    71,
      13,    71,    -1,    71,    14,    71,    -1,    15,    -1,    16,
      -1,     8,    -1,     9,    -1,    10,    -1,    11,    -1,     3,
      -1,     4,    -1,     5,    -1,     6,    -1,     7,    -1,     3,
      -1,     4,    -1,    76,    73,    76,    -1,    76,    74,    76,
      -1,    75,    76,    -1,    33,    76,    34,    -1,    70,    -1,
      79,    33,    69,    34,    -1,    78,    -1,    76,    -1,    40,
      -1,    39,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   129,   129,   133,   134,   135,   136,   140,   141,   145,
     151,   154,   158,   162,   165,   171,   174,   178,   179,   185,
     188,   191,   194,   200,   204,   205,   209,   210,   211,   214,
     220,   221,   227,   230,   233,   236,   242,   245,   248,   251,
     257,   258,   262,   263,   267,   268,   270,   274,   275,   279,
     280,   281,   282,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   295,   296,   300,   301,   305,   306,   310,   311,
     312,   313,   314,   318,   319,   320,   321,   322,   323,   327,
     328,   332,   333,   334,   338,   339,   343,   344,   345,   346,
     347,   348,   351,   355,   359,   363
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
  "INTEGER", "STRING", "CHARACTER", "UAddOp", "UMulOp", "UUnary",
  "$accept", "program", "CompUnit", "Decl", "ConstDecl", "ConstDefList",
  "BType", "ConstDef", "ConstSubscripts", "ConstInitVal",
  "ConstInitValList", "VarDecl", "VarDefList", "VarDef", "InitVal",
  "InitValList", "FuncDef", "FuncFParams", "FuncFParam", "ExpSubscripts",
  "Block", "BlockItem", "Stmt", "FuncRParams", "LVal", "Cond", "RelOp",
  "addOp", "mulOp", "UnaryOp", "Exp", "ConstExp", "Number", "Ident", 0
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
     295,   296,   297,   298,   299,   300
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    46,    47,    48,    48,    48,    48,    49,    49,    50,
      51,    51,    52,    53,    53,    54,    54,    55,    55,    56,
      56,    56,    56,    57,    58,    58,    59,    59,    59,    59,
      60,    60,    61,    61,    61,    61,    62,    62,    62,    62,
      63,    63,    64,    64,    65,    65,    65,    66,    66,    67,
      67,    67,    67,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    69,    69,    70,    70,    71,    71,
      71,    71,    71,    72,    72,    72,    72,    72,    72,    73,
      73,    74,    74,    74,    75,    75,    76,    76,    76,    76,
      76,    76,    76,    77,    78,    79
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
       2,     2,     3,     2,     3,     1,     1,     4,     3,     3,
       2,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     2,     3,
       1,     4,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    12,     0,     0,     2,     5,     7,     0,     8,
       6,     0,    95,     0,     1,     3,     4,     0,    25,    26,
       0,    11,     0,     0,     0,    23,     0,     0,     0,    28,
       0,     9,     0,     0,     0,     0,     0,    41,    24,    26,
      84,    85,     0,    94,    90,     0,    27,    92,    66,     0,
       0,    93,     0,     0,     0,    10,    13,     0,     0,    37,
      42,     0,     0,     0,     0,    88,    79,    80,    81,    82,
      83,     0,     0,     0,    39,     0,    16,     0,    29,     0,
       0,    14,     0,     0,     0,     0,     0,    55,    48,    51,
       0,    56,     0,    52,    90,     0,     0,    43,    40,    36,
      89,     0,    86,    87,     0,    65,    38,    31,    35,     0,
      34,    15,    18,    21,     0,    22,     0,     0,    63,     0,
      60,    61,    47,    49,    50,     0,    54,    45,     0,     0,
      67,     0,    91,     0,    30,     0,    17,     0,     0,     0,
       0,     0,    62,     0,    46,     0,    64,    33,    32,    19,
      20,    70,     0,     0,     0,     0,     0,    75,    76,    77,
      78,    73,    74,     0,     0,    53,    44,    69,    71,    72,
      57,    68,    59,     0,    58
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     6,     7,    20,    35,    21,    29,    81,
     114,     9,    17,    18,    78,   109,    10,    36,    37,    97,
      91,    92,    93,   104,    44,   139,   163,    71,    72,    45,
      51,    52,    47,    48
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -88
static const yytype_int16 yypact[] =
{
       6,    -1,   -88,    -9,    41,     6,   -88,   -88,    -9,   -88,
     -88,    -9,   -88,    20,   -88,   -88,   -88,   131,   -88,     0,
     191,   -88,     5,    18,    -9,   -88,   210,   130,   210,     9,
      -9,   -88,   210,    43,    42,    -9,    62,   -88,   -88,    50,
     -88,   -88,   210,   -88,    47,   210,   253,   -88,    54,    42,
      76,   253,    83,    89,   210,   -88,   -88,   103,   117,   -88,
     118,    -1,    42,    68,   210,   -88,   -88,   -88,   -88,   -88,
     -88,   210,   210,   210,   -88,    42,   -88,    85,   -88,   122,
      94,   -88,   132,   136,    17,   140,   147,   -88,   -88,   -88,
      -9,   -88,   148,   -88,    66,   275,   179,   145,   -88,   -88,
     -88,   225,   109,   -88,    79,   253,   -88,   -88,   -88,    24,
     253,   -88,   -88,   -88,    46,   -88,   156,   156,   -88,   282,
     -88,   -88,   -88,   -88,   -88,   210,   -88,   -88,   241,   210,
     -88,   210,   -88,   187,   -88,   201,   -88,   156,   156,    -3,
     320,   129,   -88,   291,   -88,   248,   253,   -88,   253,   -88,
     -88,   -88,   203,   259,   156,   156,   170,   -88,   -88,   -88,
     -88,   -88,   -88,   210,   170,   -88,   -88,   -88,   -88,   171,
     177,   253,   -88,   170,   -88
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -88,   -88,   -88,     1,   -88,   -88,     3,   163,   180,   -67,
     -88,   -88,   -88,   182,   -68,   -88,   206,   181,   162,   -88,
     -20,   -88,   -87,   -88,   -56,   -74,   -88,   -88,   -88,   -88,
     -26,   -31,   -88,     4
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      46,    56,    94,     8,    11,   124,    15,    13,     8,   108,
     154,   155,    19,   113,    59,    22,    63,    26,     2,    65,
      40,    41,    32,    79,     1,     2,    53,     3,    39,    74,
      12,   156,    95,    27,    22,    28,    94,     2,   101,    60,
      28,    14,    99,   141,    54,   102,   103,   105,   118,   115,
      42,   110,    34,    23,   133,   106,    12,    43,   119,    89,
      57,    90,   134,   151,   152,   147,    95,    26,   149,   170,
     128,    66,    67,    68,    69,    70,   135,   172,    54,    58,
     168,   169,    64,   125,   136,    28,   174,    73,    40,    41,
     140,   140,    61,   123,    39,    90,    62,    40,    41,   143,
      94,    64,   100,   145,   150,   146,    61,   148,    94,   131,
      75,   140,   153,   132,    68,    69,    70,    94,    42,    76,
      40,    41,    77,   107,    12,    43,    77,    42,   140,   140,
      95,    80,   112,    12,    43,     1,     2,   171,    95,    82,
      80,    83,   154,   155,    84,    85,    86,    95,    87,     2,
      42,    40,    41,    96,    58,    88,    12,    43,   111,    40,
      41,    24,    25,   164,    49,   116,     1,     2,   137,   117,
      82,   120,    83,    40,    41,    84,    85,    86,   121,    87,
     129,    42,    40,    41,   154,    58,   122,    12,    43,   138,
      40,    41,    82,    55,    83,    12,    43,    84,    85,    86,
     173,    87,    33,    42,    40,    41,    38,    58,    50,    12,
      43,    16,    42,    40,    41,   127,   154,   155,    12,    43,
      42,    30,    31,    98,    77,     0,    12,    43,    66,    67,
      68,    69,    70,     0,    42,     0,     0,   167,    80,     0,
      12,    43,     0,    42,    66,    67,    68,    69,    70,    12,
      43,    66,    67,    68,    69,    70,    66,    67,    68,    69,
      70,   130,    66,    67,    68,    69,    70,   157,   158,   159,
     160,     0,     0,     0,   161,   162,     0,   144,    66,    67,
      68,    69,    70,     0,   166,    66,    67,    68,    69,    70,
       0,     0,     0,   100,    66,    67,    68,    69,    70,     0,
       0,     0,     0,     0,     0,     0,   126,     0,     0,     0,
       0,     0,     0,   142,     0,     0,     0,     0,     0,     0,
       0,     0,   165,    66,    67,    68,    69,    70,   157,   158,
     159,   160,     0,     0,     0,   161,   162
};

static const yytype_int16 yycheck[] =
{
      26,    32,    58,     0,     1,    92,     5,     3,     5,    77,
      13,    14,     8,    80,    34,    11,    42,    17,    19,    45,
       3,     4,    17,    54,    18,    19,    17,    21,    24,    49,
      39,    34,    58,    33,    30,    35,    92,    19,    64,    35,
      35,     0,    62,   117,    35,    71,    72,    73,    31,    80,
      33,    77,    34,    33,    30,    75,    39,    40,    84,    58,
      17,    58,    38,   137,   138,   133,    92,    17,   135,   156,
      96,     3,     4,     5,     6,     7,    30,   164,    35,    37,
     154,   155,    35,    17,    38,    35,   173,    33,     3,     4,
     116,   117,    30,    92,    90,    92,    34,     3,     4,   125,
     156,    35,    34,   129,   135,   131,    30,   133,   164,    30,
      34,   137,   138,    34,     5,     6,     7,   173,    33,    36,
       3,     4,    37,    38,    39,    40,    37,    33,   154,   155,
     156,    37,    38,    39,    40,    18,    19,   163,   164,    22,
      37,    24,    13,    14,    27,    28,    29,   173,    31,    19,
      33,     3,     4,    35,    37,    38,    39,    40,    36,     3,
       4,    30,    31,    34,    34,    33,    18,    19,    12,    33,
      22,    31,    24,     3,     4,    27,    28,    29,    31,    31,
      35,    33,     3,     4,    13,    37,    38,    39,    40,    33,
       3,     4,    22,    30,    24,    39,    40,    27,    28,    29,
      23,    31,    22,    33,     3,     4,    24,    37,    27,    39,
      40,     5,    33,     3,     4,    36,    13,    14,    39,    40,
      33,    30,    31,    61,    37,    -1,    39,    40,     3,     4,
       5,     6,     7,    -1,    33,    -1,    -1,    34,    37,    -1,
      39,    40,    -1,    33,     3,     4,     5,     6,     7,    39,
      40,     3,     4,     5,     6,     7,     3,     4,     5,     6,
       7,    36,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    -1,    -1,    -1,    15,    16,    -1,    36,     3,     4,
       5,     6,     7,    -1,    36,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    34,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    -1,    -1,    -1,    15,    16
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    18,    19,    21,    47,    48,    49,    50,    52,    57,
      62,    52,    39,    79,     0,    49,    62,    58,    59,    79,
      51,    53,    79,    33,    30,    31,    17,    33,    35,    54,
      30,    31,    17,    54,    34,    52,    63,    64,    59,    79,
       3,     4,    33,    40,    70,    75,    76,    78,    79,    34,
      63,    76,    77,    17,    35,    53,    77,    17,    37,    66,
      79,    30,    34,    76,    35,    76,     3,     4,     5,     6,
       7,    73,    74,    33,    66,    34,    36,    37,    60,    77,
      37,    55,    22,    24,    27,    28,    29,    31,    38,    49,
      52,    66,    67,    68,    70,    76,    35,    65,    64,    66,
      34,    76,    76,    76,    69,    76,    66,    38,    60,    61,
      76,    36,    38,    55,    56,    77,    33,    33,    31,    76,
      31,    31,    38,    49,    68,    17,    31,    36,    76,    35,
      36,    30,    34,    30,    38,    30,    38,    12,    33,    71,
      76,    71,    31,    76,    36,    76,    76,    60,    76,    55,
      77,    71,    71,    76,    13,    14,    34,     8,     9,    10,
      11,    15,    16,    72,    34,    31,    36,    34,    71,    71,
      68,    76,    68,    23,    68
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
#line 129 ".\\SysY.y"
    {absyn_root = (A_decList)U_reverseList((yyvsp[(1) - (1)].decList));;}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 133 ".\\SysY.y"
    {(yyval.decList) = A_castDecList((yyvsp[(2) - (2)].decList), (yyvsp[(1) - (2)].decList));;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 134 ".\\SysY.y"
    {(yyval.decList) = A_DecList((yyvsp[(2) - (2)].dec), (yyvsp[(1) - (2)].decList));;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 135 ".\\SysY.y"
    {(yyval.decList) = (yyvsp[(1) - (1)].decList);;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 136 ".\\SysY.y"
    {(yyval.decList) = A_DecList((yyvsp[(1) - (1)].dec), NULL);;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 140 ".\\SysY.y"
    {(yyval.decList) = (yyvsp[(1) - (1)].decList);;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 141 ".\\SysY.y"
    {(yyval.decList) = (yyvsp[(1) - (1)].decList);;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 145 ".\\SysY.y"
    {
        (yyval.decList) = A_conform((yyvsp[(3) - (4)].decList), (yyvsp[(2) - (4)].sym));
    ;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 151 ".\\SysY.y"
    {
        (yyval.decList) = A_DecList((yyvsp[(3) - (3)].dec), (yyvsp[(1) - (3)].decList));
    ;}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 154 ".\\SysY.y"
    {(yyval.decList) = A_DecList((yyvsp[(1) - (1)].dec), NULL);;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 158 ".\\SysY.y"
    {(yyval.sym) = S_Symbol("int");;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 162 ".\\SysY.y"
    {
        (yyval.dec) = A_VariableDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (3)].sym), (yyvsp[(3) - (3)].exp), true);
    ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 165 ".\\SysY.y"
    {
        (yyval.dec) = A_ArrayDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (4)].sym), (A_expList)U_reverseList((yyvsp[(2) - (4)].expList)), (yyvsp[(4) - (4)].arrayInitList), true);
    ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 171 ".\\SysY.y"
    {
        (yyval.expList) = A_ExpList((yyvsp[(3) - (4)].exp), (yyvsp[(1) - (4)].expList));
    ;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 174 ".\\SysY.y"
    {(yyval.expList) = A_ExpList((yyvsp[(2) - (3)].exp), NULL);;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 178 ".\\SysY.y"
    {(yyval.arrayInitList) = (A_arrayInitList)U_reverseList((yyvsp[(2) - (3)].arrayInitList));;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 179 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yyloc)), A_IntExp(A_POS((yyloc)), 0)), NULL);
    ;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 185 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[(3) - (3)])), (yyvsp[(3) - (3)].arrayInitList)), (yyvsp[(1) - (3)].arrayInitList));
    ;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 188 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[(1) - (3)])), (yyvsp[(3) - (3)].exp)), (yyvsp[(1) - (3)].arrayInitList));
    ;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 191 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[(1) - (1)])), (yyvsp[(1) - (1)].arrayInitList)), NULL);
    ;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 194 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[(1) - (1)])), (yyvsp[(1) - (1)].exp)), NULL);
    ;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 200 ".\\SysY.y"
    {(yyval.decList) = A_conform((yyvsp[(2) - (3)].decList), (yyvsp[(1) - (3)].sym));;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 204 ".\\SysY.y"
    {(yyval.decList) = A_DecList((yyvsp[(3) - (3)].dec), (yyvsp[(1) - (3)].decList));;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 205 ".\\SysY.y"
    {(yyval.decList) = A_DecList((yyvsp[(1) - (1)].dec), NULL);;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 209 ".\\SysY.y"
    {(yyval.dec) = A_VariableDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (1)].sym), NULL, false);;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 210 ".\\SysY.y"
    {(yyval.dec) = A_VariableDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (3)].sym), (yyvsp[(3) - (3)].exp), false);;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 211 ".\\SysY.y"
    {
        (yyval.dec) = A_ArrayDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (2)].sym), (A_expList)U_reverseList((yyvsp[(2) - (2)].expList)), NULL, false);
    ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 214 ".\\SysY.y"
    {
        (yyval.dec) = A_ArrayDec(A_POS((yyloc)), NULL, (yyvsp[(1) - (4)].sym), (A_expList)U_reverseList((yyvsp[(2) - (4)].expList)), (yyvsp[(4) - (4)].arrayInitList), false);
    ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 220 ".\\SysY.y"
    {(yyval.arrayInitList) = (A_arrayInitList)U_reverseList((yyvsp[(2) - (3)].arrayInitList));;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 221 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yyloc)), A_IntExp(A_POS((yyloc)), 0)), NULL);
    ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 227 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[(3) - (3)])), (yyvsp[(3) - (3)].exp)), (yyvsp[(1) - (3)].arrayInitList));
    ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 230 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[(3) - (3)])), (yyvsp[(3) - (3)].arrayInitList)), (yyvsp[(1) - (3)].arrayInitList));
    ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 233 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[(1) - (1)])), (yyvsp[(1) - (1)].exp)), NULL);
    ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 236 ".\\SysY.y"
    {
        (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[(1) - (1)])), (yyvsp[(1) - (1)].arrayInitList)), NULL);
    ;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 242 ".\\SysY.y"
    {
        (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[(2) - (6)].sym), (A_fieldList)U_reverseList((yyvsp[(4) - (6)].fieldList)), S_Symbol("void"), (yyvsp[(6) - (6)].stm));
    ;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 245 ".\\SysY.y"
    {
        (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[(2) - (5)].sym), NULL, S_Symbol("void"), (yyvsp[(5) - (5)].stm));
    ;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 248 ".\\SysY.y"
    {
        (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[(2) - (6)].sym), (A_fieldList)U_reverseList((yyvsp[(4) - (6)].fieldList)), (yyvsp[(1) - (6)].sym), (yyvsp[(6) - (6)].stm));
    ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 251 ".\\SysY.y"
    {
        (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[(2) - (5)].sym), NULL, (yyvsp[(1) - (5)].sym), (yyvsp[(5) - (5)].stm));
    ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 257 ".\\SysY.y"
    {(yyval.fieldList) = A_FieldList((yyvsp[(3) - (3)].field), (yyvsp[(1) - (3)].fieldList));;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 258 ".\\SysY.y"
    {(yyval.fieldList) = A_FieldList((yyvsp[(1) - (1)].field), NULL);;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 262 ".\\SysY.y"
    {(yyval.field) = A_Field(A_POS((yyloc)), (yyvsp[(2) - (2)].sym), (yyvsp[(1) - (2)].sym), NULL);;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 263 ".\\SysY.y"
    {(yyval.field) = A_Field(A_POS((yyloc)), (yyvsp[(2) - (3)].sym), (yyvsp[(1) - (3)].sym), (A_expList)U_reverseList((yyvsp[(3) - (3)].expList)));;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 267 ".\\SysY.y"
    {(yyval.expList) = A_ExpList((yyvsp[(3) - (4)].exp), (yyvsp[(1) - (4)].expList));;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 268 ".\\SysY.y"
    {(yyval.expList) = A_ExpList(A_IntExp(A_POS((yyloc)), 0), NULL);;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 270 ".\\SysY.y"
    {(yyval.expList) = A_ExpList((yyvsp[(2) - (3)].exp), NULL);;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 274 ".\\SysY.y"
    {(yyval.stm) = A_BlockStm(A_POS((yyloc)), (A_comStmList)U_reverseList((yyvsp[(2) - (3)].comStmList)));;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 275 ".\\SysY.y"
    {(yyval.stm) = A_BlockStm(A_POS((yyloc)),NULL);;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 279 ".\\SysY.y"
    {(yyval.comStmList) = A_ComStmList(A_ComStm((yyvsp[(2) - (2)].decList), NULL), (yyvsp[(1) - (2)].comStmList));;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 280 ".\\SysY.y"
    {(yyval.comStmList) = A_ComStmList(A_ComStm(NULL, (yyvsp[(2) - (2)].stm)), (yyvsp[(1) - (2)].comStmList));;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 281 ".\\SysY.y"
    {(yyval.comStmList) = A_ComStmList(A_ComStm((yyvsp[(1) - (1)].decList), NULL), NULL);;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 282 ".\\SysY.y"
    {(yyval.comStmList) = A_ComStmList(A_ComStm(NULL, (yyvsp[(1) - (1)].stm)), NULL);;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 286 ".\\SysY.y"
    {(yyval.stm) = A_AssignStm(A_POS((yyloc)), (yyvsp[(1) - (4)].var), (yyvsp[(3) - (4)].exp));;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 287 ".\\SysY.y"
    {(yyval.stm) = A_ExpStm(A_POS((yyloc)), (yyvsp[(1) - (2)].exp));;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 288 ".\\SysY.y"
    {(yyval.stm) = A_ExpStm(A_POS((yyloc)),NULL);;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 289 ".\\SysY.y"
    {(yyval.stm) = (yyvsp[(1) - (1)].stm);;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 290 ".\\SysY.y"
    {(yyval.stm) = A_IfStm(A_POS((yyloc)), (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stm), NULL);;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 291 ".\\SysY.y"
    {(yyval.stm) = A_IfStm(A_POS((yyloc)), (yyvsp[(3) - (7)].exp), (yyvsp[(5) - (7)].stm), (yyvsp[(7) - (7)].stm));;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 292 ".\\SysY.y"
    {(yyval.stm) = A_WhileStm(A_POS((yyloc)), (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stm));;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 293 ".\\SysY.y"
    {(yyval.stm) = A_BreakStm(A_POS((yyloc)));;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 294 ".\\SysY.y"
    {(yyval.stm) = A_ContinueStm(A_POS((yyloc)));;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 295 ".\\SysY.y"
    {(yyval.stm) = A_ReturnStm(A_POS((yyloc)), (yyvsp[(2) - (3)].exp));;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 296 ".\\SysY.y"
    {(yyval.stm) = A_ReturnStm(A_POS((yyloc)), NULL);;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 300 ".\\SysY.y"
    {(yyval.expList) = A_ExpList((yyvsp[(3) - (3)].exp), (yyvsp[(1) - (3)].expList));;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 301 ".\\SysY.y"
    {(yyval.expList) = A_ExpList((yyvsp[(1) - (1)].exp), NULL);;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 305 ".\\SysY.y"
    {(yyval.var) = A_SimpleVar(A_POS((yylsp[(1) - (1)])), (yyvsp[(1) - (1)].sym));;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 306 ".\\SysY.y"
    {(yyval.var) = A_ArrayVar(A_POS((yylsp[(1) - (4)])), (yyvsp[(1) - (4)].var), (yyvsp[(3) - (4)].exp));;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 310 ".\\SysY.y"
    {(yyval.exp) = A_LogicExp(A_POS((yyloc)), (yyvsp[(2) - (3)].binOp), (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp));;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 311 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(2) - (3)].exp);;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 312 ".\\SysY.y"
    {(yyval.exp) = A_LogicExp(A_POS((yyloc)), A_not, (yyvsp[(2) - (2)].exp), NULL);;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 313 ".\\SysY.y"
    {(yyval.exp) = A_LogicExp(A_POS((yyloc)), A_and, (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp));;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 314 ".\\SysY.y"
    {(yyval.exp) = A_LogicExp(A_POS((yyloc)), A_or, (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp));;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 318 ".\\SysY.y"
    {(yyval.binOp) = A_eq;;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 319 ".\\SysY.y"
    {(yyval.binOp) = A_ne;;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 320 ".\\SysY.y"
    {(yyval.binOp) = A_lt;;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 321 ".\\SysY.y"
    {(yyval.binOp) = A_le;;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 322 ".\\SysY.y"
    {(yyval.binOp) = A_gt;;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 323 ".\\SysY.y"
    {(yyval.binOp) = A_ge;;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 327 ".\\SysY.y"
    {(yyval.binOp) = A_add;;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 328 ".\\SysY.y"
    {(yyval.binOp) = A_sub;;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 332 ".\\SysY.y"
    {(yyval.binOp) = A_mul;;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 333 ".\\SysY.y"
    {(yyval.binOp) = A_div;;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 334 ".\\SysY.y"
    {(yyval.binOp) = A_mod;;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 338 ".\\SysY.y"
    {(yyval.binOp) = A_add;;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 339 ".\\SysY.y"
    {(yyval.binOp) = A_sub;;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 343 ".\\SysY.y"
    {(yyval.exp) = A_OpExp(A_POS((yyloc)), (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].binOp), (yyvsp[(3) - (3)].exp));;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 344 ".\\SysY.y"
    {(yyval.exp) = A_OpExp(A_POS((yyloc)),(yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].binOp), (yyvsp[(3) - (3)].exp));;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 345 ".\\SysY.y"
    {(yyval.exp) = A_OpExp(A_POS((yyloc)), A_IntExp(A_POS((yyloc)), 0), (yyvsp[(1) - (2)].binOp), (yyvsp[(2) - (2)].exp));;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 346 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(2) - (3)].exp);;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 347 ".\\SysY.y"
    {(yyval.exp) = A_VarExp(A_POS((yyloc)), (yyvsp[(1) - (1)].var));;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 348 ".\\SysY.y"
    {
        (yyval.exp) = A_CallExp(A_POS((yyloc)), (yyvsp[(1) - (4)].sym), (A_expList)U_reverseList((yyvsp[(3) - (4)].expList)));
    ;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 351 ".\\SysY.y"
    {(yyval.exp) = A_IntExp(A_POS((yyloc)), (yyvsp[(1) - (1)].integer));;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 355 ".\\SysY.y"
    {(yyval.exp) = (yyvsp[(1) - (1)].exp);;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 359 ".\\SysY.y"
    {(yyval.integer) = (yyvsp[(1) - (1)].integer);;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 363 ".\\SysY.y"
    {(yyval.sym) = S_Symbol((yyvsp[(1) - (1)].string));;}
    break;



/* Line 1464 of yacc.c  */
#line 2377 "SysY.tab.cpp"
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
#line 366 ".\\SysY.y"


static int calculate(int left, A_binOp op, int right){
    /** 计算全为int的constInt，当前文法已不适用 */
    switch (op){
        case A_add:
            return left + right;
        case A_sub:
            return left - right;
        case A_mul:
            return left * right;
        case A_div:
            return left / right;
        case A_mod:
            return left % right;
    }
    assert(0);
}

void yyerror(char* msg){
    fprintf(stderr, "%s:%d:%d: error: %s\n", FILE_NAME, yylineno, EM_token_pos, msg);
    char** code_msg = EM_error_code();
    fprintf(stderr, "%d| %s\n", yylineno, code_msg[0]);

    char line_no[32];
    //itoa(yylineno, line_no, 10);
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


