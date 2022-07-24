//
// Created by zcjsh on 2020/7/28.
//
#include "liveness.h"
#include "temp.h"
#include "color.h"


Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail) {
    Live_moveList lm = (Live_moveList) checked_malloc(sizeof(*lm));
    lm->src = src;
    lm->dst = dst;
    lm->tail = tail;
    return lm;
}

Temp_temp Live_gtemp(G_node n) {
    //your code here.
    return (Temp_temp)G_nodeInfo(n);
}

static bool tempEqual(Temp_tempList ta, Temp_tempList tb) {
    return Temp_equal(ta, tb);
}

static Temp_tempList tempMinus(Temp_tempList ta, Temp_tempList tb) {
    return Temp_minus(ta, tb);
}

static Temp_tempList tempUnion(Temp_tempList ta, Temp_tempList tb) {
    return Temp_union(ta, tb);
}

static Temp_tempList tempIntersect(Temp_tempList ta, Temp_tempList tb) {
    return Temp_intersect(ta, tb);
}

static Temp_tempList L(Temp_temp h, Temp_tempList t) {
    return Temp_TempList(h, t);
}

static AS_instrList IL(AS_instr h, AS_instrList t) {
    return AS_InstrList(h, t);
}

static AS_instrList instMinus(AS_instrList ta, AS_instrList tb) {
    return AS_instrMinus(ta, tb);
}

static AS_instrList instUnion(AS_instrList ta, AS_instrList tb) {
    return AS_instrUnion(ta, tb);
}

static AS_instrList instIntersect(AS_instrList ta, AS_instrList tb) {
    return AS_instrIntersect(ta, tb);
}

static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps) {
    G_enter(t, flowNode, temps);
}

static Temp_tempList lookupLiveMap(G_table t, G_node flownode) {
    return (Temp_tempList)G_look(t, flownode);
}

static void getLiveMap(G_graph flow, G_table in, G_table out) {
    G_nodeList fl, sl;
    G_node n, sn;
    G_table last_in = G_empty();
    G_table last_out = G_empty();
    Temp_tempList ci, co, li, lo;
    bool flag = true;

    // Loop
    while (flag) {

        for (fl =G_reverseNodes(G_nodes(flow)) ; fl; fl = fl->tail) {//fl用于遍历flow结点
            n = fl->head;

            // 节点对应的指令
            AS_instr inst = FG_inst(n);

            // 忽略.LTORG指令
            if(inst->kind == AS_instr_::I_OPER && (0 == strcmp(inst->u.OPER.assem, "\t.LTORG\n"))) {
                continue;
            }

            li = lookupLiveMap(in, n);
            lo = lookupLiveMap(out, n);
            enterLiveMap(last_in, n, li);
            enterLiveMap(last_out, n, lo);

            ci = tempUnion(FG_use(n), tempMinus(lo, FG_def(n)));//返回ci=FG_use(n)+out(n)-FG_def(n)
            //入口活跃=use(n)+out(n)-def(n)
            co = NULL;
            for (sl = G_succ(n); sl; sl = sl->tail) {//G_succ(n)取n的后继结点表（控制流里的）
                sn = sl->head;
                co = tempUnion(co, lookupLiveMap(in, sn));//返回co=c0+lookup(in)
                //出口活跃=后继节点的入口活跃之和
            }
            enterLiveMap(in, n, ci);//in中放入节点n，值为ci为当前入口活跃temp
            enterLiveMap(out, n, co);//out中放入节点n，值为co为当前出口活跃temp
        }

        flag = false;
        for (fl = G_nodes(flow); fl; fl = fl->tail) {
            n = fl->head;
            li = lookupLiveMap(in, n);
            lo = lookupLiveMap(out, n);
            ci = lookupLiveMap(last_in, n);
            co = lookupLiveMap(last_out, n);

            if (!tempEqual(li, ci) || !tempEqual(lo, co)) {
                flag = true;
                break;
            }
        }//直到到达不动点前都要继续执行
    }

#ifdef DEBUG_PRINT_LIVENESS
    for (fl = G_nodes(flow); fl; fl = fl->tail) {
        n = fl->head;
        li = lookupLiveMap(in, n);
        lo = lookupLiveMap(out, n);

        printf("In:%d--", n->mykey);
        for(; li; li = li->tail) {
            printf(":%d", li->head->num);
        }
        printf("\n");

        printf("Out:%d--", n->mykey);
        for(; lo; lo = lo->tail) {
            printf(":%d", lo->head->num);
        }
        printf("\n");
    }
#endif

    free(last_in);
    free(last_out);
}

static G_node findOrCreateNode(Temp_temp t, G_graph g, TAB_table tab) {
    G_node ln = (G_node)TAB_look(tab, t);
    if (ln == NULL) {
        ln = G_Node(g, t);
        TAB_enter(tab, t, ln);
    }
    return ln;
}

static void solveLiveness(struct Live_graph *lg,
                          G_graph flow, G_table in, G_table out) {
    F_frame frame = G_frame(flow);
    G_graph g = G_Graph(frame);
    TAB_table tab = TAB_empty();
    Live_moveList ml = NULL;
    G_nodeList fl;
    G_node n, ndef, nedge, move_src, move_dst;
    Temp_tempList tdef, tuse, t, tedge;
    Temp_tempList live;

    Temp_map moveList = Temp_empty();
    Temp_map spillCost = Temp_empty();
    AS_instr inst;
    AS_instrList worklistMoves = NULL;

#if 0
    // 由于函数默认传参数R0-R3为寄存器，所以要创建结点
    int arg_cnt = F_getFormalsCount(frame);
    Temp_temp * callerArray = F_getCallerArray();
    for(int k = 0; k < arg_cnt && k < 4; k++) {
        findOrCreateNode(callerArray[k], g, tab);
    }
#endif

    // Traverse node
    for (fl = G_nodes(flow); fl; fl = fl->tail) {
        n = fl->head;

        // 节点对应的指令
        inst = FG_inst(n);

        // 忽略.LTORG指令
        if(inst->kind == AS_instr_::I_OPER && (0 == strcmp(inst->u.OPER.assem, ".LTORG\n"))) {
            continue;
        }

        // 查询节点对应的出口活跃点
        live = lookupLiveMap(out, n);

        // 获取该指令对应的源操作数和目的操作数
        tdef = FG_def(n);
        tuse = FG_use(n);

        // 结果没有被使用，说明该指令需要删除
        if(tdef != NULL && (!Temp_inList(tdef->head, live))) {
            if (tdef->head->num >= 100) {
                inst->isDead = true;
                continue;
            }
        }

        // 使用与定值合并
        Temp_tempList defuse = tempUnion(tuse, tdef);

        // Move instruction?
        if (FG_isMove(n)) {

            live = tempMinus(live, tuse);

            for (t = defuse; t; t = t->tail) {
                Temp_temp ti = t->head;

                findOrCreateNode(ti, g, tab);

                AS_instrList ml = (AS_instrList)Temp_lookPtr(moveList, ti);
                ml = instUnion(ml, IL(inst, NULL));
                Temp_enterPtr(moveList, ti, (void*)ml);
            }

            worklistMoves = instUnion(worklistMoves, IL(inst, NULL));
        }

        // Spill Cost
        for (t = defuse; t; t = t->tail) {
            Temp_temp ti = t->head;

            // 计算变量溢出的代价
            long long spills = (long long)Temp_lookPtr(spillCost, ti);
            ++spills;

            Temp_enterPtr(spillCost, ti, (void*)spills);
        }

        live = tempUnion(live, tdef);

        // Traverse defined vars
        for (t = tdef; t; t = t->tail) {

            ndef = findOrCreateNode(t->head, g, tab);

#ifdef DEBUG_PRINT_LIVENESS
            printf("Var:%d--", t->head->num);
#endif

            // Add edges between output vars and defined var
            for (tedge = live; tedge; tedge = tedge->tail) {

                nedge = findOrCreateNode(tedge->head, g, tab);

#ifdef DEBUG_PRINT_LIVENESS
                printf(":%d", ((Temp_temp)nedge->info)->num);
#endif

                // Skip if edge is added
                if (ndef == nedge || G_goesTo(nedge, ndef)) {
                    continue;
                }

                G_addEdge(nedge, ndef);
                // G_addEdge(ndef, nedge);

            }
#ifdef DEBUG_PRINT_LIVENESS
            printf("\n");
#endif
        }
    }

    lg->graph = g;
    lg->worklistMoves = worklistMoves;
    lg->moveList = moveList;
    lg->spillCost = spillCost;
}

static void solveLiveness3(struct Live_graph *lg,
                           G_graph flow, G_table in, G_table out) {
    G_graph g = G_Graph(G_frame(flow));
    TAB_table tab = TAB_empty();
    Temp_map moveList = Temp_empty();
    Temp_map spillCost = Temp_empty();
    AS_instrList worklistMoves = NULL;
    G_nodeList fl;
    G_node n, ndef, nedge, move_src, move_dst;
    Temp_tempList tdef, tuse, tout, tl, tedge, live = NULL;
    AS_instr inst;
    bool blockStart = true;

    // Traverse node
    fl = G_reverseNodes(G_nodes(flow));
    //if (fl) live = lookupLiveMap(out, fl->head);
    for (; fl; fl = fl->tail) {
        if (blockStart) {
            live = lookupLiveMap(out, fl->head);
            blockStart = false;
        }

        n = fl->head;
        inst = FG_inst(n);
        tuse = FG_use(n);
        tdef = FG_def(n);

        if (inst->kind == AS_instr_::I_LABEL) {
            blockStart = true;
            continue;
        }

        Temp_tempList defuse = tempUnion(tuse, tdef);

        // Spill Cost
        for (tl = defuse; tl; tl = tl->tail) {
            Temp_temp ti = tl->head;
            long long spills = (long long)Temp_lookPtr(spillCost, ti);
            ++spills;
            Temp_enterPtr(spillCost, ti, (void*)spills);
        }

        // Move instruction?
        if (FG_isMove(n)) {
            live = tempMinus(live, tuse);

            for (; defuse; defuse = defuse->tail) {
                Temp_temp t = defuse->head;
                AS_instrList ml = (AS_instrList)Temp_lookPtr(moveList, t);
                ml = instUnion(ml, IL(inst, NULL));
                Temp_enterPtr(moveList, t, (void*)ml);
            }

            worklistMoves = instUnion(worklistMoves, IL(inst, NULL));
        }

        live = tempUnion(live, tdef);

        // Traverse defined vars
        for (tl = tdef; tl; tl = tl->tail) {
            ndef = findOrCreateNode(tl->head, g, tab);

            // Add edges between output vars and defined var
            for (tedge = live; tedge; tedge = tedge->tail) {
                nedge = findOrCreateNode(tedge->head, g, tab);

                // Skip if edge is added
                if (ndef == nedge || G_goesTo(ndef, nedge) || G_goesTo(nedge, ndef)) {
                    continue;
                }

                G_addEdge(ndef, nedge);
            }
        }

        live = tempUnion(tuse, tempMinus(live, tdef));
    }

    lg->graph = g;
    lg->worklistMoves = worklistMoves;
    lg->moveList = moveList;
    lg->spillCost = spillCost;
}

struct Live_graph Live_liveness(G_graph flow) {//生成冲突图和节点偶对表
    //your code here.

    // Construct liveness graph
    G_table in = G_empty(), out = G_empty();
    getLiveMap(flow, in, out);//得到各个节点的出入口活跃信息

    // Construct interference graph
    struct Live_graph lg;
    solveLiveness(&lg, flow, in, out);

    G_free(in);
    G_free(out);

    return lg;
}
