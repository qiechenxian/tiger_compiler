//
// Created by zcjsh on 2020/7/26.
//

#include "flowgraph.h"

#include "vector"

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

G_graph FG_AssemFlowGraph(AS_instrList il, F_frame f) {
    //your code here.
    G_graph g = G_Graph();
    G_nodeList nl = NULL, jumpnl = NULL;
    Temp_labelList ll = NULL, jl = NULL, last_lbl = NULL;
    G_node n = NULL, last_n = NULL, jump_n = NULL;
    AS_instr inst = NULL, last_inst = NULL, last_nonlbl_inst = NULL;
    vector<Temp_label> labelList;
    bool needStep = true;

    // Iterate and add instructions to graph
    for (;il;) {
        inst = il->head;

        needStep = true;

        if (inst->kind != AS_instr_::I_LABEL) {

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
                if (last_inst->kind == AS_instr_::I_LABEL) {

                    if(label_added) {
                        nl = G_NodeList(n, nl);
                        ll = Temp_LabelList(last_inst->u.LABEL.label, ll);
                    }

                    if (last_nonlbl_inst) {
                        G_addEdge(last_n, n);
                    }
                } else if (last_inst->kind == AS_instr_::I_OPER && last_inst->u.OPER.jumps != NULL) {
                    // add edge for conditional jumps
                    if (strstr(last_inst->u.OPER.assem, "b ") == 0 &&  strstr(last_inst->u.OPER.assem, "bl ") == 0) {
                        G_addEdge(last_n, n);
                    }
                } else {
                    G_addEdge(last_n, n);
                }
            }

            if (inst->kind == AS_instr_::I_OPER && inst->u.OPER.jumps != NULL) {
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

                if(newil->head->kind != AS_instr_::I_LABEL) {
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


