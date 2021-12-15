//
// Created by zcjsh on 2020/7/26.
//

#include "flowgraph.h"

#include <vector>
#include <string>

using namespace  std;

Temp_tempList FG_def(G_node n) {
    AS_instr inst = (AS_instr) G_nodeInfo(n);
    switch (inst->kind) {
        case AS_instr_::I_OPER:
            return inst->u.OPER.dst;
        case AS_instr_::I_LABEL:
            return NULL;
        case AS_instr_::I_MOVE:
            return inst->u.MOVE.dst;
    }
}
bool div_mod_call(G_node n) {
    AS_instr inst = (AS_instr) G_nodeInfo(n);
    return (inst->kindl==AS_instr_::O_mod||inst->kindl==AS_instr_::O_div);
}
bool imm_tag(G_node n)
{
    AS_instr inst = (AS_instr) G_nodeInfo(n);
    return inst->u.OPER.info_tag;
}

Temp_tempList FG_use(G_node n) {
    AS_instr inst = (AS_instr) G_nodeInfo(n);
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
int FG_kind(G_node n){
    AS_instr inst = (AS_instr) G_nodeInfo(n);
    return inst->kindl;
}
bool FG_tag(G_node n){
    AS_instr inst = (AS_instr) G_nodeInfo(n);
    return inst->u.OPER.info_tag;
}
Temp_tempList FG_div_use(G_node n) {
    AS_instr inst = (AS_instr) G_nodeInfo(n);
    return inst->u.OPER.div_src;
}
Temp_tempList FG_div_def(G_node n) {
    AS_instr inst = (AS_instr) G_nodeInfo(n);
    return inst->u.OPER.div_dst;
}
bool FG_isMove(G_node n) {
    AS_instr inst = (AS_instr) G_nodeInfo(n);
    return (inst->kind == AS_instr_::I_MOVE);
}

AS_instr FG_inst(G_node n) {
    return (AS_instr) G_nodeInfo(n);
}

static G_node findLabeledNode(Temp_label lab, G_nodeList nl, Temp_labelList ll) {
    G_node result = NULL;
    for (; nl && ll; nl = nl->tail, ll = ll->tail) {
        if (ll->head == lab) {
            result = nl->head;
            return result;
        }
    }
    return result;
}

static bool isJumpInstr(AS_instr inst) {
    return inst && (inst->kind == AS_instr_::I_OPER) && (inst->u.OPER.jumps != NULL);
}

static bool isJumpInstr(AS_instrList inst) {
    return inst && (inst->head->kind == AS_instr_::I_OPER) && (inst->head->u.OPER.jumps != NULL);
}

static bool isNonCondJumpInstr(AS_instr inst) {
    return isJumpInstr(inst) && (inst->u.OPER.assem[2] == ' ');
}

static bool isLabelInstr(AS_instr inst) {
    return inst && (inst->kind == AS_instr_::I_LABEL);
}

static bool isLabelInstr(AS_instrList inst) {
    return inst && (inst->head->kind == AS_instr_::I_LABEL);
}

static string notRelOperator(string op) {
    if(op == "beq") {
        return "bne";
    } else if(op == "bne") {
        return "beq";
    } else if(op == "ble") {
        return "bgt";
    } else if(op == "bgt") {
        return "ble";
    } else if(op == "bge") {
        return "blt";
    } else if(op == "blt") {
        return "bge";
    } else {
        return "Unknown";
    }
}

void HolePoopOpt(AS_instrList instList, F_frame f)
{
    AS_instrList loopInstList;
    AS_instrList secondInst, thirdInst;
    AS_instr inst;

    for (loopInstList = instList; loopInstList; loopInstList = loopInstList->tail) {

        inst = loopInstList->head;

        if(inst->isDead) {
            continue;
        }

        secondInst = loopInstList->tail;
        if(secondInst != NULL) {
            thirdInst = secondInst->tail;
        }

        // 三指令优化
        //	beq     .L10
        //	b       .L11  ===>  bneq .L11
        //  .L10:               .L10
        if(isJumpInstr(loopInstList) && isJumpInstr(secondInst) && isLabelInstr(thirdInst)) {

            string instStr = loopInstList->head->u.OPER.assem;
            string secondStr = secondInst->head->u.OPER.assem;
            if((inst->u.OPER.jumps->labels->head == thirdInst->head->u.LABEL.label)
               && (instStr[2] != ' ') && (secondStr[2] == ' ')) {
                // 有条件指令 无条件指令 Label
                string oldOp = instStr.substr(1,3);
                string newOp = notRelOperator(oldOp);

                // 更新指令
                sprintf(inst->u.OPER.assem, "\t%s     %s\n", newOp.c_str(),
                        Temp_labelString(secondInst->head->u.OPER.jumps->labels->head));
                inst->u.OPER.jumps->labels->head = secondInst->head->u.OPER.jumps->labels->head;

                secondInst->head->isDead = true;
            }
        }

        // b .L1 && .L1 则删除b .L1指令
        if(isJumpInstr(loopInstList) && isLabelInstr(secondInst)) {
            if(inst->u.OPER.jumps->labels->head == secondInst->head->u.LABEL.label) {
                inst->isDead = true;
                continue;
            }
        }
    }
}

G_graph FG_AssemFlowGraph(AS_instrList il, F_frame f) {
    //your code here.
    G_graph g = G_Graph(f);
    G_nodeList nl = NULL, jumpnl = NULL;
    Temp_labelList ll = NULL, jl = NULL, last_lbl = NULL;
    G_node n = NULL, last_n = NULL, jump_n = NULL;
    AS_instr inst = NULL, last_inst = NULL, last_nonlbl_inst = NULL;
    vector<Temp_label> labelList;
    bool needStep = true;

    HolePoopOpt(il, f);

    // Iterate and add instructions to graph
    for (;il;) {
        inst = il->head;

        if(inst->isDead) {
            il = il->tail;
            continue;
        }

        needStep = true;

        if (!isLabelInstr(inst)) {

            n = G_Node(g, (void *) inst);

            bool label_added = true;
            if(labelList.size()) {
                for(Temp_label label : labelList) {
                    nl = G_NodeList(n, nl);
                    ll = Temp_LabelList(label, ll);
                }
                label_added = false;
                labelList.clear();
            }

            if (last_inst) {
                if (isLabelInstr(last_inst)) {

                    if(label_added) {
                        nl = G_NodeList(n, nl);
                        ll = Temp_LabelList(last_inst->u.LABEL.label, ll);
                    }

                    if (last_nonlbl_inst && !isNonCondJumpInstr(last_nonlbl_inst)) {
                        G_addEdge(last_n, n);
                    }
                } else if (isJumpInstr(last_inst)) {
                    // add edge for conditional jumps
                    if (last_inst->u.OPER.assem[2] != ' ') {
                        G_addEdge(last_n, n);
                    }
                } else {
                    G_addEdge(last_n, n);
                }
            }

            if (isJumpInstr(inst)) {
                jumpnl = G_NodeList(n, jumpnl);
            }

            last_n = n;

            last_nonlbl_inst = inst;

            last_inst = inst;

        }  else  {

            last_inst = inst;

            bool multiLabel = false;
            AS_instrList newil = il;
            for (newil = newil->tail; newil; newil = newil->tail) {

                // 过滤死代码
                if(newil->head->isDead) continue;

                if(!isLabelInstr(newil)) {
                    break;
                }

                multiLabel = true;

                last_inst = newil->head;

                labelList.push_back(newil->head->u.LABEL.label);
            }

            if(multiLabel) {
                il = newil;
                labelList.push_back(inst->u.LABEL.label);
                needStep = false;
            }
        }

        if(needStep) {
            il = il->tail;
        }
    }

    // Handle jump instructions
    for (; jumpnl; jumpnl = jumpnl->tail) {
        n = jumpnl->head;
        inst = (AS_instr) G_nodeInfo(n);
        for (jl = inst->u.OPER.jumps->labels; jl; jl = jl->tail) {
            jump_n = findLabeledNode(jl->head, nl, ll);
            if (jump_n) {
                G_addEdge(n, jump_n);
            } else {
//                EM_error(0, "fail to find node for label %s", Temp_labelString(jl->head));
            }
        }
    }

    return g;
}


