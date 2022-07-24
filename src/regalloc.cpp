//
// Created by zcjsh on 2020/7/28.
//
#include "regalloc.h"
#include "temp.h"

#include <vector>

using namespace std;

static void printTemp(void *t) {
    Temp_map m = Temp_name();
    fprintf(stderr, "node: %s\n", Temp_look(m, (Temp_temp) t));
}

//打印指令
static void printInst(void *info) {

    AS_instr inst = (AS_instr) info;

    char result[200] = {0};

    if(!inst->isDead) {
        AS_string(inst, Temp_name(), result);

        if(result[0] != '\0') {
            fprintf(stdout, "%s", result);
        }
    }
}

//翻转instr list
static AS_instrList reverseInstrList(AS_instrList il) {
    AS_instrList rl = NULL;
    for (; il; il = il->tail) {
        rl = AS_InstrList(il->head, rl);
    }
    return rl;
}

//返回 dst
static Temp_tempList inst_def(AS_instr inst) {
    switch (inst->kind) {
        case AS_instr_::I_OPER:
            return inst->u.OPER.dst;
        case AS_instr_::I_LABEL:
            return NULL;
        case AS_instr_::I_MOVE:
            return inst->u.MOVE.dst;
    }
}

//返回 src
static Temp_tempList inst_use(AS_instr inst) {
    switch (inst->kind) {
        case AS_instr_::I_OPER:
            return inst->u.OPER.src;
        case AS_instr_::I_LABEL:
            return NULL;
        case AS_instr_::I_MOVE:
            return inst->u.MOVE.src;
    }
    return NULL;
}

//temp to node
static G_node temp2Node(Temp_temp t, G_graph g) {
    if (t == NULL) return NULL;
    G_nodeList nodes = G_nodes(g);
    G_nodeList p;
    for (p = nodes; p != NULL; p = p->tail)
        if (Live_gtemp(p->head) == t) return p->head;
    return NULL;
}

// node to temp
static Temp_temp node2Temp(G_node n) {
    if (n == NULL) return NULL;
    return Live_gtemp(n);
}

// temp equal ?
static bool tempEqual(Temp_tempList ta, Temp_tempList tb) {
    return Temp_equal(ta, tb);
}

// temp list ta minus tb
static Temp_tempList tempMinus(Temp_tempList ta, Temp_tempList tb) {
    return Temp_minus(ta, tb);
}

// temp list ta Union tb
static Temp_tempList tempUnion(Temp_tempList ta, Temp_tempList tb) {
    return Temp_union(ta, tb);
}

// temp list ta intersect tb
static Temp_tempList tempIntersect(Temp_tempList ta, Temp_tempList tb) {
    return Temp_intersect(ta, tb);
}

//temp in templist?
static bool tempIn(Temp_temp t, Temp_tempList tl) {
    return Temp_inList(t, tl);
}

//temp list(h,t)
static Temp_tempList L(Temp_temp h, Temp_tempList t) {
    return Temp_TempList(h, t);
}

// instr in instr list?
static bool instIn(AS_instr i, AS_instrList il) {
    return AS_instrInList(i, il);
}

//已合并的传送指令集合 get alias
static G_node getAlias(G_node n, G_table aliases, Temp_tempList coalescedNodes) {
    Temp_temp t = node2Temp(n);
    if (tempIn(t, coalescedNodes)) {
        G_node alias = (G_node) G_look(aliases, n);
        return getAlias(alias, aliases, coalescedNodes);
    } else {
        return n;
    }
}

static Temp_tempList aliased(Temp_tempList tl, G_graph ig,
                             G_table aliases, Temp_tempList cn) {
    Temp_tempList al = NULL;
    for (; tl; tl = tl->tail) {
        Temp_temp t = tl->head;
        G_node n = temp2Node(t, ig);
        if(n != nullptr) {
            G_node alias = getAlias(n, aliases, cn);
            Temp_temp alias_temp = node2Temp(alias);
            al = L(alias_temp, al);
        }
        al = L(t, al);
    }

    return tempUnion(al, NULL);
}

struct RA_result RA_regAlloc(F_frame f, AS_instrList il) {
//your code here.
    struct RA_result ret;
    bool ra_finished = false;

    Temp_map precored_map = F_initialRegisters(f);
    struct COL_result col;
    AS_instrList rewriteList;
    int tryNum = 0;
    const int maxTryNum = 10;//遍历一遍

    // 可着色的寄存器一览
    Temp_tempList canColoredRegs = F_registers();
    Temp_tempList initial = nullptr;
    bool spilled_exist = false;
    COL_init_spilled_temp_num(-1);

    while (tryNum ++ < maxTryNum) {

#ifdef DEBUG_PRINT_COLOR
        printf("Color:%s:%d start\n", Temp_labelString(F_getName(f)), tryNum);
#endif

        // 因每次都会构建控制流图，因此initial每次都需要设置空
        initial = nullptr;

        // 构建控制流图
        G_graph flow = FG_AssemFlowGraph(il, f);

#ifdef DEBUG_PRINT_IR
        G_show(stderr, G_nodes(flow), printInst);
#endif

        // 进行变量活跃性分析
        struct Live_graph live = Live_liveness(flow);

        // 控制流图不再使用，清除空间
        G_Graph_free(flow);

#ifdef DEBUG_PRINT_GRAPH
        G_show(stderr, G_nodes(live.graph), printTemp);
#endif

        col = COL_color(live.graph, precored_map, canColoredRegs, initial,
                        live.worklistMoves, live.moveList, live.spillCost);

        if (col.spills == NULL) {
#ifdef DEBUG_PRINT_COLOR
            printf("Color:%s:%d OK\n", Temp_labelString(F_getName(f)), tryNum);
#endif
            break;
        }

        // 下面进行未着色结点的溢出处理，然后重写程序
        Temp_tempList spilled = col.spills;

        rewriteList = NULL;

        // Assign locals in memory
        Temp_tempList tl, new_spilled = NULL;
        AS_instrList inst_move;

#ifdef DEBUG_PRINT
        // 查看合并的Move指令是否包含有溢出的临时变量
        for(inst_move = col.coalescedMoves; inst_move; inst_move = inst_move->tail) {

            Temp_tempList src = inst_move->head->u.MOVE.src;
            Temp_tempList dst = inst_move->head->u.MOVE.dst;

            printf("Move %d:%d\n", dst->head->num, src->head->num);
        }
#endif

        TAB_table spilledLocal = TAB_empty();
        for (tl = spilled; tl; tl = tl->tail) {

            if(NULL != TAB_look(spilledLocal, tl->head)) {
                continue;
            }

            new_spilled = Temp_TempList(tl->head, new_spilled);

            F_access local = NULL;
            local = F_allocLocal(f, true, 1);
            TAB_enter(spilledLocal, tl->head, local);
        }

        Temp_tempList new_spilled_temps = nullptr;

        // Rewrite instructions
        for (; il; il = il->tail) {
            AS_instr inst = il->head;

            // 查看合并的Move指令是否包含有溢出的临时变量
            bool moveExist = false;
            for(inst_move = col.coalescedMoves; inst_move; inst_move = inst_move->tail) {
                if(inst == inst_move->head) {
                    moveExist = true;
                    break;
                }
            }

            if(moveExist) {
                rewriteList = AS_InstrList(inst, rewriteList);
                continue;
            }

            Temp_tempList useSpilled = tempIntersect(
                    aliased(inst_use(inst), live.graph, col.alias, col.coalescedNodes),
                    new_spilled);
            Temp_tempList defSpilled = tempIntersect(
                    aliased(inst_def(inst), live.graph, col.alias, col.coalescedNodes),
                    new_spilled);

            // Skip unspilled instructions
            // 跳过未溢出的指令
            if ((useSpilled == NULL) && (defSpilled == NULL)) {
                rewriteList = AS_InstrList(inst, rewriteList);
                continue;
            }

            Temp_tempList replace_list;
            for (tl = useSpilled; tl; tl = tl->tail) {

                // 溢出变量所在栈中的位置
                F_access local = (F_access) TAB_look(spilledLocal, tl->head);

                // 新建一个临时变量
                Temp_temp new_temp = Temp_newTemp();
                if(!spilled_exist) {
                    COL_init_spilled_temp_num(new_temp->num);
                    spilled_exist = true;
                }

                // 记录一下
                new_spilled_temps = Temp_TempList(new_temp, new_spilled_temps);

                char *buf = (char *) checked_malloc(sizeof(char) * INST_GENERAL_LEN);
                sprintf(buf, "\tldr     'd0, [FP, #%d] \n#spilled\n", F_accessOffset(local));

                rewriteList = AS_InstrList(
                        AS_Oper(String(buf), L(new_temp, NULL), NULL, NULL), rewriteList);

                //ldr是放在inst use前面,替换inst src列表
                replace_list = nullptr;

                if (inst->kind == AS_instr_::I_MOVE) {
                    replace_list = inst->u.MOVE.src;
                } else if (inst->kind == AS_instr_::I_OPER) {
                    replace_list = inst->u.OPER.src;
                }

                for (; replace_list; replace_list = replace_list->tail) {
                    if (replace_list->head == tl->head) {
                        replace_list->head = new_temp;
                    }
                }
            }

            rewriteList = AS_InstrList(inst, rewriteList);

            for (tl = defSpilled; tl; tl = tl->tail) {

                // 溢出变量所在栈中的位置
                F_access local = (F_access) TAB_look(spilledLocal, tl->head);

                // 新建一个临时变量
                Temp_temp new_temp = Temp_newTemp();
                if(!spilled_exist) {
                    COL_init_spilled_temp_num(new_temp->num);
                    spilled_exist = true;
                }

                new_spilled_temps = Temp_TempList(new_temp, new_spilled_temps);

                char *buf = (char *) checked_malloc(sizeof(char) * INST_GENERAL_LEN);
                sprintf(buf, "\tstr     's0, [FP, #%d] \n#spilled\n", F_accessOffset(local));

                rewriteList = AS_InstrList(
                        AS_Oper(String(buf), NULL, L(new_temp, NULL), NULL), rewriteList);

                //str是放在inst dfe后面,替换dst列表
                replace_list = nullptr;

                if (inst->kind == AS_instr_::I_MOVE) {
                    replace_list = inst->u.MOVE.dst;
                } else if (inst->kind == AS_instr_::I_OPER) {
                    replace_list = inst->u.OPER.dst;
                }

                for (; replace_list; replace_list = replace_list->tail) {
                    if (replace_list->head == tl->head) {
                        replace_list->head = new_temp;
                    }
                }
            }
        }

#if 0
        if (col.coalescedMoves != NULL) {
            for (il = rewriteList; il; il = il->tail) {
                AS_instr inst = il->head;

                // Remove coalesced moves
                if (instIn(inst, col.coalescedMoves)) {
                    inst->isDead = true;
                }
            }
        }
#endif

        il = reverseInstrList(rewriteList);

        G_Graph_free(live.graph);

#ifdef DEBUG_PRINT_COLOR
        printf("Color:%s:%d NG\n", Temp_labelString(F_getName(f)), tryNum);
#endif
    }

    if (col.spills != NULL) {
        EM_errorWithExitCode(-2, 0, "fail to allocate registers");
#ifdef DEBUG_PRINT_COLOR
        printf("Color:%s failed to allocate registers\n", Temp_labelString(F_getName(f)));
#endif
    }

    if (col.coalescedMoves != NULL) {
        for (rewriteList = il; rewriteList; rewriteList = rewriteList->tail) {
            AS_instr inst = rewriteList->head;

            // Remove coalesced moves
            if (instIn(inst, col.coalescedMoves)) {
                inst->isDead = true;
            }
        }
    }

    ret.coloring = col.coloring;

    ret.il = il;

    //free(col);
    //delete col;


// Temp_tempList precolored = NULL;
// Temp_tempList initial = NULL;
// Temp_tempList simplifyWorklist = NULL;
// Temp_tempList freezeWorklist = NULL;
// Temp_tempList spillWorklist = NULL;
// Temp_tempList spilledNodes = NULL;
// Temp_tempList coalescedNodes = NULL;   // Coalesce
// Temp_tempList coloredNodes = NULL;
// Temp_tempList selectStack = NULL;

// Temp_tempList worklistMoves = NULL; // Coalesce

// do {

// } while (simplifyWorklist != NULL || worklistMoves != NULL);

    return ret;
}


