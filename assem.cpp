//
// Created by zcjsh on 2020/7/21.
//
#include "assem.h"


// 构造函数
AS_targets AS_Targets(Temp_labelList labels) {
    AS_targets p = (AS_targets) checked_malloc(sizeof(*p));
    p->labels = labels;
    return p;
}

AS_instr AS_Oper(c_string a, Temp_tempList d, Temp_tempList s, AS_targets j) {
    AS_instr p = (AS_instr) checked_malloc(sizeof(*p));
    p->kind = AS_instr_::I_OPER;
    p->u.OPER.assem = a;
    p->u.OPER.dst = d;
    p->u.OPER.src = s;
    p->u.OPER.jumps = j;
    return p;
}

AS_instr AS_Label(c_string a, Temp_label label) {
    AS_instr p = (AS_instr) checked_malloc(sizeof *p);
    p->kind = AS_instr_::I_LABEL;
    p->u.LABEL.assem = a;
    p->u.LABEL.label = label;
    return p;
}

AS_instr AS_Move(c_string a, Temp_tempList d, Temp_tempList s) {
    AS_instr p = (AS_instr) checked_malloc(sizeof *p);
    p->kind = AS_instr_::I_MOVE;
    p->u.MOVE.assem = a;
    p->u.MOVE.dst = d;
    p->u.MOVE.src = s;
    return p;
}

AS_instrList AS_InstrList(AS_instr head, AS_instrList tail) {
    AS_instrList p = (AS_instrList) checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

//把list b 拼接到 list a后边
AS_instrList AS_splice(AS_instrList a, AS_instrList b) {
    if (a = NULL) return b;
    AS_instrList p;
    for (p = a; p->tail != NULL; p = p->tail);
    p->tail = b;
    return a;
}

// 返回第n个temp和返回第n个label
static Temp_temp nthTemp(Temp_tempList list, int i) {
    assert(list);
    if (i == 0) return list->head;
    else return nthTemp(list->tail, i - 1);
}

static Temp_label nthLabel(Temp_labelList list, int i) {
    assert(list);
    if (i == 0) return list->head;
    else return nthLabel(list->tail, i - 1);
}

/*
 * 格式化assem字符串，参数分别是dst，src，jumps
 * 最后一个参数用来确定临时变量需要如何处理
 */
static void format(char *result, c_string assem, Temp_tempList dst, Temp_tempList src, AS_targets jumps, Temp_map m) {
    fprintf(stdout, "a format: assem=%s, dst=%p, src=%p\n", assem, dst, src);
    char *p;
    int i = 0;
    for (p = assem; p && *p != '\0'; p++) {
        if (*p == '`') {
            switch (*(++p)) {
                case 's': {
                    int n = atoi(++p);
                    c_string s = Temp_look(m, nthTemp(src, n));
                    strcpy(result + i, s);
                    i += strlen(s);
                }
                    break;
                case 'd': {
                    int n = atoi(++p);
                    c_string s = Temp_look(m, nthTemp(dst, n));
                    strcpy(result + i, s);
                    i += strlen(s);
                }
                    break;
                case 'j':
                    assert(jumps);
                    {
                        int n = atoi(++p);
                        c_string s = Temp_labelString(nthLabel(jumps->labels, n));
                        strcpy(result + i, s);
                        i += strlen(s);
                    }
                    break;
                case '`':
                    result[i] = '`';
                    i++;
                    break;
                default:
                    assert(0);
            }
        } else {
            result[i] = *p;
            i++;
        }
    }
    result[i] = '\0';
    fprintf(stdout, "result=%s\n", result);
}

void AS_print(FILE *out, AS_instr i, Temp_map m) {
    char result[200];
    switch (i->kind) {
        case AS_instr_::I_OPER:
            format(result, i->u.OPER.assem, i->u.OPER.dst, i->u.OPER.src, i->u.OPER.jumps, m);
            fprintf(out, "%s", result);
            break;
        case AS_instr_::I_LABEL:
            format(result, i->u.LABEL.assem, NULL, NULL, NULL, m);
            fprintf(out, "%s", result);
            break;
        case AS_instr_::I_MOVE:
            format(result, i->u.MOVE.assem, i->u.MOVE.dst, i->u.MOVE.src, NULL, m);
            fprintf(out, "%s", result);
            break;
        default:
            EM_error(0, "not find As_instr kind");
            assert(0);
    }
}


//着色算法需要
void AS_printInstrList(FILE *out, AS_instrList iList, Temp_map m) {
    for (; iList; iList = iList->tail) {
        AS_print(out, iList->head, m);
    }
    fprintf(out, "\n");
}

AS_proc AS_Proc(c_string p, AS_instrList b, c_string e) {
    AS_proc proc = (AS_proc)checked_malloc(sizeof(*proc));
    proc->prolog = p;
    proc->body = b;
    proc->epilog = e;
    return proc;
}






