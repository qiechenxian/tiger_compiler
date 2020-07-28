//
// Created by zcjsh on 2020/7/28.
//

typedef struct ctx COL_ctx;

#include "color.h"

struct ctx {
    G_graph nodes;//图
    Temp_map precolored;//机器寄存器集合，每个都事先分配颜色
    Temp_tempList initial;//临时寄存器集合，既没有预着色也没有处理
    Temp_tempList spillWorklist;//高度数的节点表
    Temp_tempList freezeWorklist;//低度数的传送有关的节点表
    Temp_tempList simplifyWorklist;//低度数的传送无关的节点表
    Temp_tempList spilledNodes;//在本轮中要被溢出的节点集合
    Temp_tempList coalescedNodes;//已合并的寄存器集合
    Temp_tempList coloredNodes;//已成功着色的节点集合
    Temp_tempList selectStack;//一个包含从图中删除的临时变量的栈

    AS_instrList coalescedMoves;//已经合并的传递指令集合
    AS_instrList constrainedMoves;//源操作数和目标操作数冲突的传送指令集合
    AS_instrList frozenMoves;//不再考虑和并的传送指令集合
    AS_instrList worklistMoves;//有可能合并的传送指令集合
    AS_instrList activeMoves;//还未作何合并准备的传送指令集合

    Temp_map spillCost;//
    Temp_map moveList;//从一个节点到与该节点相关的传送指令表映射
    G_table alias;//当传送指令(u,v)被合并，并且v放入已合并节点集合，则alias(v)=u
    G_table degree;//每个节点当前度数的数组

    int K;//
};

static COL_ctx c;


/*
 * FG_def  指令的dst
 */
static Temp_tempList instDef(AS_instr inst) {
    switch (inst->kind) {
        case AS_instr_::I_OPER:
            return inst->u.OPER.dst;
        case AS_instr_::I_LABEL:
            return NULL;
        case AS_instr_::I_MOVE:
            return inst->u.MOVE.dst;
    }
}

/*
 * FG_use 指令的src
 */
static Temp_tempList instUse(AS_instr inst) {
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

//复制寄存器表
static Temp_tempList cloneRegs(Temp_tempList regs) {
    Temp_tempList tl = NULL;
    for (; regs; regs = regs->tail) {
        tl = Temp_TempList(regs->head, tl);
    }
    return tl;
}

static Temp_temp tempHead(Temp_tempList temps) {
    if (temps == NULL) return NULL;
    return temps->head;
}

static G_node temp2Node(Temp_temp t) {
    if (t == NULL) return NULL;
    G_nodeList nodes = G_nodes(c.nodes);
    G_nodeList p;
    for(p=nodes; p!=NULL; p=p->tail)
        if (Live_gtemp(p->head)==t) return p->head;
    return NULL;
}
