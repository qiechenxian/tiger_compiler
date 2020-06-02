#include <stdio.h>
#include "util.h"
#include "absyn.h"
#include "semant.h"
#include "env.h"

extern FILE* yyin;
extern FILE* yyout;
extern char* yytext;
extern int yyparse();

A_decList absyn_root;


int main(int argc, char ** argv) {
    yyin = fopen(argv[1], "r");
    yyout = stdout;
    FILE_NAME = argv[1];
    S_table tenv = E_base_typeEntry();
    S_table venv = E_base_valueEntry();

    yyparse();

    SEM_transProgram(venv, tenv, absyn_root);
    printf("\nsemantic check finish !\n");

    return 0;
}
