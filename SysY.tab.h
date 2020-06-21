/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */
#include "ast.h"
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
#line 44 "SysY.y" /* yacc.c:1921  */

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

#line 122 "SysY.tab.h" /* yacc.c:1921  */
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
