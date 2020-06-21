%{
#include "SysY.tab.h"
#include "util.h"
#include "errormsg.h"

extern int yywrap();
extern void yyerror();

string striped(char*);
int otoi(char* text);
int htoi(char* text);

int yycolumn = 1;

#define ADJ (EM_add_token(yytext, yyleng))
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \
    yycolumn += yyleng;
%}

/* lex defines */

add_ops "+"|"-"
mul_ops "*"|"/"
letter [_a-zA-z]
no_zero_num [1-9]
num 0|{no_zero_num}
unsign_int {no_zero_num}({num})*
character \'{add_ops}?\'|\'{mul_ops}?\'|\'{letter}?\'|\'{num}?\' 
string \"([\32\33\35-\126])*\"

%option yylineno
%Start COMMENT1 COMMENT2
%%
<INITIAL>"//" {BEGIN COMMENT1;}
<COMMENT1>. {/*in comment1*/}
<COMMENT1>\n|\r|\r\n {BEGIN INITIAL;}

<INITIAL>"/*" {BEGIN COMMENT2;}
<COMMENT2>.|\n|\r|\r\n {/*in comment2*/}
<COMMENT2>"*/" {BEGIN INITIAL;}

<INITIAL>\+ { ADJ;  return ADD; }
<INITIAL>\- { ADJ;  return SUB; }
<INITIAL>\* { ADJ;  return MUL; }
<INITIAL>\/ { ADJ;  return DIV; }
<INITIAL>"%" { ADJ; return MOD; }
<INITIAL>\< { ADJ;  return LT; }
<INITIAL>\<= { ADJ;  return LE; }
<INITIAL>\> { ADJ;  return GT; }
<INITIAL>\>= { ADJ;  return GE; }
<INITIAL>"!" { ADJ;  return NOT; }
<INITIAL>"&&" { ADJ;  return AND; }
<INITIAL>"||" { ADJ;  return OR; }
<INITIAL>== { ADJ;  return EQ; }
<INITIAL>!= { ADJ;  return NE; }
<INITIAL>= { ADJ;  return ASSIGNMENT; }
<INITIAL>const { ADJ;  return CONST; }
<INITIAL>int { ADJ;  return INT; }
<INITIAL>char { ADJ;  return CHAR; }
<INITIAL>void { ADJ;  return VOID; }
<INITIAL>if { ADJ;  return IF; }
<INITIAL>else { ADJ;  return ELSE; }
<INITIAL>while { ADJ;  return WHILE; }
<INITIAL>switch { ADJ;  return SWITCH; }
<INITIAL>case { ADJ;  return CASE; }
<INITIAL>return { ADJ;  return RETURN; }
<INITIAL>break { ADJ;  return BREAK; }
<INITIAL>continue { ADJ;  return CONTINUE; }
<INITIAL>, { ADJ;  return COMMA; }
<INITIAL>; { ADJ;  return SEMICOLON; }
<INITIAL>: { ADJ;  return COLON; }
<INITIAL>"(" { ADJ;  return L_PARENTHESIS; }
<INITIAL>")" { ADJ;  return R_PARENTHESIS; }
<INITIAL>"[" { ADJ;  return L_BRACKETS; }
<INITIAL>"]" { ADJ;  return R_BRACKETS; }
<INITIAL>"{" { ADJ; return L_BRACE; }
<INITIAL>"}" { ADJ; return R_BRACE; } 
<INITIAL>[a-zA-Z_][a-zA-Z0-9_]* { ADJ; yylval.string = String(yytext); return ID; }
<INITIAL>{unsign_int}|0 { ADJ; yylval.integer = atoi(yytext); return INTEGER;  }
<INITIAL>0[0-7]* { ADJ; yylval.integer = otoi(yytext); return INTEGER; }
<INITIAL>(0x|0X)[0-9a-fA-F]+ { ADJ; yylval.integer = htoi(yytext); return INTEGER; }
<INITIAL>{string} { ADJ; yylval.string = striped(yytext); return STRING; } 
<INITIAL>{character} { ADJ; yylval.character = *striped(yytext); return CHARACTER; } 

<INITIAL>[ \t]+ { /* space */ ADJ; }
<INITIAL>(\n|\r|\n\r)+ {yycolumn = 1;EM_clear_buf();}
<INITIAL>. { /* error */ yyerror("invalid character"); }

%%

string striped(char* st){
    /** 去掉左右括号，返回一个string*/
    int len = strlen(st);
    string striped_str = (string)malloc((len-1)*sizeof(char));
    for(int i = 1; i < len-1; i++){
        striped_str[i-1] = st[i];
    }
    striped_str[len-2] = 0;
    return striped_str;
}

char* stripZero(char* st){
    /** 去除8进制和16进制的前导0 */
    for(; *st == '0' && *(st+1); st++);
    assert(st);
    return st;
}

int otoi(char* text){
    /** 8进制转10进制*/
    assert(text[0] == '0');
    char *ptr;
    int res=(int)strtol(text,&ptr,8);
//    char* str_num = stripZero(text);
//    int res = *str_num - '0';
//    while(*(++str_num)){res = 8 * res + *str_num - '0';}
//    return res;
}

 /// 16进制字符转对应的数值
#define transHexP(p) (p-'0'<10?p-'0':(p-'A'<6?p-'7':p-'W'))

int htoi(char* text){
    /** 16进制转10进制*/
    char *ptr;
    int res=(int)strtol(text,&ptr,16);
//    assert(text[0] == '0' && (text[1] == 'x' | text[1] == 'X'));
//    char* str_num = stripZero(text+2);
//    int res = transHexP(*str_num);
//    while(*(++str_num)){res = 16 * res + transHexP(*str_num);}
//    return res;
}

