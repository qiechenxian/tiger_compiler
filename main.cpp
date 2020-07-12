#include <cstdio>
#include <unistd.h>
#include "util.h"
#include "ast.h"
#include "semant.h"
#include "env.h"
#include "prast.h"

extern FILE* yyin;
extern FILE* yyout;
extern char* yytext;
extern int yyparse();

/**
 * 跨文件全局变量
 */
extern  A_decList absyn_root;
c_string INPUT_FILE;
c_string OUTPUT_FILE;



int main(int argc, char ** argv) {


    /**
     * 参数处理 --loyx 2020/6/15
     */

    int option;
    while ( (option = getopt(argc, argv, "So:O::")) != -1){
        switch (option) {
            case 'o':
                OUTPUT_FILE = String(optarg);
                break;
            case 'O':
                if (*optarg == '2'){
                    // todo O2
                }else{
                    // todo O0
                }
                break;
            default:
                // 此处匹配-S选项
                break;
        }
    }
    INPUT_FILE = argv[optind];


    /**
     * open the input file
     */
    yyin = fopen(INPUT_FILE, "r");
    yyout = stdout;
    FILE_NAME = INPUT_FILE; // todo refactor?


    S_table tenv = E_base_typeEntry();
    S_table venv = E_base_valueEntry();

    yyparse();

    printf("before semantic ast:\n");
    pr_decList(stderr,absyn_root,0);
    printf("\n");

    SEM_transProgram(venv, tenv, absyn_root);
    printf("\nsemantic check finish !\n");

    printf("after semantic ast:\n");
    pr_decList(stderr, absyn_root, 0);

    return 0;
}

