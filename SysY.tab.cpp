/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.3.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "SysY.y" /* yacc.c:337  */

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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



#line 114 "SysY.tab.c" /* yacc.c:337  */
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "SysY.tab.h".  */
#ifndef YY_YY_SYSY_TAB_H_INCLUDED
# define YY_YY_SYSY_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
enum yytokentype
{
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 44 "SysY.y" /* yacc.c:352  */

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

#line 221 "SysY.tab.c" /* yacc.c:352  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
    int first_line;
    int first_column;
    int last_line;
    int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_SYSY_TAB_H_INCLUDED  */



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
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
/* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   379

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  46
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  97
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  182

#define YYUNDEFTOK  2
#define YYMAXUTOK   300

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
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
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   129,   129,   133,   134,   135,   136,   140,   141,   145,
     151,   154,   158,   162,   165,   171,   174,   178,   179,   185,
     188,   191,   194,   200,   204,   205,   209,   210,   211,   214,
     220,   221,   227,   228,   231,   232,   238,   241,   244,   247,
     253,   254,   258,   259,   263,   264,   265,   269,   273,   274,
     275,   276,   280,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   293,   294,   298,   299,   303,   304,   305,   306,
     307,   311,   312,   313,   314,   315,   316,   320,   321,   325,
     326,   327,   331,   332,   336,   337,   338,   339,   340,   341,
     344,   348,   349,   350,   351,   352,   356,   360
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
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
                "addOp", "mulOp", "UnaryOp", "Exp", "ConstExp", "Number", "Ident", YY_NULLPTR
        };
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300
};
# endif

#define YYPACT_NINF -93

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-93)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
        {
                259,   -16,   -93,   -12,    39,   259,   -93,   -93,   -12,   -93,
                -93,   -12,   -93,   -18,   -93,   -93,   -93,   128,   -93,     1,
                161,   -93,    -7,   -11,   -12,   -93,   236,    58,    49,    66,
                -12,   -93,    49,   126,   -25,   -12,   159,   -93,   -93,   146,
                -93,   -93,   236,   -93,    11,   236,   372,   -93,    23,   -25,
                226,    49,    49,   248,   -93,    41,    49,   -93,   372,    72,
                176,   -93,    68,   -16,   -25,   333,   236,   -93,   -93,   -93,
                -93,   -93,   -93,   236,   236,   236,   -93,   -25,   338,   -93,
                -93,    49,    49,    47,   -93,   267,   181,   -93,    25,   105,
                203,   119,   152,   -93,   -12,   -93,   138,   -93,   147,    38,
                208,   116,   -93,   -93,   -93,   284,   308,   -93,   227,   372,
                -93,   -93,   308,   -93,   -93,   -93,   -24,   372,   -93,   -93,
                -93,    86,   372,   184,   184,   -93,   142,   -93,   -93,   -93,
                -93,   -93,   236,   -93,   -93,   289,   236,   -93,   236,   -93,
                225,   -93,   246,   -93,   184,   184,    77,   348,   120,   -93,
                343,   -93,   294,   372,   -93,   372,   -93,   372,   -93,   232,
                301,   184,   184,   198,   -93,   -93,   -93,   -93,   -93,   -93,
                236,   198,   -93,   -93,   -93,   -93,   173,   174,   372,   -93,
                198,   -93
        };

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
        {
                0,     0,    12,     0,     0,     2,     5,     7,     0,     8,
                6,     0,    97,     0,     1,     3,     4,     0,    25,    26,
                0,    11,     0,     0,     0,    23,     0,     0,     0,    28,
                0,     9,     0,     0,     0,     0,     0,    41,    24,    26,
                82,    83,     0,    96,    88,     0,    27,    90,    64,     0,
                0,     0,     0,     0,    95,     0,     0,    10,    13,     0,
                0,    37,    42,     0,     0,     0,     0,    86,    77,    78,
                79,    80,    81,     0,     0,     0,    39,     0,     0,    93,
                16,     0,     0,     0,    29,     0,     0,    14,     0,     0,
                0,     0,     0,    50,     0,    54,     0,    51,    88,     0,
                0,    43,    40,    36,    87,     0,    84,    85,     0,    63,
                38,    94,    91,    92,    31,    35,     0,    34,    15,    18,
                21,     0,    22,     0,     0,    61,     0,    58,    59,    47,
                48,    49,     0,    53,    45,     0,     0,    65,     0,    89,
                0,    30,     0,    17,     0,     0,     0,     0,     0,    60,
                0,    46,     0,    62,    33,    32,    19,    20,    68,     0,
                0,     0,     0,     0,    73,    74,    75,    76,    71,    72,
                0,     0,    52,    44,    67,    69,    70,    55,    66,    57,
                0,    56
        };

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
        {
                -93,   -93,   -93,    12,   -93,   -93,    21,   169,   186,   -66,
                -93,   -93,   -93,   206,   -74,   -93,   205,   240,   170,   -93,
                91,   -93,   -92,   -93,   -58,     8,   -93,    15,    17,   -27,
                -26,   -19,   -21,    96
        };

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
        {
                -1,     4,     5,     6,     7,    20,    35,    21,    29,    87,
                121,     9,    17,    18,    84,   116,    10,    36,    37,   101,
                95,    96,    97,   108,    44,   146,   170,    73,    74,    45,
                99,    53,    47,    48
        };

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
        {
                46,    52,    98,     2,   131,    52,   140,    54,     2,   115,
                32,    54,    60,    58,   141,    23,    65,    15,    26,    67,
                120,     8,    11,    34,    52,    52,     8,    12,    28,    52,
                54,    54,    78,    79,    27,    54,    28,    85,    98,    14,
                105,    68,    69,    70,    71,    72,    66,   106,   107,   109,
                40,    41,    40,    41,    52,    52,    75,   117,   123,    52,
                54,    54,   112,   113,   126,    54,   154,   122,    81,   133,
                82,   177,    93,    81,   135,    82,   156,     2,    83,   179,
                42,    94,    51,    55,    83,   114,    12,    43,   181,    43,
                161,   162,    49,    81,    81,    82,    82,   147,   147,    13,
                81,    56,    82,   100,    19,    98,   150,    22,   130,    86,
                152,   163,   153,    98,   155,    52,   142,    94,   147,   160,
                39,    54,    98,   157,   143,    61,    22,    81,    81,    82,
                82,    62,   148,   161,   162,   147,   147,    81,   124,    82,
                76,    40,    41,    59,   178,    68,    69,    70,    71,    72,
                127,   136,   158,   159,   171,   103,     1,     2,    24,    25,
                88,    56,    89,    26,   132,    90,    91,    92,   110,   175,
                176,    42,    81,   149,    82,    60,   129,    12,    43,    40,
                41,    28,    66,   128,    40,    41,   161,    40,    41,    63,
                39,    30,    31,    64,     1,     2,   144,   180,    88,    57,
                89,    40,    41,    90,    91,    92,    40,    41,    33,    42,
                16,    40,    41,    60,    51,    12,    43,   145,    86,   119,
                88,    43,    89,    12,    43,    90,    91,    92,    40,    41,
                38,    42,     0,   102,   125,    60,    42,    12,    43,    40,
                41,    42,    12,    43,   134,   161,   162,    12,    43,    40,
                41,    68,    69,    70,    71,    72,    63,   138,    42,     0,
                77,   139,    83,     0,    12,    43,   174,    50,     0,    42,
                68,    69,    70,    71,    72,    12,    43,     1,     2,    51,
                3,     0,     0,    86,    80,     0,    43,    68,    69,    70,
                71,    72,    68,    69,    70,    71,    72,    68,    69,    70,
                71,    72,     0,   118,    68,    69,    70,    71,    72,   164,
                165,   166,   167,    70,    71,    72,   168,   169,     0,     0,
                137,     0,     0,     0,     0,   151,     0,     0,     0,     0,
                173,     0,     0,     0,     0,   104,    68,    69,    70,    71,
                72,    68,    69,    70,    71,    72,    68,    69,    70,    71,
                72,    68,    69,    70,    71,    72,   164,   165,   166,   167,
                0,     0,     0,   168,   169,     0,     0,   104,     0,     0,
                0,   111,     0,     0,   172,    68,    69,    70,    71,    72
        };

static const yytype_int16 yycheck[] =
        {
                26,    28,    60,    19,    96,    32,    30,    28,    19,    83,
                17,    32,    37,    32,    38,    33,    42,     5,    17,    45,
                86,     0,     1,    34,    51,    52,     5,    39,    35,    56,
                51,    52,    51,    52,    33,    56,    35,    56,    96,     0,
                66,     3,     4,     5,     6,     7,    35,    73,    74,    75,
                3,     4,     3,     4,    81,    82,    33,    83,    33,    86,
                81,    82,    81,    82,    90,    86,   140,    86,    53,    31,
                53,   163,    60,    58,   100,    58,   142,    19,    37,   171,
                33,    60,    33,    17,    37,    38,    39,    40,   180,    40,
                13,    14,    34,    78,    79,    78,    79,   123,   124,     3,
                85,    35,    85,    35,     8,   163,   132,    11,    96,    37,
                136,    34,   138,   171,   140,   142,    30,    96,   144,   145,
                24,   142,   180,   142,    38,    34,    30,   112,   113,   112,
                113,    35,   124,    13,    14,   161,   162,   122,    33,   122,
                49,     3,     4,    17,   170,     3,     4,     5,     6,     7,
                31,    35,   144,   145,    34,    64,    18,    19,    30,    31,
                22,    35,    24,    17,    17,    27,    28,    29,    77,   161,
                162,    33,   157,    31,   157,    37,    38,    39,    40,     3,
                4,    35,    35,    31,     3,     4,    13,     3,     4,    30,
                94,    30,    31,    34,    18,    19,    12,    23,    22,    30,
                24,     3,     4,    27,    28,    29,     3,     4,    22,    33,
                5,     3,     4,    37,    33,    39,    40,    33,    37,    38,
                22,    40,    24,    39,    40,    27,    28,    29,     3,     4,
                24,    33,    -1,    63,    31,    37,    33,    39,    40,     3,
                4,    33,    39,    40,    36,    13,    14,    39,    40,     3,
                4,     3,     4,     5,     6,     7,    30,    30,    33,    -1,
                34,    34,    37,    -1,    39,    40,    34,    27,    -1,    33,
                3,     4,     5,     6,     7,    39,    40,    18,    19,    33,
                21,    -1,    -1,    37,    36,    -1,    40,     3,     4,     5,
                6,     7,     3,     4,     5,     6,     7,     3,     4,     5,
                6,     7,    -1,    36,     3,     4,     5,     6,     7,     8,
                9,    10,    11,     5,     6,     7,    15,    16,    -1,    -1,
                36,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
                36,    -1,    -1,    -1,    -1,    34,     3,     4,     5,     6,
                7,     3,     4,     5,     6,     7,     3,     4,     5,     6,
                7,     3,     4,     5,     6,     7,     8,     9,    10,    11,
                -1,    -1,    -1,    15,    16,    -1,    -1,    34,    -1,    -1,
                -1,    33,    -1,    -1,    31,     3,     4,     5,     6,     7
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
                63,    33,    75,    77,    78,    17,    35,    53,    77,    17,
                37,    66,    79,    30,    34,    76,    35,    76,     3,     4,
                5,     6,     7,    73,    74,    33,    66,    34,    77,    77,
                36,    73,    74,    37,    60,    77,    37,    55,    22,    24,
                27,    28,    29,    49,    52,    66,    67,    68,    70,    76,
                35,    65,    64,    66,    34,    76,    76,    76,    69,    76,
                66,    33,    77,    77,    38,    60,    61,    76,    36,    38,
                55,    56,    77,    33,    33,    31,    76,    31,    31,    38,
                49,    68,    17,    31,    36,    76,    35,    36,    30,    34,
                30,    38,    30,    38,    12,    33,    71,    76,    71,    31,
                76,    36,    76,    76,    60,    76,    55,    77,    71,    71,
                76,    13,    14,    34,     8,     9,    10,    11,    15,    16,
                72,    34,    31,    36,    34,    71,    71,    68,    76,    68,
                23,    68
        };

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
        {
                0,    46,    47,    48,    48,    48,    48,    49,    49,    50,
                51,    51,    52,    53,    53,    54,    54,    55,    55,    56,
                56,    56,    56,    57,    58,    58,    59,    59,    59,    59,
                60,    60,    61,    61,    61,    61,    62,    62,    62,    62,
                63,    63,    64,    64,    65,    65,    65,    66,    67,    67,
                67,    67,    68,    68,    68,    68,    68,    68,    68,    68,
                68,    68,    69,    69,    70,    70,    71,    71,    71,    71,
                71,    72,    72,    72,    72,    72,    72,    73,    73,    74,
                74,    74,    75,    75,    76,    76,    76,    76,    76,    76,
                76,    77,    77,    77,    77,    77,    78,    79
        };

/* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
        {
                0,     2,     1,     2,     2,     1,     1,     1,     1,     4,
                3,     1,     1,     3,     4,     4,     3,     3,     2,     3,
                3,     1,     1,     3,     3,     1,     1,     3,     2,     4,
                3,     2,     3,     3,     1,     1,     6,     5,     6,     5,
                3,     1,     2,     3,     4,     2,     3,     3,     2,     2,
                1,     1,     4,     2,     1,     5,     7,     5,     2,     2,
                3,     2,     3,     1,     1,     4,     3,     3,     2,     3,
                3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
                1,     1,     1,     1,     3,     3,     2,     3,     1,     4,
                1,     3,     3,     2,     3,     1,     1,     1
        };


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
                    else
                        goto append;

                append:
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

    return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
    YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
    YYSIZE_T yysize = yysize0;
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    /* Internationalized format string. */
    const char *yyformat = YY_NULLPTR;
    /* Arguments of yyformat. */
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
    /* Number of reported tokens (one for the "unexpected", one per
       "expected"). */
    int yycount = 0;

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yychar) is if
         this state is a consistent state with a default action.  Thus,
         detecting the absence of a lookahead is sufficient to determine
         that there is no unexpected or expected token to report.  In that
         case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is a
         consistent state with a default action.  There might have been a
         previous inconsistent state, consistent state with a non-default
         action, or user semantic action that manipulated yychar.
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */
    if (yytoken != YYEMPTY)
    {
        int yyn = yypact[*yyssp];
        yyarg[yycount++] = yytname[yytoken];
        if (!yypact_value_is_default (yyn))
        {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            /* Stay within bounds of both yycheck and yytname.  */
            int yychecklim = YYLAST - yyn + 1;
            int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
            int yyx;

            for (yyx = yyxbegin; yyx < yyxend; ++yyx)
                if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                    && !yytable_value_is_error (yytable[yyx + yyn]))
                {
                    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                        yycount = 1;
                        yysize = yysize0;
                        break;
                    }
                    yyarg[yycount++] = yytname[yyx];
                    {
                        YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                            yysize = yysize1;
                        else
                            return 2;
                    }
                }
        }
    }

    switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
        default: /* Avoid compiler warnings. */
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

    {
        YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
            yysize = yysize1;
        else
            return 2;
    }

    if (*yymsg_alloc < yysize)
    {
        *yymsg_alloc = 2 * yysize;
        if (! (yysize <= *yymsg_alloc
               && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
            *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
        return 1;
    }

    /* Avoid sprintf, as that infringes on the user's name space.
       Don't have undefined behavior even if the translation
       produced a string with the wrong number of "%s"s.  */
    {
        char *yyp = *yymsg;
        int yyi = 0;
        while ((*yyp = *yyformat) != '\0')
            if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
            {
                yyp += yytnamerr (yyp, yyarg[yyi++]);
                yyformat += 2;
            }
            else
            {
                yyp++;
                yyformat++;
            }
    }
    return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
    YYUSE (yyvaluep);
    YYUSE (yylocationp);
    if (!yymsg)
        yymsg = "Deleting";
    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    YYUSE (yytype);
    YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
        = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

    int yyn;
    int yyresult;
    /* Lookahead token as an internal (translated) token number.  */
    int yytoken = 0;
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

    yyssp = yyss = yyssa;
    yyvsp = yyvs = yyvsa;
    yylsp = yyls = yylsa;
    yystacksize = YYINITDEPTH;

    YYDPRINTF ((stderr, "Starting parse\n"));

    yystate = 0;
    yyerrstatus = 0;
    yynerrs = 0;
    yychar = YYEMPTY; /* Cause a token to be read.  */
    yylsp[0] = yylloc;
    goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
    yynewstate:
    /* In all cases, when you get here, the value and location stacks
       have just been pushed.  So pushing a state here evens the stacks.  */
    yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
    yysetstate:
    *yyssp = (yytype_int16) yystate;

    if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
        goto yyexhaustedlab;
#else
    {
        /* Get the current used size of the three stacks, in elements.  */
        YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
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
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
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
# undef YYSTACK_RELOCATE
            if (yyss1 != yyssa)
                YYSTACK_FREE (yyss1);
        }
# endif

        yyssp = yyss + yysize - 1;
        yyvsp = yyvs + yysize - 1;
        yylsp = yyls + yysize - 1;

        YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                (unsigned long) yystacksize));

        if (yyss + yystacksize - 1 <= yyssp)
            YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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
    if (yypact_value_is_default (yyn))
        goto yydefault;

    /* Not known => get a lookahead token if don't already have one.  */

    /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
    if (yychar == YYEMPTY)
    {
        YYDPRINTF ((stderr, "Reading a token: "));
        yychar = yylex ();
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
        if (yytable_value_is_error (yyn))
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
    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    *++yyvsp = yylval;
    YY_IGNORE_MAYBE_UNINITIALIZED_END
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
    yyreduce:
    /* yyn is the number of a rule to reduce with.  */
    yylen = yyr2[yyn];

    /* If YYLEN is nonzero, implement the default value of the action:
       '$$ = $1'.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  Assigning to YYVAL
       unconditionally makes the parser a bit smaller, and it avoids a
       GCC warning that YYVAL may be used uninitialized.  */
    yyval = yyvsp[1-yylen];

    /* Default location. */
    YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
    yyerror_range[1] = yyloc;
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
    {
        case 2:
#line 129 "SysY.y" /* yacc.c:1652  */
        {absyn_root = (A_decList)U_reverseList((yyvsp[0].decList));}
#line 1591 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 3:
#line 133 "SysY.y" /* yacc.c:1652  */
        {(yyval.decList) = A_castDecList((yyvsp[0].decList), (yyvsp[-1].decList));}
#line 1597 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 4:
#line 134 "SysY.y" /* yacc.c:1652  */
        {(yyval.decList) = A_DecList((yyvsp[0].dec), (yyvsp[-1].decList));}
#line 1603 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 5:
#line 135 "SysY.y" /* yacc.c:1652  */
        {(yyval.decList) = (yyvsp[0].decList);}
#line 1609 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 6:
#line 136 "SysY.y" /* yacc.c:1652  */
        {(yyval.decList) = A_DecList((yyvsp[0].dec), NULL);}
#line 1615 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 7:
#line 140 "SysY.y" /* yacc.c:1652  */
        {(yyval.decList) = (yyvsp[0].decList);}
#line 1621 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 8:
#line 141 "SysY.y" /* yacc.c:1652  */
        {(yyval.decList) = (yyvsp[0].decList);}
#line 1627 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 9:
#line 145 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.decList) = A_conform((yyvsp[-1].decList), (yyvsp[-2].sym));
        }
#line 1635 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 10:
#line 151 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.decList) = A_DecList((yyvsp[0].dec), (yyvsp[-2].decList));
        }
#line 1643 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 11:
#line 154 "SysY.y" /* yacc.c:1652  */
        {(yyval.decList) = A_DecList((yyvsp[0].dec), NULL);}
#line 1649 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 12:
#line 158 "SysY.y" /* yacc.c:1652  */
        {(yyval.sym) = S_Symbol("int");}
#line 1655 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 13:
#line 162 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.dec) = A_VariableDec(A_POS((yyloc)), NULL, (yyvsp[-2].sym), A_IntExp(A_POS((yylsp[0])), (yyvsp[0].integer)), true);
        }
#line 1663 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 14:
#line 165 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.dec) = (A_dec)A_ArrayDec(A_POS((yyloc)), NULL, (yyvsp[-3].sym), (A_expList)U_reverseList((yyvsp[-2].expList)), (A_arrayInitList)U_reverseList((yyvsp[0].arrayInitList)), true);
        }
#line 1671 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 15:
#line 171 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.expList) = A_ExpList(A_IntExp(A_POS((yylsp[-1])), (yyvsp[-1].integer)), (yyvsp[-3].expList));
        }
#line 1679 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 16:
#line 174 "SysY.y" /* yacc.c:1652  */
        {(yyval.expList) = A_ExpList(A_IntExp(A_POS((yylsp[-1])), (yyvsp[-1].integer)), NULL);}
#line 1685 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 17:
#line 178 "SysY.y" /* yacc.c:1652  */
        {(yyval.arrayInitList) = (yyvsp[-1].arrayInitList);}
#line 1691 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 18:
#line 179 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yyloc)), A_IntExp(A_POS((yyloc)), 0)), NULL);
        }
#line 1699 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 19:
#line 185 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[0])), (A_arrayInitList)U_reverseList((yyvsp[0].arrayInitList))), (yyvsp[-2].arrayInitList));
        }
#line 1707 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 20:
#line 188 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[-2])), A_IntExp(A_POS((yylsp[0])), (yyvsp[0].integer))), (yyvsp[-2].arrayInitList));
        }
#line 1715 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 21:
#line 191 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[0])), (A_arrayInitList)U_reverseList((yyvsp[0].arrayInitList))), NULL);
        }
#line 1723 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 22:
#line 194 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[0])), A_IntExp(A_POS((yylsp[0])), (yyvsp[0].integer))), NULL);
        }
#line 1731 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 23:
#line 200 "SysY.y" /* yacc.c:1652  */
        {(yyval.decList) = A_conform((yyvsp[-1].decList), (yyvsp[-2].sym));}
#line 1737 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 24:
#line 204 "SysY.y" /* yacc.c:1652  */
        {(yyval.decList) = A_DecList((yyvsp[0].dec), (yyvsp[-2].decList));}
#line 1743 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 25:
#line 205 "SysY.y" /* yacc.c:1652  */
        {(yyval.decList) = A_DecList((yyvsp[0].dec), NULL);}
#line 1749 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 26:
#line 209 "SysY.y" /* yacc.c:1652  */
        {(yyval.dec) = A_VariableDec(A_POS((yyloc)), NULL, (yyvsp[0].sym), NULL, false);}
#line 1755 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 27:
#line 210 "SysY.y" /* yacc.c:1652  */
        {(yyval.dec) = A_VariableDec(A_POS((yyloc)), NULL, (yyvsp[-2].sym), (yyvsp[0].exp), false);}
#line 1761 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 28:
#line 211 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.dec) = A_ArrayDec(A_POS((yyloc)), NULL, (yyvsp[-1].sym), (A_expList)U_reverseList((yyvsp[0].expList)), NULL, false);
        }
#line 1769 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 29:
#line 214 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.dec) = A_ArrayDec(A_POS((yyloc)), NULL, (yyvsp[-3].sym), (A_expList)U_reverseList((yyvsp[-2].expList)), (A_arrayInitList)U_reverseList((yyvsp[0].arrayInitList)), false);
        }
#line 1777 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 30:
#line 220 "SysY.y" /* yacc.c:1652  */
        {(yyval.arrayInitList) = (yyvsp[-1].arrayInitList);}
#line 1783 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 31:
#line 221 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yyloc)), A_IntExp(A_POS((yyloc)), 0)), NULL);
        }
#line 1791 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 32:
#line 227 "SysY.y" /* yacc.c:1652  */
        {(yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[0])), (yyvsp[0].exp)), (yyvsp[-2].arrayInitList));}
#line 1797 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 33:
#line 228 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[0])), (A_arrayInitList)U_reverseList((yyvsp[0].arrayInitList))), (yyvsp[-2].arrayInitList));
        }
#line 1805 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 34:
#line 231 "SysY.y" /* yacc.c:1652  */
        {(yyval.arrayInitList) = A_ArrayInitList(A_SingleInit(A_POS((yylsp[0])), (yyvsp[0].exp)), NULL);}
#line 1811 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 35:
#line 232 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.arrayInitList) = A_ArrayInitList(A_NestedInit(A_POS((yylsp[0])), (A_arrayInitList)U_reverseList((yyvsp[0].arrayInitList))), NULL);
        }
#line 1819 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 36:
#line 238 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[-4].sym), (A_fieldList)U_reverseList((yyvsp[-2].fieldList)), S_Symbol("void"), (yyvsp[0].stm));
        }
#line 1827 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 37:
#line 241 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[-3].sym), NULL, S_Symbol("void"), (yyvsp[0].stm));
        }
#line 1835 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 38:
#line 244 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[-4].sym), (A_fieldList)U_reverseList((yyvsp[-2].fieldList)), (yyvsp[-5].sym), (yyvsp[0].stm));
        }
#line 1843 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 39:
#line 247 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.dec) = A_FunctionDec(A_POS((yyloc)), (yyvsp[-3].sym), NULL, (yyvsp[-4].sym), (yyvsp[0].stm));
        }
#line 1851 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 40:
#line 253 "SysY.y" /* yacc.c:1652  */
        {(yyval.fieldList) = A_FieldList((yyvsp[0].field), (yyvsp[-2].fieldList));}
#line 1857 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 41:
#line 254 "SysY.y" /* yacc.c:1652  */
        {(yyval.fieldList) = A_FieldList((yyvsp[0].field), NULL);}
#line 1863 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 42:
#line 258 "SysY.y" /* yacc.c:1652  */
        {(yyval.field) = A_Field(A_POS((yyloc)), (yyvsp[0].sym), (yyvsp[-1].sym), NULL);}
#line 1869 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 43:
#line 259 "SysY.y" /* yacc.c:1652  */
        {(yyval.field) = A_Field(A_POS((yyloc)), (yyvsp[-1].sym), (yyvsp[-2].sym), (A_expList)U_reverseList((yyvsp[0].expList)));}
#line 1875 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 44:
#line 263 "SysY.y" /* yacc.c:1652  */
        {(yyval.expList) = A_ExpList((yyvsp[-1].exp), (yyvsp[-3].expList));}
#line 1881 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 45:
#line 264 "SysY.y" /* yacc.c:1652  */
        {(yyval.expList) = A_ExpList(A_IntExp(A_POS((yyloc)), 0), NULL);}
#line 1887 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 46:
#line 265 "SysY.y" /* yacc.c:1652  */
        {(yyval.expList) = A_ExpList((yyvsp[-1].exp), NULL);}
#line 1893 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 47:
#line 269 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = A_BlockStm(A_POS((yyloc)), (A_comStmList)U_reverseList((yyvsp[-1].comStmList)));}
#line 1899 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 48:
#line 273 "SysY.y" /* yacc.c:1652  */
        {(yyval.comStmList) = A_ComStmList(A_ComStm((yyvsp[0].decList), NULL), (yyvsp[-1].comStmList));}
#line 1905 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 49:
#line 274 "SysY.y" /* yacc.c:1652  */
        {(yyval.comStmList) = A_ComStmList(A_ComStm(NULL, (yyvsp[0].stm)), (yyvsp[-1].comStmList));}
#line 1911 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 50:
#line 275 "SysY.y" /* yacc.c:1652  */
        {(yyval.comStmList) = A_ComStmList(A_ComStm((yyvsp[0].decList), NULL), NULL);}
#line 1917 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 51:
#line 276 "SysY.y" /* yacc.c:1652  */
        {(yyval.comStmList) = A_ComStmList(A_ComStm(NULL, (yyvsp[0].stm)), NULL);}
#line 1923 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 52:
#line 280 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = A_AssignStm(A_POS((yyloc)), (yyvsp[-3].var), (yyvsp[-1].exp));}
#line 1929 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 53:
#line 281 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = A_ExpStm(A_POS((yyloc)), (yyvsp[-1].exp));}
#line 1935 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 54:
#line 282 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = (yyvsp[0].stm);}
#line 1941 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 55:
#line 283 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = A_IfStm(A_POS((yyloc)), (yyvsp[-2].exp), (yyvsp[0].stm), NULL);}
#line 1947 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 56:
#line 284 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = A_IfStm(A_POS((yyloc)), (yyvsp[-4].exp), (yyvsp[-2].stm), (yyvsp[0].stm));}
#line 1953 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 57:
#line 285 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = A_WhileStm(A_POS((yyloc)), (yyvsp[-2].exp), (yyvsp[0].stm));}
#line 1959 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 58:
#line 286 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = A_BreakStm(A_POS((yyloc)));}
#line 1965 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 59:
#line 287 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = A_ContinueStm(A_POS((yyloc)));}
#line 1971 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 60:
#line 288 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = A_ReturnStm(A_POS((yyloc)), (yyvsp[-1].exp));}
#line 1977 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 61:
#line 289 "SysY.y" /* yacc.c:1652  */
        {(yyval.stm) = A_ReturnStm(A_POS((yyloc)), NULL);}
#line 1983 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 62:
#line 293 "SysY.y" /* yacc.c:1652  */
        {(yyval.expList) = A_ExpList((yyvsp[0].exp), (yyvsp[-2].expList));}
#line 1989 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 63:
#line 294 "SysY.y" /* yacc.c:1652  */
        {(yyval.expList) = A_ExpList((yyvsp[0].exp), NULL);}
#line 1995 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 64:
#line 298 "SysY.y" /* yacc.c:1652  */
        {(yyval.var) = A_SimpleVar(A_POS((yylsp[0])), (yyvsp[0].sym));}
#line 2001 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 65:
#line 299 "SysY.y" /* yacc.c:1652  */
        {(yyval.var) = A_ArrayVar(A_POS((yylsp[-3])), (yyvsp[-3].var), (yyvsp[-1].exp));}
#line 2007 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 66:
#line 303 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = A_LogicExp(A_POS((yyloc)), (yyvsp[-1].binOp), (yyvsp[-2].exp), (yyvsp[0].exp));}
#line 2013 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 67:
#line 304 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = (yyvsp[-1].exp);}
#line 2019 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 68:
#line 305 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = A_LogicExp(A_POS((yyloc)), A_not, (yyvsp[0].exp), NULL);}
#line 2025 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 69:
#line 306 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = A_LogicExp(A_POS((yyloc)), A_and, (yyvsp[-2].exp), (yyvsp[0].exp));}
#line 2031 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 70:
#line 307 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = A_LogicExp(A_POS((yyloc)), A_or, (yyvsp[-2].exp), (yyvsp[0].exp));}
#line 2037 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 71:
#line 311 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_eq;}
#line 2043 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 72:
#line 312 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_ne;}
#line 2049 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 73:
#line 313 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_lt;}
#line 2055 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 74:
#line 314 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_le;}
#line 2061 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 75:
#line 315 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_gt;}
#line 2067 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 76:
#line 316 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_ge;}
#line 2073 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 77:
#line 320 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_add;}
#line 2079 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 78:
#line 321 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_sub;}
#line 2085 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 79:
#line 325 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_mul;}
#line 2091 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 80:
#line 326 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_div;}
#line 2097 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 81:
#line 327 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_mod;}
#line 2103 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 82:
#line 331 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_add;}
#line 2109 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 83:
#line 332 "SysY.y" /* yacc.c:1652  */
        {(yyval.binOp) = A_sub;}
#line 2115 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 84:
#line 336 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = A_OpExp(A_POS((yyloc)), (yyvsp[-2].exp), (yyvsp[-1].binOp), (yyvsp[0].exp));}
#line 2121 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 85:
#line 337 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = A_OpExp(A_POS((yyloc)),(yyvsp[-2].exp), (yyvsp[-1].binOp), (yyvsp[0].exp));}
#line 2127 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 86:
#line 338 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = A_OpExp(A_POS((yyloc)), A_IntExp(A_POS((yyloc)), 0), (yyvsp[-1].binOp), (yyvsp[0].exp));}
#line 2133 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 87:
#line 339 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = (yyvsp[-1].exp);}
#line 2139 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 88:
#line 340 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = A_VarExp(A_POS((yyloc)), (yyvsp[0].var));}
#line 2145 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 89:
#line 341 "SysY.y" /* yacc.c:1652  */
        {
            (yyval.exp) = A_CallExp(A_POS((yyloc)), (yyvsp[-3].sym), (A_expList)U_reverseList((yyvsp[-1].expList)));
        }
#line 2153 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 90:
#line 344 "SysY.y" /* yacc.c:1652  */
        {(yyval.exp) = A_IntExp(A_POS((yyloc)), (yyvsp[0].integer));}
#line 2159 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 91:
#line 348 "SysY.y" /* yacc.c:1652  */
        {(yyval.integer) = calculate((yyvsp[-2].integer), (yyvsp[-1].binOp), (yyvsp[0].integer));}
#line 2165 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 92:
#line 349 "SysY.y" /* yacc.c:1652  */
        {(yyval.integer) = calculate((yyvsp[-2].integer), (yyvsp[-1].binOp), (yyvsp[0].integer));}
#line 2171 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 93:
#line 350 "SysY.y" /* yacc.c:1652  */
        {(yyval.integer) = calculate(0, (yyvsp[-1].binOp), (yyvsp[0].integer));}
#line 2177 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 94:
#line 351 "SysY.y" /* yacc.c:1652  */
        {(yyval.integer) = (yyvsp[-1].integer);}
#line 2183 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 95:
#line 352 "SysY.y" /* yacc.c:1652  */
        {(yyval.integer) = (yyvsp[0].integer);}
#line 2189 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 96:
#line 356 "SysY.y" /* yacc.c:1652  */
        {(yyval.integer) = (yyvsp[0].integer);}
#line 2195 "SysY.tab.c" /* yacc.c:1652  */
            break;

        case 97:
#line 360 "SysY.y" /* yacc.c:1652  */
        {(yyval.sym) = S_Symbol((yyvsp[0].string));}
#line 2201 "SysY.tab.c" /* yacc.c:1652  */
            break;


#line 2205 "SysY.tab.c" /* yacc.c:1652  */
        default: break;
    }
    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action invokes
       YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
       if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
       incorrect destructor might then be invoked immediately.  In the
       case of YYERROR or YYBACKUP, subsequent parser actions might lead
       to an incorrect destructor call or verbose syntax error message
       before the lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

    YYPOPSTACK (yylen);
    yylen = 0;
    YY_STACK_PRINT (yyss, yyssp);

    *++yyvsp = yyval;
    *++yylsp = yyloc;

    /* Now 'shift' the result of the reduction.  Determine what state
       that goes to, based on the state we popped back to and the rule
       number reduced by.  */
    {
        const int yylhs = yyr1[yyn] - YYNTOKENS;
        const int yyi = yypgoto[yylhs] + *yyssp;
        yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
                   ? yytable[yyi]
                   : yydefgoto[yylhs]);
    }

    goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
    yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus)
    {
        ++yynerrs;
#if ! YYERROR_VERBOSE
        yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
        {
            char const *yymsgp = YY_("syntax error");
            int yysyntax_error_status;
            yysyntax_error_status = YYSYNTAX_ERROR;
            if (yysyntax_error_status == 0)
                yymsgp = yymsg;
            else if (yysyntax_error_status == 1)
            {
                if (yymsg != yymsgbuf)
                    YYSTACK_FREE (yymsg);
                yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
                if (!yymsg)
                {
                    yymsg = yymsgbuf;
                    yymsg_alloc = sizeof yymsgbuf;
                    yysyntax_error_status = 2;
                }
                else
                {
                    yysyntax_error_status = YYSYNTAX_ERROR;
                    yymsgp = yymsg;
                }
            }
            yyerror ((char*)yymsgp);
            if (yysyntax_error_status == 2)
                goto yyexhaustedlab;
        }
# undef YYSYNTAX_ERROR
#endif
    }

    yyerror_range[1] = yylloc;

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
    /* Pacify compilers when the user code never invokes YYERROR and the
       label yyerrorlab therefore never appears in user code.  */
    if (0)
        YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
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
    yyerrstatus = 3;      /* Each real token shifted decrements this.  */

    for (;;)
    {
        yyn = yypact[yystate];
        if (!yypact_value_is_default (yyn))
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

        yyerror_range[1] = *yylsp;
        yydestruct ("Error: popping",
                    yystos[yystate], yyvsp, yylsp);
        YYPOPSTACK (1);
        yystate = *yyssp;
        YY_STACK_PRINT (yyss, yyssp);
    }

    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    *++yyvsp = yylval;
    YY_IGNORE_MAYBE_UNINITIALIZED_END

    yyerror_range[2] = yylloc;
    /* Using YYLLOC is tempting, but would change the location of
       the lookahead.  YYLOC is available though.  */
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
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


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
    yyexhaustedlab:
    yyerror (YY_("memory exhausted"));
    yyresult = 2;
    /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
    yyreturn:
    if (yychar != YYEMPTY)
    {
        /* Make sure we have latest lookahead translation.  See comments at
           user semantic actions for why this is necessary.  */
        yytoken = YYTRANSLATE (yychar);
        yydestruct ("Cleanup: discarding lookahead",
                    yytoken, &yylval, &yylloc);
    }
    /* Do not reclaim the symbols of the rule whose action triggered
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
    return yyresult;
}
#line 363 "SysY.y" /* yacc.c:1918  */


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

