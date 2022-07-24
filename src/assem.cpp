//
// Created by zcjsh on 2020/7/21.
//
#include "assem.h"
#include "frame.h"
#include <string>

using namespace std;

// 构造函数
AS_targets AS_Targets(Temp_labelList labels) {
    AS_targets p = (AS_targets) checked_malloc(sizeof(*p));
    p->labels = labels;
    return p;
}


AS_instr AS_Oper(c_string a, Temp_tempList d, Temp_tempList s, AS_targets j,bool tag, int op_kind) {
    AS_instr p = (AS_instr) checked_malloc(sizeof(*p));
    p->kind = AS_instr_::I_OPER;
    if(strstr(a, "add") !=NULL)
    {
        p->kindl=AS_instr_::O_add;
    }
    else if(strstr(a, "sub") !=NULL)
    {
        p->kindl=AS_instr_::O_sub;
    }
    else if(strstr(a, "str") !=NULL)
    {
        p->kindl=AS_instr_::O_str;
    } else if(strstr(a, "ldr") !=NULL)
    {
        p->kindl=AS_instr_::O_ldr;
    } else if(strstr(a, "lsl") !=NULL)
    {
        p->kindl=AS_instr_::O_lsl;
    }
    else if(strstr(a, "__aeabi_idivmod") !=NULL)
    {
        p->kindl=AS_instr_::O_mod;
    }
    else if(strstr(a, "__aeabi_idiv") !=NULL)
    {
        p->kindl=AS_instr_::O_div;
    }
    else if(strstr(a, "mov") !=NULL)
    {
        p->kindl=AS_instr_::O_mov;
    }
    else
    {
        p->kindl=AS_instr_::O_others;
    }
    if(p->kindl==AS_instr_::O_div||p->kindl==AS_instr_::O_mod)
    {
        Temp_tempList list=d;
        list=list->tail;
        Temp_temp save_dst=list->tail->head;
        list->tail=NULL;
        p->u.OPER.div_src=d;
        p->u.OPER.div_dst=Temp_TempList(save_dst,NULL);
        p->u.OPER.dst = s;
        p->u.OPER.src = s;
    }
    else
    {
        p->u.OPER.dst = d;
        p->u.OPER.src = s;
    }
    p->u.OPER.info_tag=tag;
    p->u.OPER.assem = a;
    p->u.OPER.jumps = j;
    p->isDead = false;
    p->op_kind = op_kind;
    return p;
}

AS_instr AS_Label(c_string a,Temp_label label) {
    AS_instr p = (AS_instr) checked_malloc(sizeof *p);
    p->kindl=AS_instr_::O_others;
    p->kind = AS_instr_::I_LABEL;
    p->u.LABEL.assem = a;
    p->u.LABEL.label = label;
    p->isDead = false;
    p->op_kind = AS_instr_::OP_NG;
    return p;
}

AS_instr AS_Move(c_string a, Temp_tempList d, Temp_tempList s,bool tag) {

    AS_instr p = (AS_instr) checked_malloc(sizeof *p);
    p->kindl=AS_instr_::O_mov;
    p->u.OPER.info_tag=tag;
    p->kind = AS_instr_::I_MOVE;
    p->u.MOVE.assem = a;
    p->u.MOVE.dst = d;
    p->u.MOVE.src = s;
    p->isDead = false;
    p->op_kind = AS_instr_::OP_NG;
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
    if (a == NULL) return b;
    AS_instrList p;
    for (p = a; p->tail != NULL; p = p->tail);
    p->tail = b;
    return a;
}

AS_instrList AS_instrUnion(AS_instrList ta, AS_instrList tb) {
    AS_instr t;
    AS_instrList tl = NULL;
    TAB_table m = TAB_empty();

    for (; ta; ta = ta->tail) {
        t = ta->head;
        if (TAB_look(m, t) == NULL) {
            TAB_enter(m, t, (void *) "u");
            tl = AS_InstrList(t, tl);
        }
    }

    for (; tb; tb = tb->tail) {
        t = tb->head;
        if (TAB_look(m, t) == NULL) {
            TAB_enter(m, t, (void *) "u");
            tl = AS_InstrList(t, tl);
        }
    }
    free(m);

    return tl;
}

AS_instrList AS_instrMinus(AS_instrList ta, AS_instrList tb) {
    AS_instr t;
    AS_instrList tl = NULL;
    TAB_table m = TAB_empty();

    for (; tb; tb = tb->tail) {
        t = tb->head;
        TAB_enter(m, t, (void *) "m");
    }

    for (; ta; ta = ta->tail) {
        t = ta->head;
        if (TAB_look(m, t) == NULL) {
            tl = AS_InstrList(t, tl);
        }
    }
    free(m);
    return tl;
}

AS_instrList AS_instrIntersect(AS_instrList ta, AS_instrList tb) {
    AS_instr t;
    AS_instrList tl = NULL;
    TAB_table m = TAB_empty();

    for (; ta; ta = ta->tail) {
        t = ta->head;
        TAB_enter(m, t, (void *) "i");
    }

    for (; tb; tb = tb->tail) {
        t = tb->head;
        if (TAB_look(m, t) != NULL) {
            tl = AS_InstrList(t, tl);
        }
    }
    free(m);
    return tl;
}

// 返回第n个temp和返回第n个label
static Temp_temp nthTemp(Temp_tempList list, int i) {
    EM_ASSERT_CODE=-50;
    assert(list);
    if (i == 0) return list->head;
    else return nthTemp(list->tail, i - 1);
}

static Temp_label nthLabel(Temp_labelList list, int i) {
    EM_ASSERT_CODE=-51;
    assert(list);
    if (i == 0) return list->head;
    else return nthLabel(list->tail, i - 1);
}

/*
 * 格式化assem字符串，参数分别是dst，src，jumps
 * 最后一个参数用来确定临时变量需要如何处理
 */
static void format(char *result, c_string assem, Temp_tempList dst, Temp_tempList src, AS_targets jumps, Temp_map m, bool isMove) {
//    fprintf(stderr, "a format: assem=%s, dst=%p, src=%p\n", assem, dst, src);

    // Move操作，但两者相同，则忽略
    if(isMove && (src != NULL) && (dst != NULL)) {
        c_string s = Temp_look(m, src->head);
        c_string d = Temp_look(m, dst->head);

        // 同样的操作字符串，则忽略
        if(strcmp(s, d) == 0) {
            // 设置为空串
            result[0] = '\0';
            return;
        }
    }

    char *p;
    int i = 0;
    for (p = assem; p && *p != '\0'; p++) {
        if (*p == '\'') {
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
                    EM_ASSERT_CODE=-52;
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
                    EM_ASSERT_CODE=-53;
                    assert(0);
            }
        } else {
            result[i] = *p;
            i++;
        }
    }
    result[i] = '\0';
//    fprintf(stderr, "result=%s\n", result);
}

void AS_string(AS_instr i, Temp_map m, char *result) {

    // 设置为空船
    result[0] = '\0';

    switch (i->kind) {
        case AS_instr_::I_OPER:
            format(result, i->u.OPER.assem, i->u.OPER.dst, i->u.OPER.src, i->u.OPER.jumps, m, false);
            break;
        case AS_instr_::I_LABEL:
            format(result, i->u.LABEL.assem, NULL, NULL, NULL, m, false);
            break;
        case AS_instr_::I_MOVE:
            if(NULL == i->u.MOVE.src) {
                format(result, i->u.MOVE.assem, i->u.MOVE.dst, i->u.MOVE.src, NULL, m, true);
            } else if(i->u.MOVE.src->head != i->u.MOVE.dst->head){
                format(result, i->u.MOVE.assem, i->u.MOVE.dst, i->u.MOVE.src, NULL, m, true);
            }
            break;
        default:
//            EM_error(0, "not find As_instr kind");
            EM_errorWithExitCode(-4, 0, "not find As_instr kind");
    }
}

// 对如下的指令进行窥孔优化
// 	str     R0, [FP, #-152]
//	ldr     R0, [FP, #-152]
// 消除ldr指令
bool checkStrLtrHole(string lastInst, string currentInst)
{
    bool strExist = false, ldrExist = false;

    // 空指令忽略
    if(lastInst.empty() || currentInst.empty()) return false;

    // 不是str指令
    if(string::npos != lastInst.find("str")) {
        strExist = true;
    }

    // 不是ldr指令
    if(string::npos != currentInst.find("ldr"))  {
        ldrExist = true;
    }

    if(strExist && ldrExist) {
        if(lastInst.substr(9) == currentInst.substr(9)) return true;
        return false;
    }

    return false;
}

//着色算法需要
void AS_printInstrList(FILE *out, AS_instrList iList, Temp_map m) {
    char result[200] = {0};

    // 上次的指令
    string lastInstStr, currentInstStr;

    for (; iList; iList = iList->tail) {

        // 死代码则跳过
        if(iList->head->isDead)  continue;

        // 形成指令字符串
        AS_string(iList->head, m, result);
        if(result[0] == '\0') continue;

        currentInstStr = result;

        // NOP指令忽略
        if(string::npos != currentInstStr.find("NOP")) continue;

        if(checkStrLtrHole(lastInstStr, currentInstStr)) {
            result[0] = '\0';
            // 忽略ltr指令，继续下一个指令
            continue;
        }

        lastInstStr = result;

        if(result[0] != '\0') {
            fprintf(out, "%s", result);
        }
    }
//    fprintf(out, "\n");
}

AS_proc AS_Proc(c_string p, AS_instrList b, c_string e) {
    AS_proc proc = (AS_proc) checked_malloc(sizeof(*proc));
    proc->prolog = p;
    proc->body = b;
    proc->epilog = e;
    return proc;
}
bool AS_instrInList(AS_instr i, AS_instrList il) {
    for (; il; il = il->tail) {
        if (il->head == i) {
            return true;
        }
    }
    return false;
}






