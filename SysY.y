%{
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


%}
%union{
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
}

%token ADD
%token SUB
%token MUL
%token DIV
%token MOD
%token LT
%token LE
%token GT
%token GE
%token NOT
%token AND
%token OR
%token EQ
%token NE
%token ASSIGNMENT
%token CONST
%token INT
%token CHAR
%token VOID
%token IF
%token ELSE
%token WHILE
%token SWITCH
%token CASE
%token RETURN
%token BREAK
%token CONTINUE
%token COMMA
%token SEMICOLON
%token COLON
%token L_PARENTHESIS
%token R_PARENTHESIS
%token L_BRACKETS
%token R_BRACKETS
%token L_BRACE
%token R_BRACE
%token <string> ID
%token <integer> INTEGER
%token <string> STRING
%token <character> CHARACTER

%type <var> LVal
%type <integer> Number
%type <sym> Ident BType
%type <binOp> addOp mulOp UnaryOp RelOp EqOp
%type <stm> Block Stmt
%type <comStmList> BlockItem
%type <expList> ConstSubscripts ExpSubscripts FuncRParams
%type <exp> Exp Cond ConstExp LOrExp LAndExp EqExp RelExp AddExp MulExp UnaryExp PrimaryExp
%type <decList> CompUnit Decl ConstDecl ConstDefList VarDecl VarDefList
%type <dec> ConstDef VarDef FuncDef
%type <arrayInitList> ConstInitValList ConstInitVal InitVal InitValList
%type <field> FuncFParam
%type <fieldList> FuncFParams


%locations
%error-verbose
%start program
%%
program:
    CompUnit {absyn_root = (A_decList)U_reverseList($1);}
    ;

CompUnit:
    CompUnit Decl {$$ = A_castDecList($2, $1);}
    | CompUnit FuncDef {$$ = A_DecList($2, $1);}
    | Decl {$$ = $1;}
    | FuncDef {$$ = A_DecList($1, NULL);}
    ;

Decl:
    ConstDecl {$$ = $1;}
    | VarDecl {$$ = $1;}
    ;

ConstDecl:
    CONST BType ConstDefList SEMICOLON  {
        $$ = A_conform($3, $2);
    }
    ;

ConstDefList:
    ConstDefList COMMA ConstDef {
        $$ = A_DecList($3, $1);
    }
    | ConstDef {$$ = A_DecList($1, NULL);}
    ;

BType:
    INT {$$ = S_Symbol("int");}
    ;

ConstDef:
    Ident ASSIGNMENT ConstExp {
        $$ = A_VariableDec(A_POS(@$), NULL, $1, $3, true);
    }
    | Ident ConstSubscripts ASSIGNMENT ConstInitVal {
        $$ = A_ArrayDec(A_POS(@$), NULL, $1, (A_expList)U_reverseList($2), $4, true);
    }
    ;

ConstSubscripts:
    ConstSubscripts L_BRACKETS ConstExp R_BRACKETS {
        $$ = A_ExpList($3, $1);
    }
    | L_BRACKETS ConstExp R_BRACKETS {$$ = A_ExpList($2, NULL);}
    ;

ConstInitVal:
    L_BRACE ConstInitValList R_BRACE {$$ = (A_arrayInitList)U_reverseList($2);}
    | L_BRACE R_BRACE {
        $$ = A_ArrayInitList(A_SingleInit(A_POS(@$), A_IntExp(A_POS(@$), 0)), NULL);
    }
    ;

ConstInitValList:
    ConstInitValList COMMA ConstInitVal {
        $$ = A_ArrayInitList(A_NestedInit(A_POS(@3), $3), $1);
    }
    | ConstInitValList COMMA ConstExp {
        $$ = A_ArrayInitList(A_SingleInit(A_POS(@1), $3), $1);
    }
    | ConstInitVal {
        $$ = A_ArrayInitList(A_NestedInit(A_POS(@1), $1), NULL);
    }
    | ConstExp {
        $$ = A_ArrayInitList(A_SingleInit(A_POS(@1), $1), NULL);
    }
    ;

VarDecl:
    BType VarDefList SEMICOLON {$$ = A_conform($2, $1);}
    ;

VarDefList:
    VarDefList COMMA VarDef {$$ = A_DecList($3, $1);}
    | VarDef {$$ = A_DecList($1, NULL);}
    ;

VarDef:
    Ident {$$ = A_VariableDec(A_POS(@$), NULL, $1, NULL, false);}
    | Ident ASSIGNMENT Exp {$$ = A_VariableDec(A_POS(@$), NULL, $1, $3, false);}
    | Ident ConstSubscripts {
        $$ = A_ArrayDec(A_POS(@$), NULL, $1, (A_expList)U_reverseList($2), NULL, false);
    }
    | Ident ConstSubscripts ASSIGNMENT InitVal {
        $$ = A_ArrayDec(A_POS(@$), NULL, $1, (A_expList)U_reverseList($2), $4, false);
    }
    ;

InitVal:
    L_BRACE InitValList R_BRACE {$$ = (A_arrayInitList)U_reverseList($2);}
    | L_BRACE R_BRACE {
        $$ = A_ArrayInitList(A_SingleInit(A_POS(@$), A_IntExp(A_POS(@$), 0)), NULL);
    }
    ;

InitValList:
    InitValList COMMA Exp {
        $$ = A_ArrayInitList(A_SingleInit(A_POS(@3), $3), $1);
    }
    | InitValList COMMA InitVal {
        $$ = A_ArrayInitList(A_NestedInit(A_POS(@3), $3), $1);
    }
    | Exp {
        $$ = A_ArrayInitList(A_SingleInit(A_POS(@1), $1), NULL);
    }
    | InitVal {
        $$ = A_ArrayInitList(A_NestedInit(A_POS(@1), $1), NULL);
    }
    ;

FuncDef:
    VOID Ident L_PARENTHESIS FuncFParams R_PARENTHESIS Block {
        $$ = A_FunctionDec(A_POS(@$), $2, (A_fieldList)U_reverseList($4), S_Symbol("void"), $6);
    }
    | VOID Ident L_PARENTHESIS R_PARENTHESIS Block {
        $$ = A_FunctionDec(A_POS(@$), $2, NULL, S_Symbol("void"), $5);
    }
    | BType Ident L_PARENTHESIS FuncFParams R_PARENTHESIS Block {
        $$ = A_FunctionDec(A_POS(@$), $2, (A_fieldList)U_reverseList($4), $1, $6);
    }
    | BType Ident L_PARENTHESIS R_PARENTHESIS Block {
        $$ = A_FunctionDec(A_POS(@$), $2, NULL, $1, $5);
    }
    ;

FuncFParams:
    FuncFParams COMMA FuncFParam {$$ = A_FieldList($3, $1);}
    | FuncFParam {$$ = A_FieldList($1, NULL);}
    ;

FuncFParam:
    BType Ident {$$ = A_Field(A_POS(@$), $2, $1, NULL);}
    | BType Ident  ExpSubscripts {$$ = A_Field(A_POS(@$), $2, $1, (A_expList)U_reverseList($3));}
    ;

ExpSubscripts:
    ExpSubscripts L_BRACKETS Exp R_BRACKETS {$$ = A_ExpList($3, $1);}
    | L_BRACKETS R_BRACKETS {$$ = A_ExpList(A_IntExp(A_POS(@$), 0), NULL);}

   | L_BRACKETS Exp R_BRACKETS {$$ = A_ExpList($2, NULL);}
    ;

Block:
    L_BRACE BlockItem R_BRACE {$$ = A_BlockStm(A_POS(@$), (A_comStmList)U_reverseList($2));}
	|L_BRACE R_BRACE {$$ = A_BlockStm(A_POS(@$),NULL);}
    ;

BlockItem:
    BlockItem Decl {$$ = A_ComStmList(A_ComStm($2, NULL), $1);}
    | BlockItem Stmt {$$ = A_ComStmList(A_ComStm(NULL, $2), $1);}
    | Decl {$$ = A_ComStmList(A_ComStm($1, NULL), NULL);}
    | Stmt {$$ = A_ComStmList(A_ComStm(NULL, $1), NULL);}
    ;

Stmt:
    LVal ASSIGNMENT Exp SEMICOLON {$$ = A_AssignStm(A_POS(@$), $1, $3);}
    | Exp SEMICOLON {$$ = A_ExpStm(A_POS(@$), $1);}
	| SEMICOLON {$$ = A_ExpStm(A_POS(@$),NULL);}
    | Block {$$ = $1;}
    | IF L_PARENTHESIS Cond R_PARENTHESIS Stmt {$$ = A_IfStm(A_POS(@$), $3, $5, NULL);}
    | IF L_PARENTHESIS Cond R_PARENTHESIS Stmt ELSE Stmt {$$ = A_IfStm(A_POS(@$), $3, $5, $7);}
    | WHILE L_PARENTHESIS Cond R_PARENTHESIS Stmt {$$ = A_WhileStm(A_POS(@$), $3, $5);}
    | BREAK SEMICOLON {$$ = A_BreakStm(A_POS(@$));}
    | CONTINUE SEMICOLON {$$ = A_ContinueStm(A_POS(@$));}
    | RETURN Exp SEMICOLON {$$ = A_ReturnStm(A_POS(@$), $2);}
    | RETURN SEMICOLON {$$ = A_ReturnStm(A_POS(@$), NULL);}
    ;

FuncRParams:
    FuncRParams COMMA Exp {$$ = A_ExpList($3, $1);}
    | Exp {$$ = A_ExpList($1, NULL);}
    ;

LVal:
    Ident {$$ = A_SimpleVar(A_POS(@1), $1);}
    | LVal L_BRACKETS Exp R_BRACKETS {$$ = A_ArrayVar(A_POS(@1), $1, $3);}
    ;

Cond:
    LOrExp {$$ = $1}
    ;

LOrExp:
    LAndExp {$$ = $1}
    | LOrExp OR LAndExp {$$ = A_OpExp(A_POS(@$), $1, A_and, $3)}
    ;

LAndExp:
    EqExp {$$ = $1}
    | LAndExp AND EqExp {$$ = A_OpExp(A_POS(@$), $1, A_or, $3)}
    ;

EqExp:
    RelExp {$$ = $1}
    | EqExp EqOp RelExp {$$ = A_OpExp(A_POS(@$), $1, $2, $3)}
    ;

RelExp:
    AddExp {$$ = $1}
    | RelExp RelOp AddExp {$$ = A_OpExp(A_POS(@$), $1, $2, $3)}
    ;

RelOp:
    LT {$$ = A_lt;}
    | LE {$$ = A_le;}
    | GT {$$ = A_gt;}
    | GE {$$ = A_ge;}
    ;

EqOp:
    EQ {$$ = A_eq;}
    | NE {$$ = A_ne;}
    ;

addOp:
    ADD {$$ = A_add;}
    | SUB {$$ = A_sub;}
    ;

mulOp:
    MUL {$$ = A_mul;}
    |DIV {$$ = A_div;}
    |MOD {$$ = A_mod;}
    ;

UnaryOp:
    ADD {$$ = A_add;}
    | SUB {$$ = A_sub;}
    | NOT {$$ = A_not;}
    ;

Exp:
    AddExp {$$ = $1}
    ;

AddExp:
    MulExp {$$ = $1}
    | AddExp addOp MulExp {$$ = A_OpExp(A_POS(@$), $1, $2, $3)}
    ;

MulExp:
    UnaryExp {$$ = $1}
    | MulExp mulOp UnaryExp {$$ = A_OpExp(A_POS(@$), $1, $2, $3)}
    ;

UnaryExp:
    PrimaryExp {$$ = $1}
    | Ident L_PARENTHESIS FuncRParams R_PARENTHESIS {
        $$ = A_CallExp(A_POS(@$), $1, (A_expList)U_reverseList($3));
    }
    | Ident L_PARENTHESIS R_PARENTHESIS{
        $$ = A_CallExp(A_POS(@$), $1, NULL);
    }
    | UnaryOp UnaryExp {
        $$ = A_OpExp(A_POS(@$), A_IntExp(A_POS(@$),0), $1, $2)
    }
    ;

PrimaryExp:
    L_PARENTHESIS Exp R_PARENTHESIS {$$ = $2}
    | LVal {$$ = A_VarExp(A_POS(@$), $1);}
    | Number {$$ = A_IntExp(A_POS(@$), $1);}
    | STRING {$$ = A_StringExp(A_POS(@$), $1);}
    ;

ConstExp:
    AddExp {$$ = $1;}
    ;

Number:
    INTEGER {$$ = $1;}
    ;

Ident:
    ID {$$ = S_Symbol($1);}
    ;

%%

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

