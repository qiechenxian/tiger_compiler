#include <cstdio>
#include <unistd.h>
#include "util.h"
#include "ast.h"
#include "semant.h"
#include "env.h"
#include "prast.h"
#include "cannon.h"
#include "print_ir_tree.h"
#include "codegen.h"
#include "regalloc.h"
using namespace std;

/**
 * 跨文件全局变量
 */
extern A_decList absyn_root;
c_string INPUT_FILE;
c_string OUTPUT_FILE;

static void doProc(FILE *outfile,F_frame frame, T_stm body) {

    FILE *out = stderr;

    AS_proc proc;
    //struct RA_result allocation;
    T_stmList stmList;
    AS_instrList iList;

    F_tempMap = Temp_empty();

    stmList = C_linearize(body);
    stmList = C_traceSchedule(C_basicBlocks(stmList,frame));
//    printcannoList(stderr, stmList);

    iList = F_codegen(frame, stmList);

    iList = F_procEntryExit2(iList);
    //fprintf(outfile,"%s",iList->tail->head->u.LABEL.assem);
    struct RA_result ra=RA_regAlloc(frame,iList);
    iList=ra.il;

    AS_proc proc_done = F_procEntryExit3(frame, iList);
    fprintf(outfile, "%s", proc_done->prolog);
    AS_printInstrList(outfile,proc_done->body,Temp_layerMap(F_tempMap, Temp_layerMap( ra.coloring, Temp_name())));
//    AS_printInstrList(outfile,proc_done->body,Temp_layerMap(F_tempMap,  Temp_name()));
    fprintf(outfile, "%s\n", proc_done->epilog);

}

static void doGlobal(FILE *outfile, F_fragList fragList){
    int word_size = get_word_size();
    fprintf(outfile, "\t.arch   armv7-a\n");
    fprintf(outfile, "\t.file   \"%s\"\n", INPUT_FILE);
    fprintf(outfile, "\t.data\n");
    for (F_fragList iter = fragList; iter; iter = iter->tail){
        F_frag frag = iter->head;
        if (frag->kind == F_frag_::F_globalFrag){
            int size = frag->u.global.size*word_size;
            char* name = S_getName(frag->u.global.label);
            if (frag->u.global.comm){
                fprintf(outfile, "\t.comm   %s, %d, %d\n", name, size, word_size);
//                fprintf(outfile, "\n");
                continue;
            }
            fprintf(outfile, "\t.global %s\n", name);
            fprintf(outfile, "\t.align  2\n");
            fprintf(outfile, "\t.type   %s, %%object\n", name);
            fprintf(outfile, "\t.size   %s, %d\n", name, size);
            fprintf(outfile, "%s:\n", name);
            U_pairList init_iter = frag->u.global.init_values;
            for (; init_iter; init_iter = init_iter->tail){
                U_intPair init_pair = init_iter->head;
                if (init_pair->x > 1){
                    fprintf(outfile, "\t.space  %d\n", init_pair->x * word_size);
                } else{
                    fprintf(outfile, "\t.word   %d\n", init_pair->y);
                }
            }
//            fprintf(outfile, "\n");
        } else if (frag->kind == F_frag_::F_stringFrag){
            fprintf(outfile, "\t.section\t.rodata\n");
            fprintf(outfile, "%s:\n", S_getName(frag->u.stringg.label));
            fprintf(outfile, "\t.ascii  \"%s\\000\"\n", frag->u.stringg.str);
//            fprintf(outfile, "\n");
        }
    }
//    fprintf(outfile, "\n");
}

extern FILE *yyin;
extern FILE *yyout;
extern char *yytext;

extern int yyparse();
bool OptionalLeveL2 = false; /// -O2选项的全局flag



int main(int argc, char **argv) {

    F_fragList frags;
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
                    OptionalLeveL2 = true;
                } else {
                    OptionalLeveL2 = false;
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
    if(yyin == NULL) {
        printf("Open file(%s) failed\n", INPUT_FILE);
        return -1;
    }

    yyout = stdout;
    FILE_NAME = INPUT_FILE; // todo refactor?


    S_table tenv = E_base_typeEntry();
    S_table venv = E_base_valueEntry(tenv);
    yyparse();

//    fprintf(stderr, "\nbefore semantic ast:\n");
//    pr_decList(stderr, absyn_root, 0);
//    fprintf(stderr, "\n");

    frags = SEM_transProgram(venv, tenv, absyn_root);
//    printStmList(stderr, frags);
//    fprintf(stderr, "\nafter semantic ast:\n");
//    pr_decList(stderr, absyn_root, 0);
//    fprintf(stderr, "\nsemantic check finish !\n");

//    printStmList(stderr, frags);

    //输出汇编指令的路径,应更改为文件名
    FILE *outfile=fopen(OUTPUT_FILE, "w");
    if(outfile == NULL) {
        printf("open file(%s) failed", OUTPUT_FILE);
        return -1;
    }

//    FILE *outfile = stdout;
    doGlobal(outfile, frags);
    fprintf(outfile, "\t.text\n");
    for (; frags; frags = frags->tail) {
        if (frags->head->kind == F_frag_::F_procFrag) {
            doProc(outfile, frags->head->u.proc.frame, frags->head->u.proc.body);
        }
    }

    return 0;
}





