//
// Created by zcjsh on 2020/7/28.
//

typedef struct ctx COL_ctx;

#include "color.h"

//机器的所有数据结构，
struct ctx {
    G_graph nodes;//冲突图
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

    int K;//颜色数目，可用的寄存器个数
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
    return NULL;
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

//temp列表头
static Temp_temp tempHead(Temp_tempList temps) {
    if (temps == NULL) return NULL;
    return temps->head;
}

//temp to node
static G_node temp2Node(Temp_temp t) {//
    if (t == NULL) return NULL;
    G_nodeList nodes = G_nodes(c.nodes);
    G_nodeList p;
    for (p = nodes; p != NULL; p = p->tail)//遍历冲突图节点
        if (Live_gtemp(p->head) == t) return p->head;//返回冲突图中的temp节点
    return NULL;
}

//temp list to node list
static G_nodeList tempL2NodeL(Temp_tempList tl) {
    G_nodeList nl = NULL;
    for (; tl; tl = tl->tail) {
        nl = G_NodeList(temp2Node(tl->head), nl);
    }
    //翻转
    return G_reverseNodes(nl);
}

//node to temp
static Temp_temp node2Temp(G_node n) {
    if (n == NULL) return NULL;
    return Live_gtemp(n);
}

//node list to temp list
static Temp_tempList nodeL2TempL(G_nodeList nl) {
    Temp_tempList tl = NULL;
    for (; nl; nl = nl->tail) {
        tl = Temp_TempList(node2Temp(nl->head), tl);
    }
    return Temp_reverseList(tl);
}

//temp name
static c_string tempName(Temp_temp t) {
    return Temp_look(Temp_name(), t);
}

//string to color temp
static Temp_temp str2Color(c_string color, Temp_map regcolors, Temp_tempList regs) {
    for (; regs; regs = regs->tail) {
        c_string s = Temp_look(regcolors, regs->head);
        if (s && (strcmp(s, color) == 0)) {
            return regs->head;
        }
    }
//    EM_error(A_Pos(0,0,0,0), "register not found for given color:");
//    printf("register not found for given color: %s\n", color);
    return NULL;
}

//color temp to string
static c_string color2Str(Temp_temp reg, Temp_map regcolors) {
    c_string color = Temp_look(regcolors, reg);
    if (color == NULL) {
        printf("color not found for given register: %s\n", tempName(reg));
    }
    return color;
}

//templist length
static int tempCount(Temp_tempList t) {
    int cnt = 0;
    for (; t; t = t->tail) {
        ++cnt;
    }
    return cnt;
};

//temp list equal?
static bool tempEqual(Temp_tempList ta, Temp_tempList tb) {
    return Temp_equal(ta, tb);
}

//temp list mnius
static Temp_tempList tempMinus(Temp_tempList ta, Temp_tempList tb) {
    return Temp_minus(ta, tb);
}

//temp list Union
static Temp_tempList tempUnion(Temp_tempList ta, Temp_tempList tb) {
    return Temp_union(ta, tb);
}

//temp list intersect
static Temp_tempList tempIntersect(Temp_tempList ta, Temp_tempList tb) {
    return Temp_intersect(ta, tb);
}

//temp in temp list?
static bool tempIn(Temp_temp t, Temp_tempList tl) {
    return Temp_inList(t, tl);
}

//L(temp,templist)
static Temp_tempList L(Temp_temp h, Temp_tempList t) {
    return Temp_TempList(h, t);
}

//InstL(instr,instrlist)
static AS_instrList IL(AS_instr h, AS_instrList t) {
    return AS_InstrList(h, t);
}

//instr list minus
static AS_instrList instMinus(AS_instrList ta, AS_instrList tb) {
    return AS_instrMinus(ta, tb);
}

//instr list union
static AS_instrList instUnion(AS_instrList ta, AS_instrList tb) {
    return AS_instrUnion(ta, tb);
}

//instr list intersect
static AS_instrList instIntersect(AS_instrList ta, AS_instrList tb) {
    return AS_instrIntersect(ta, tb);
}

//instr in list?
static bool instIn(AS_instr i, AS_instrList il) {
    return AS_instrInList(i, il);
}

//冲突的temp list
static Temp_tempList adjacent(Temp_temp t) {
    G_node n = temp2Node(t);
    G_nodeList adjn = G_adj(n);
    Temp_tempList adjs = NULL;
    for (; adjn; adjn = adjn->tail) {
        adjs = L(node2Temp(n), adjs);
    }
    //adj - （从图中删除的+已经合并的）
    adjs = tempMinus(adjs, tempUnion(c.selectStack, c.coalescedNodes));
    return adjs;
}

//添加一条边
static void addEdge(G_node nu, G_node nv) {
    if (nu == nv) return;
    if (G_goesTo(nu, nv) || G_goesTo(nv, nu)) return;
    G_addEdge(nu, nv);

    Temp_temp u = node2Temp(nu);
    Temp_temp v = node2Temp(nv);

    if (Temp_look(c.precolored, u) == NULL) {
        long d = (long long) G_look(c.degree, nu);
        d += 1;
        G_enter(c.degree, nu, (void *) d);
    }

    if (Temp_look(c.precolored, v) == NULL) {
        long d = (long long) G_look(c.degree, nv);
        d += 1;
        G_enter(c.degree, nv, (void *) d);
    }
}

static AS_instrList nodeMoves(Temp_temp t) {
    AS_instrList ml = (AS_instrList) Temp_lookPtr(c.moveList, t);
    //ml 相交（还未做好合并准备的指令+有可能合并的传送指令集合）
    return instIntersect(ml, instUnion(c.activeMoves, c.worklistMoves));
}

//是否还有与temp相关的move指令？
static bool moveRelated(Temp_temp t) {
    return nodeMoves(t) != NULL;
}

//有可能合并的传送指令集合准备
static void makeWorkList() {//低度数的传送无关表，一般来说当一个变量的冲突边小于K时其放入低度数的冲突无关表，K为当前使用的寄存器个个数
    Temp_tempList tl;
    for (tl = c.initial; tl; tl = tl->tail) {//遍历预着色节点
        Temp_temp t = tl->head;
        G_node n = temp2Node(t);//在冲突图中查找预着色节点
        c.initial = tempMinus(c.initial, L(t, NULL));//预着色节点表=预着色节点表-temp t

        if (G_degree(n) >= c.K) {
            c.spillWorklist = tempUnion(c.spillWorklist, L(t, NULL));
        } else if (moveRelated(t)) {
            c.freezeWorklist = tempUnion(c.freezeWorklist, L(t, NULL));
        } else {
            c.simplifyWorklist = tempUnion(c.simplifyWorklist, L(t, NULL));
        }
//        Temp_tempList spillWorklist;//高度数的节点表
//        Temp_tempList freezeWorklist;//低度数的传送有关的节点表
//        Temp_tempList simplifyWorklist;//低度数的传送无关的节点表
    }
}

//移除冲突
static void removeAdj(G_node n) {
    G_nodeList adjs = G_succ(n);
    for (; adjs; adjs = adjs->tail) {
        G_node m = adjs->head;
        G_rmEdge(n, m);
    }

    adjs = G_pred(n);
    for (; adjs; adjs = adjs->tail) {
        G_rmEdge(adjs->head, n);
    }
}

static void enableMoves(Temp_tempList tl) {
    for (; tl; tl = tl->tail) {
        AS_instrList il = nodeMoves(tl->head);
        for (; il; il = il->tail) {
            AS_instr m = il->head;
            if (instIn(m, c.activeMoves)) {
                c.activeMoves = instMinus(c.activeMoves, IL(m, NULL));
                c.worklistMoves = instUnion(c.worklistMoves, IL(m, NULL));
            }
        }
    }
}

//
static void decrementDegree(G_node n) {
    Temp_temp t = node2Temp(n);
    long d = (long long) G_look(c.degree, n);
    d -= 1;
    G_enter(c.degree, n, (void *) d);

    if (d == c.K) {
        enableMoves(L(t, adjacent(t)));
        c.spillWorklist = tempMinus(c.spillWorklist, L(t, NULL));
        if (moveRelated(t)) {
            c.freezeWorklist = tempUnion(c.freezeWorklist, L(t, NULL));
        } else {
            c.simplifyWorklist = tempUnion(c.simplifyWorklist, L(t, NULL));
        }
    }
}

static void addWorkList(Temp_temp t) {
    long degree = (long long) G_look(c.degree, temp2Node(t));
    if (Temp_look(c.precolored, t) == NULL &&
        (!moveRelated(t)) &&
        (degree < c.K)) {
        c.freezeWorklist = tempMinus(c.freezeWorklist, L(t, NULL));
        c.simplifyWorklist = tempUnion(c.simplifyWorklist, L(t, NULL));
    }
}

// temp t ok？
static bool OK(Temp_temp t, Temp_temp r) {
    G_node nt = temp2Node(t);
    G_node nr = temp2Node(r);
    long degree = (long long) G_look(c.degree, nt);
    if (degree < c.K) {
        return true;
    }
    if (Temp_look(c.precolored, t)) {
        return true;
    }
    if (G_goesTo(nt, nr) || G_goesTo(nr, nt)) {
        return true;
    }
    return false;
}

//保守的，briggs合并策略
static bool conservative(Temp_tempList tl) {
    G_nodeList nl = tempL2NodeL(tl);
    int k = 0;
    for (; nl; nl = nl->tail) {
        long degree = (long long) G_look(c.degree, nl->head);
        if (degree >= c.K) {
            ++k;
        }
    }
    return (k < c.K);
}

//得到别名
static G_node getAlias(G_node n) {
    Temp_temp t = node2Temp(n);
    if (tempIn(t, c.coalescedNodes)) {
        G_node alias = (G_node) G_look(c.alias, n);
        return getAlias(alias);
    } else {
        return n;
    }
}

//简化（第二步）第一步的冲突图构建由solve_liveness解决
static void simplify() {
    if (c.simplifyWorklist == NULL) {//c.simplifyWorklist低度数的传送无关的节点表
        return;
    }

    Temp_temp t = c.simplifyWorklist->head;
    G_node n = temp2Node(t);
    c.simplifyWorklist = c.simplifyWorklist->tail;

    c.selectStack = L(t, c.selectStack);  // push

    G_nodeList adjs = G_adj(n);
    for (; adjs; adjs = adjs->tail) {
        G_node m = adjs->head;
        decrementDegree(m);
    }
}

//合并temp
static void combine(Temp_temp u, Temp_temp v) {
    G_node nu = temp2Node(u);
    G_node nv = temp2Node(v);
    if (tempIn(v, c.freezeWorklist)) {
        c.freezeWorklist = tempMinus(c.freezeWorklist, L(v, NULL));
    } else {
        c.spillWorklist = tempMinus(c.spillWorklist, L(v, NULL));
    }

    c.coalescedNodes = tempUnion(c.coalescedNodes, L(v, NULL));
    G_enter(c.alias, nv, (void *) nu);

    AS_instrList au = (AS_instrList) Temp_lookPtr(c.moveList, u);
    AS_instrList av = (AS_instrList) Temp_lookPtr(c.moveList, v);
    au = instUnion(au, av);
    Temp_enterPtr(c.moveList, u, (void *) au);

    enableMoves(L(v, NULL));

    //Temp_tempList tadjs = adjacent(v);
    //G_nodeList adjs = tempL2NodeL(tadjs);
    G_nodeList adjs = G_adj(nv);
    Temp_tempList tadjs = nodeL2TempL(adjs);
    for (; adjs; adjs = adjs->tail) {
        G_node nt = adjs->head;
        nt = getAlias(nt);
        addEdge(nt, nu);
        decrementDegree(nt);
    }
    tadjs = NULL;

    long degree = (long long) G_look(c.degree, nu);
    if (degree >= c.K && tempIn(u, c.freezeWorklist)) {
        c.freezeWorklist = tempMinus(c.freezeWorklist, L(u, NULL));
        c.spillWorklist = tempUnion(c.spillWorklist, L(u, NULL));
    }
}

//合并，进行保守的合并，（第三步）
static void coalesce() {//错误注释语句信息的定位
    if (c.worklistMoves == NULL) {
        return;
    }

    AS_instr m = c.worklistMoves->head;
    Temp_temp x = tempHead(instUse(m));
    Temp_temp y = tempHead(instDef(m));
    Temp_temp u, v;

    x = node2Temp(getAlias(temp2Node(x)));
    y = node2Temp(getAlias(temp2Node(y)));

    if (Temp_look(c.precolored, x) != NULL) {
        u = y;
        v = x;
    } else {
        u = x;
        v = y;
    }
    G_node nu = temp2Node(u);
    G_node nv = temp2Node(v);

    c.worklistMoves = instMinus(c.worklistMoves, IL(m, NULL));

    if (u == v) {
        c.coalescedMoves = instUnion(c.coalescedMoves, IL(m, NULL));
        addWorkList(u);
    } else if (Temp_look(c.precolored, v) || G_goesTo(nu, nv) || G_goesTo(nv, nu)) {
        c.constrainedMoves = instUnion(c.constrainedMoves, IL(m, NULL));
        addWorkList(u);
        addWorkList(v);
    } else {
        bool flag = false;
        if (Temp_look(c.precolored, u)) {
            flag = true;
            Temp_tempList adj = adjacent(v);
            for (; adj; adj = adj->tail) {
                if (!OK(adj->head, u)) {
                    flag = false;
                    break;
                }
            }
        } else {
            Temp_tempList adju = adjacent(u);
            Temp_tempList adjv = adjacent(v);
            Temp_tempList adj = tempUnion(adju, adjv);
            flag = conservative(adj);
        }

        if (flag) {
            c.coalescedMoves = instUnion(c.coalescedMoves, IL(m, NULL));
            combine(u, v);
            addWorkList(u);
        } else {
            c.activeMoves = instUnion(c.activeMoves, IL(m, NULL));
        }
    }
}

// 冻结temp相关的move指令
static void freezeMoves(Temp_temp u) {
    AS_instrList il = nodeMoves(u);
    for (; il; il = il->tail) {
        AS_instr m = il->head;
        Temp_temp x = tempHead(instUse(m));
        Temp_temp y = tempHead(instDef(m));
        G_node nx = temp2Node(x);
        G_node ny = temp2Node(y);
        G_node nv;

        if (getAlias(nx) == getAlias(ny)) {
            nv = getAlias(nx);
        } else {
            nv = getAlias(ny);
        }
        Temp_temp v = node2Temp(nv);

        c.activeMoves = instMinus(c.activeMoves, IL(m, NULL));
        c.frozenMoves = instUnion(c.frozenMoves, IL(m, NULL));

        long degree = (long long) G_look(c.degree, nv);
        if (nodeMoves(v) == NULL && degree < c.K) {
            c.freezeWorklist = tempMinus(c.freezeWorklist, L(v, NULL));
            c.simplifyWorklist = tempUnion(c.simplifyWorklist, L(v, NULL));
        }
    }
}

//
// 冻结，(第四步），简化和合并都不能进行时，寻找一个度数较低的传送有关的节点，冻结与这个节点相关的传送指令
// 这将导致此节点被看作传送无关的，从而使得更多节点可简化
static void freeze() {
    if (c.freezeWorklist == NULL) {
        return;
    }

    Temp_temp u = c.freezeWorklist->head;
    c.freezeWorklist = tempMinus(c.freezeWorklist, L(u, NULL));
    c.simplifyWorklist = tempUnion(c.simplifyWorklist, L(u, NULL));
    freezeMoves(u);
}


//选择溢出
static void selectSpill() {
    if (c.spillWorklist == NULL) {
        return;
    }
    Temp_tempList tl = c.spillWorklist;
    float minSpillPriority = 9999.0f;
    Temp_temp m = NULL;
    for (; tl; tl = tl->tail) {
        Temp_temp t = tl->head;
        auto cost = (long long) Temp_lookPtr(c.spillCost, t);
        auto degree = (long long) G_look(c.degree, temp2Node(t));
        degree = (degree > 0) ? degree : 1;
        float priority = ((float) cost) / degree;
        if (priority < minSpillPriority) {
            minSpillPriority = priority;
            m = t;
        }
    }
    c.spillWorklist = tempMinus(c.spillWorklist, L(m, NULL));
    c.simplifyWorklist = tempUnion(c.simplifyWorklist, L(m, NULL));
    freezeMoves(m);
}

static void colorMain() {
    makeWorkList();
    do {
        if (c.simplifyWorklist != NULL) {
            simplify();//简化过程
        } else if (c.worklistMoves != NULL) {
            coalesce();
        } else if (c.freezeWorklist != NULL) {
            freeze();
        } else if (c.spillWorklist != NULL) {
            selectSpill();
        }
    } while (c.simplifyWorklist != NULL || c.worklistMoves != NULL ||
             c.freezeWorklist != NULL || c.spillWorklist != NULL);
}

struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs,
                            AS_instrList worklistMoves, Temp_map moveList, Temp_map spillCost) {
    struct COL_result ret;

    c.precolored = initial;//预着色节点map
    c.initial = NULL;
    c.simplifyWorklist = NULL;
    c.freezeWorklist = NULL;
    c.spillWorklist = NULL;
    c.spilledNodes = NULL;
    c.coalescedNodes = NULL;
    c.coloredNodes = NULL;
    c.selectStack = NULL;

    c.coalescedMoves = NULL;
    c.constrainedMoves = NULL;
    c.frozenMoves = NULL;
    c.worklistMoves = worklistMoves;//所有的mov命令表
    c.activeMoves = NULL;

    c.spillCost = spillCost;//temp------所有命令中defuse中含有temp的总和
    c.moveList = moveList;//所有move命令构成的列表
    c.degree = G_empty();
    c.alias = G_empty();
    c.nodes = ig;//冲突图

    c.K = tempCount(regs);//可用的寄存器个数


    Temp_map precolored = initial;//预着色表
    Temp_map colors = Temp_layerMap(Temp_empty(), initial);//将一个空表与与着色表连接
    Temp_tempList spilledNodes = NULL, coloredNodes = NULL;
    G_nodeList nodes = G_nodes(ig);//冲突图首结点，冲突图中对应节点信息为temp的指针
    G_nodeList temps = NULL;

    G_nodeList nl;
    for (nl = nodes; nl; nl = nl->tail) {//遍历冲突图

        long degree = G_degree(nl->head);//degree为冲突边个数

        G_enter(c.degree, nl->head, (void *) degree);//c.degree为每个节点当前度数的数组

        if (Temp_look(precolored, node2Temp(nl->head))) {//node2Temp返回temp指针，指向命令中的temp位置
            G_enter(c.degree, nl->head, (void *) 999);//冲突图中的预着色节点的度数设置为999
            continue;
        }
        c.initial = L(node2Temp(nl->head), c.initial);////c.initial临时寄存器集合，既没有预着色也没有处理
    }
    colorMain();//开始着色

    // for (nl = nodes; nl; nl = nl->tail) {
    //   if (Temp_look(precolored, node2Temp(nl->head))) {
    //     continue;
    //   }
    //   c.selectStack = L(node2Temp(nl->head), c.selectStack);
    // }

    c.selectStack = Temp_reverseList(c.selectStack);

    while (c.selectStack != NULL) {
        Temp_temp t = c.selectStack->head; // pop
        G_node n = temp2Node(t);
        c.selectStack = c.selectStack->tail;

        Temp_tempList okColors = cloneRegs(regs);
        G_nodeList adjs = G_adj(n);
        G_node nw;
        Temp_temp w;
        c_string color;

        for (; adjs; adjs = adjs->tail) {

            nw = adjs->head;
            w = node2Temp(nw);
            G_node nw_alias = getAlias(nw);
            Temp_temp w_alias = node2Temp(nw_alias);
            if ((color = Temp_look(colors, w_alias)) != NULL) {
                Temp_temp colorTemp = str2Color(color, precolored, regs);
                if (colorTemp) {
                    okColors = tempMinus(okColors, L(colorTemp, NULL));
                }
            }
        }
        if (okColors == NULL) {
            c.spilledNodes = L(t, c.spilledNodes);
        } else {
            coloredNodes = L(t, coloredNodes);
            Temp_enter(colors, t, color2Str(okColors->head, precolored));
        }
    }
    Temp_tempList tl;
    for (tl = c.coalescedNodes; tl; tl = tl->tail) {
        G_node alias = getAlias(temp2Node(tl->head));
        c_string color = Temp_look(colors, node2Temp(alias));
        Temp_enter(colors, tl->head, color);
    }

    ret.coloring = colors;

    ret.colored = NULL;
    for (; coloredNodes; coloredNodes = coloredNodes->tail) {
        ret.colored = L(coloredNodes->head, ret.colored);
    }

    ret.spills = NULL;
    for (; c.spilledNodes; c.spilledNodes = c.spilledNodes->tail) {
//        printf("spilled: %s\n", tempName(c.spilledNodes->head));
        ret.spills = L(c.spilledNodes->head, ret.spills);
    }

    ret.coalescedMoves = c.coalescedMoves;
    ret.coalescedNodes = c.coalescedNodes;
    ret.alias = c.alias;

    return ret;
}

static struct COL_result COL_color2(G_graph ig, Temp_map initial, Temp_tempList regs,
                                    AS_instrList worklistMoves, Temp_map moveList, Temp_map spillCost) {
    //your code here.
    struct COL_result ret;

    Temp_map precolored = initial;
    Temp_map colors = Temp_layerMap(Temp_empty(), initial);
    G_nodeList spilledNodes = NULL, coloredNodes = NULL;
    G_nodeList nodes = G_nodes(ig);
    G_nodeList temps = NULL;

    G_nodeList nl;
    for (nl = nodes; nl; nl = nl->tail) {
        if (Temp_look(precolored, node2Temp(nl->head))) {
            continue;
        }
        temps = G_NodeList(nl->head, temps);
    }

    while (temps != NULL) {
        G_node n = temps->head;
        Temp_tempList okColors = cloneRegs(regs);
        G_nodeList adjs = G_adj(n);
        G_node adj;
        c_string color;

        for (; adjs; adjs = adjs->tail) {
            adj = adjs->head;
            if ((color = Temp_look(colors, node2Temp(adj))) != NULL) {
                Temp_temp colorTemp = str2Color(color, precolored, regs);
                if (colorTemp) {
                    okColors = tempMinus(okColors,
                                         Temp_TempList(colorTemp, NULL));
                }
            }
        }

        if (okColors == NULL) {
            spilledNodes = G_NodeList(n, spilledNodes);
        } else {
            coloredNodes = G_NodeList(n, coloredNodes);
            Temp_enter(colors, node2Temp(n), color2Str(okColors->head, precolored));
        }

        // Next
        temps = temps->tail;
    }

    ret.coloring = colors;

    ret.colored = NULL;
    for (; coloredNodes; coloredNodes = coloredNodes->tail) {
        ret.colored = Temp_TempList(node2Temp(coloredNodes->head), ret.colored);
    }

    ret.spills = NULL;
    for (; spilledNodes; spilledNodes = spilledNodes->tail) {
//        printf("spilled: %s\n", tempName(node2Temp(spilledNodes->head)));
        ret.spills = Temp_TempList(node2Temp(spilledNodes->head), ret.spills);
    }

    ret.alias = G_empty();
    ret.coalescedMoves = NULL;
    ret.coalescedNodes = NULL;

    return ret;
}



