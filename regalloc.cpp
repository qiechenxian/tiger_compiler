//
// Created by zcjsh on 2020/7/28.
//
#include "regalloc.h"

static void printTemp(void *t) {
    Temp_map m = Temp_name();
    fprintf(stderr, "node: %s\n", Temp_look(m, (Temp_temp) t));
}

//打印指令
static void printInst(void *info) {
    AS_instr inst = (AS_instr) info;
    AS_print(stdout, inst, Temp_name());
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
    Temp_tempList bl = NULL;
    for (; tl; tl = tl->tail) {
        Temp_temp t = tl->head;
        G_node n = temp2Node(t, ig);
        G_node alias = getAlias(n, aliases, cn);
        t = node2Temp(n);
        al = L(t, al);
        bl = L(node2Temp(alias), bl);
    }
    return tempUnion(al, bl);
}

struct RA_result RA_regAlloc(F_frame f, AS_instrList il) {
//your code here.
    struct RA_result ret;
    bool ra_finished = false;

    G_graph flow;
    struct Live_graph live;
    Temp_map initial;
    struct COL_result col;// = (COL_result*)checked_malloc(sizeof(COL_result));
    AS_instrList rewriteList;
    int tryNum = 0;
    while (++tryNum < 7) {
        flow = FG_AssemFlowGraph(il, f);

        #if 0
        G_show(stderr, G_nodes(flow), printInst);
        #endif

        live = Live_liveness(flow);

        #if 1
        G_show(stderr, G_nodes(live.graph), printTemp);
        #endif

        initial = F_initialRegisters(f);

        col = COL_color(live.graph, initial, F_registers(),
                        live.worklistMoves, live.moveList, live.spillCost);

        if (col.spills == NULL) {
            break;
        }

        Temp_tempList spilled = col.spills;
        rewriteList = NULL;

        // Assign locals in memory
        Temp_tempList tl;
        TAB_table spilledLocal = TAB_empty();
        for (tl = spilled; tl; tl = tl->tail) {
            //TODO local的大小
            F_access local = F_allocLocal(f, true, get_word_size());
            TAB_enter(spilledLocal, tl->head, local);
        }

        // Rewrite instructions
        for (; il; il = il->tail) {
            AS_instr inst = il->head;

            Temp_tempList useSpilled = tempIntersect(
                    aliased(inst_use(inst), live.graph, col.alias, col.coalescedNodes),
                    spilled);
            Temp_tempList defSpilled = tempIntersect(
                    aliased(inst_def(inst), live.graph, col.alias, col.coalescedNodes),
                    spilled);

            // Skip unspilled instructions
            // 跳过未溢出的指令
            if ((useSpilled == NULL) && (defSpilled == NULL)) {
                rewriteList = AS_InstrList(inst, rewriteList);
                continue;
            }

            for (tl = useSpilled; tl; tl = tl->tail) {
                char buf[128];

                Temp_temp temp = tl->head;
                F_access local = (F_access) TAB_look(spilledLocal, temp);

                sprintf(buf, "\tldr     'd0, ['s0, #%d] \n#spilled\n", F_accessOffset(local));

                rewriteList = AS_InstrList(
                        AS_Oper(String(buf), L(temp, NULL), L(F_FP(), NULL), NULL), rewriteList);
            }

            rewriteList = AS_InstrList(inst, rewriteList);

            for (tl = defSpilled; tl; tl = tl->tail) {
                char buf[128];

                Temp_temp temp = tl->head;
                F_access local = (F_access) TAB_look(spilledLocal, temp);

                sprintf(buf, "\tstr     's0, ['s1, #%d] \n#spilled\n", F_accessOffset(local));

                rewriteList = AS_InstrList(
                        AS_Oper(String(buf), NULL, L(temp, L(F_FP(), NULL)), NULL), rewriteList);
            }
        }

        il = reverseInstrList(rewriteList);
    }

    if (col.spills != NULL) {
        EM_error(0, "fail to allocate registers");
    }

    if (col.coalescedMoves != NULL) {
        rewriteList = NULL;
        for (; il; il = il->tail) {
            AS_instr inst = il->head;

// Remove coalesced moves
//溢出已经合并的指令
            if (instIn(inst, col.coalescedMoves)) {
                char buf[1024];
                sprintf(buf, "# ");
                strcat(buf, inst->u.OPER.assem);
                inst->u.OPER.assem = String(buf);
//continue;
            }

            rewriteList = AS_InstrList(inst, rewriteList);
        }

        il = reverseInstrList(rewriteList);
    }

    ret.coloring = col.coloring;
    //free(col);
    //delete col;
    ret.il = il;

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


