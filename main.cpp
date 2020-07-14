#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstring>
#include <io.h>
#include "util.h"
#include "ast.h"
#include "semant.h"
#include "env.h"
#include "prast.h"
#include "dirent.h"

using namespace std;


extern FILE *yyin;
extern FILE *yyout;
extern char *yytext;

extern int yyparse();

/**
 * 跨文件全局变量
 */
extern A_decList absyn_root;
c_string INPUT_FILE;
c_string OUTPUT_FILE;


void Test_out() {
    //遍历文件
    INPUT_FILE = "D:\\CLionProjects\\untitled\\sysyruntimelibrary-master\\functional_test";
    OUTPUT_FILE="D:\\CLionProjects\\untitled\\sysyruntimelibrary-master\\functional_test_out";
    int i = 0;
    int filesize = 0;
    DIR *dir = NULL;
    struct dirent *entry;
    if ((dir = opendir(INPUT_FILE)) == NULL) {
        printf("opendir failed!");
        return;
    } else {
        while (entry = readdir(dir)) {
            i++;
            if (entry->d_name[entry->d_namlen - 2] == 's' && entry->d_name[entry->d_namlen - 1] == 'y') {
                // window下Clion绝对路径
                string d_name=entry->d_name;
                string test_input=INPUT_FILE+string("\\")+d_name;
                string test_out=OUTPUT_FILE+string("\\")+d_name.substr(0,entry->d_namlen-2)+string("out");



                yyin=fopen(test_input.data(),"r");
                yyout=fopen(test_out.data(),"w");
                FILE_NAME=(c_string)(test_input.c_str());

                S_table tenv = E_base_typeEntry();
                S_table venv = E_base_valueEntry();

                yyparse();

                FILE *out=yyout;


                fprintf(out,"test_input = %s\n", test_input.c_str());
                fprintf(out,"test_out = %s\n", test_out.c_str());

                fprintf(out, "before semantic ast:\n");
                pr_decList(out, absyn_root, 0);
                fprintf(out, "\n");

                SEM_transProgram(venv, tenv, absyn_root);
                fprintf(out, "\nsemantic check finish !\n");

                fprintf(out, "after semantic ast:\n");
                pr_decList(out, absyn_root, 0);

                fclose(yyin);
                fclose(yyout);
            }
        }
        closedir(dir);
    }
    return;
};

int main(int argc, char **argv) {


    /**
     * 参数处理 --loyx 2020/6/15
     */
    int option;
    while ((option = getopt(argc, argv, "So:O::")) != -1) {
        switch (option) {
            case 'o':
                OUTPUT_FILE = String(optarg);
                break;
            case 'O':
                if (*optarg == '2') {
                    // todo O2
                } else {
                    // todo O0
                }
                break;
            default:
                // 此处匹配-S选项
                break;
        }
    }
    INPUT_FILE = argv[optind];

    Test_out();


    /**
     * open the input file
     */
    yyin = fopen(INPUT_FILE, "r");
    yyout = stdout;
    FILE_NAME = INPUT_FILE; // todo refactor?


    S_table tenv = E_base_typeEntry();
    S_table venv = E_base_valueEntry();

    yyparse();

    fprintf(stderr, "before semantic ast:\n");
    pr_decList(stderr, absyn_root, 0);
    fprintf(stderr, "\n");

    SEM_transProgram(venv, tenv, absyn_root);
    fprintf(stderr, "\nsemantic check finish !\n");

    fprintf(stderr, "after semantic ast:\n");
    pr_decList(stderr, absyn_root, 0);

    return 0;
}





