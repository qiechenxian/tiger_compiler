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
    Temp_tempList lastStack; // selectStack的最后一个

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

    int spilled_min_temp_num;
};

static COL_ctx c;

void COL_init_spilled_temp_num(int num) {
    c.spilled_min_temp_num = num;
}

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
    Temp_tempList tl, head = NULL, last = NULL;

    for (; regs; regs = regs->tail) {

        Temp_tempList p = (Temp_tempList) checked_malloc(sizeof(*p));
        p->head = regs->head;
        p->tail = NULL;

        if(head == NULL) {
            head = p;
            last = p;
        } else {
            last->tail = p;
            last = p;
        }
    }

    return head;
}

//temp列表头
static Temp_temp tempHead(Temp_tempList temps) {
    if (temps == NULL) return NULL;
    return temps->head;
}

//temp to node
G_node temp2Node(Temp_temp t) {
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

static Temp_tempList adjacent(G_node n) {
    G_nodeList adjn = G_adj(n);

    Temp_tempList adjs = NULL;
    for (; adjn; adjn = adjn->tail) {
        adjs = L(node2Temp(adjn->head), adjs);
    }

    //adj - （从图中删除的+已经合并的）
    adjs = tempMinus(adjs, tempUnion(c.selectStack, c.coalescedNodes));

    return adjs;
}

//冲突的temp list
static Temp_tempList adjacent(Temp_temp t) {
    G_node n = temp2Node(t);
    return adjacent(n);
}

//添加一条边
static void addEdge(G_node nu, G_node nv) {

    if (nu == nv) return;
    if (G_goesTo(nu, nv) && G_goesTo(nv, nu)) return;
    G_addEdge(nu, nv);

    if (!precolred(nu)) {
        degreeAdd(nu, 1);
    }

    if (!precolred(nv)) {
        degreeAdd(nv, 1);
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
static void makeWorkList() {

    //低度数的传送无关表，一般来说当一个变量的冲突边小于K时其放入低度数的冲突无关表，K为当前使用的寄存器个个数
    Temp_tempList tl;

    // 遍历预着色节点
    for (tl = c.initial; tl; tl = tl->tail) {
        Temp_temp t = tl->head;
        G_node n = temp2Node(t);

        //在冲突图中查找预着色节点

        if (G_degree(n) >= c.K) {
#ifdef DEBUG_PRINT
            printf("Init2Spill:%d\n", t->num);
#endif
            c.spillWorklist = tempUnion(c.spillWorklist, L(t, NULL));
        } else if (moveRelated(t)) {
#ifdef DEBUG_PRINT
            printf("Init2Freeze:%d\n", t->num);
#endif
            c.freezeWorklist = tempUnion(c.freezeWorklist, L(t, NULL));
        } else {
#ifdef DEBUG_PRINT
            printf("Init2Simplify:%d\n", t->num);
#endif
            c.simplifyWorklist = tempUnion(c.simplifyWorklist, L(t, NULL));
        }
//        Temp_tempList spillWorklist;//高度数的节点表
//        Temp_tempList freezeWorklist;//低度数的传送有关的节点表
//        Temp_tempList simplifyWorklist;//低度数的传送无关的节点表
    }

    c.initial = NULL;
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

    if(c.activeMoves == nullptr) return;

    for (; tl; tl = tl->tail) {
        AS_instrList il = nodeMoves(tl->head);
        for (; il; il = il->tail) {
            AS_instr m = il->head;
            if(m->isDead) continue;
            if (instIn(m, c.activeMoves)) {
                c.activeMoves = instMinus(c.activeMoves, IL(m, NULL));
                c.worklistMoves = instUnion(c.worklistMoves, IL(m, NULL));
            }
        }
    }
}

//
static void decrementDegree(G_node n) {

    long d = degreeAdd(n, -1);

    if (d == (c.K - 1)) {

        Temp_temp t = node2Temp(n);
        enableMoves(L(t, adjacent(t)));
        c.spillWorklist = tempMinus(c.spillWorklist, L(t, NULL));
        if (moveRelated(t)) {
            c.freezeWorklist = tempUnion(c.freezeWorklist, L(t, NULL));
        } else {
            c.simplifyWorklist = tempUnion(c.simplifyWorklist, L(t, NULL));
        }
    }
}

static void deleteEdge(G_node nu, G_node nv) {
    if (nu == nv) return;

    if (!G_goesTo(nu, nv) || !G_goesTo(nv, nu)) {
        printf("delete edge error");
        return;
    }

    G_rmEdge(nu, nv);
    G_rmEdge(nv, nu);

    decrementDegree(nu);
    decrementDegree(nv);
}

static void addWorkList(Temp_temp u) {
    if (precolred(u) &&
        (!moveRelated(u)) &&
        (degree(u) < c.K)) {
        c.freezeWorklist = tempMinus(c.freezeWorklist, L(u, NULL));
        c.simplifyWorklist = tempUnion(c.simplifyWorklist, L(u, NULL));
    }
}

// temp t ok？
static bool OK(Temp_temp t, Temp_temp r) {
    return (degree(t) < c.K) || precolred(t) || G_goesTo(t, r) || G_goesTo(r, t);
}

//保守的，briggs合并策略
static bool conservative(Temp_tempList tl) {
    int k = 0;
    for (; tl; tl = tl->tail) {
        if (degree(tl->head) >= c.K) {
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

//得到别名
static G_node getAlias(Temp_temp n) {
    if (tempIn(n, c.coalescedNodes)) {
        G_node n_node = temp2Node(n);
        G_node alias = (G_node) G_look(c.alias, n_node);
        return getAlias(alias);
    } else {
        return temp2Node(n);
    }
}


//简化（第二步）第一步的冲突图构建由solve_liveness解决
static void simplify() {

    if (c.simplifyWorklist == NULL) {
        //c.simplifyWorklist低度数的传送无关的节点表
        return;
    }

    Temp_temp t = nullptr;
    if(c.spilled_min_temp_num != -1) {
        // 优先着色首先溢出的寄存器，因此这里优先取小于新建临时变量的变量
        Temp_tempList currentWorkList = c.simplifyWorklist;
        Temp_tempList prevWorkList = nullptr;
        for(;currentWorkList; currentWorkList = currentWorkList->tail) {
            if(currentWorkList->head->num < c.spilled_min_temp_num) {
                // 找到了
                t = currentWorkList->head;
                if(prevWorkList != nullptr) {
                    prevWorkList->tail = currentWorkList->tail;
                } else {
                    c.simplifyWorklist = c.simplifyWorklist->tail;
                }
                break;
            }
            prevWorkList = currentWorkList;
        }
    }

    if(nullptr == t) {
        t = c.simplifyWorklist->head;
        c.simplifyWorklist = c.simplifyWorklist->tail;
    }

    G_node n = temp2Node(t);

#ifdef DEBUG_PRINT
    printf("Simplify:%d\n", t->num);
#endif

    c.selectStack = L(t, c.selectStack);  // push

    // 降低邻居节点的度数
    Temp_tempList adjs = adjacent(n);
    for (; adjs; adjs = adjs->tail) {
        G_node m = temp2Node(adjs->head);
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

    Temp_tempList adjs = adjacent(nv);
    for (; adjs; adjs = adjs->tail) {
        G_node nt = temp2Node(adjs->head);
        addEdge(nt, nu);
        decrementDegree(nt);
    }

    if (degree(u) >= c.K && tempIn(u, c.freezeWorklist)) {
        c.freezeWorklist = tempMinus(c.freezeWorklist, L(u, NULL));
        c.spillWorklist = tempUnion(c.spillWorklist, L(u, NULL));
    }
}

bool precolred(Temp_temp u)
{
    return (u != NULL) && (Temp_look(c.precolored, u) != NULL);
}

bool precolred(G_node nu)
{
    Temp_temp u = node2Temp(nu);

    return precolred(u);
}

bool adjacentOK(Temp_temp v, Temp_temp u)
{
    Temp_tempList adj = adjacent(v);
    for (; adj; adj = adj->tail) {
        if (!OK(adj->head, u)) {
            return false;
        }
    }

    return true;
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

    if (precolred(x)) {
        u = y;
        v = x;
    } else {
        u = x;
        v = y;
    }

    c.worklistMoves = instMinus(c.worklistMoves, IL(m, NULL));

    if (u == v) {
        c.coalescedMoves = instUnion(c.coalescedMoves, IL(m, NULL));
        addWorkList(u);
    } else if (precolred(v) || G_goesTo(u, v) || G_goesTo(v, u)) {
        c.constrainedMoves = instUnion(c.constrainedMoves, IL(m, NULL));
        addWorkList(u);
        addWorkList(v);
    } else if((precolred(u) && adjacentOK(u, v)) ||
              ((!precolred(u)) && conservative(tempUnion(adjacent(u), adjacent(v))))){

        c.coalescedMoves = instUnion(c.coalescedMoves, IL(m, NULL));
        combine(u, v);
        addWorkList(u);
    } else {
        c.activeMoves = instUnion(c.activeMoves, IL(m, NULL));
    }
}

// 冻结temp相关的move指令
static void freezeMoves(Temp_temp u) {
    AS_instrList il = nodeMoves(u);
    for (; il; il = il->tail) {
        AS_instr m = il->head;
        Temp_temp x = tempHead(instUse(m));
        Temp_temp y = tempHead(instDef(m));
        Temp_temp v;

        if (getAlias(y) == getAlias(u)) {
            v = node2Temp(getAlias(x));
        } else {
            v = node2Temp(getAlias(y));
        }

        c.activeMoves = instMinus(c.activeMoves, IL(m, NULL));
        c.frozenMoves = instUnion(c.frozenMoves, IL(m, NULL));

        if (nodeMoves(v) == NULL && degree(v) < c.K) {
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
    Temp_tempList tl;
    float minSpillPriority = 9999.0f;
    Temp_temp m = NULL;

    // 曾经溢出，优先选择这些溢出的优先着色
    if(c.spilled_min_temp_num != -1) {
        tl = c.spillWorklist;
        for (; tl; tl = tl->tail) {
            Temp_temp t = tl->head;
            if((t->num >= c.spilled_min_temp_num)) {
                // 小于的先不选择
                continue;
            }
            long cost = (long long) Temp_lookPtr(c.spillCost, t);
            long degree = (long long) G_look(c.degree, temp2Node(t));
            degree = (degree > 0) ? degree : 1;
            float priority = ((float) cost) / degree;
            if ((priority < minSpillPriority)) {
                minSpillPriority = priority;
                m = t;
            }
        }
    }

    // 没有选择到，则重新选择
    if(m == NULL) {
        tl = c.spillWorklist;
        for (; tl; tl = tl->tail) {
            Temp_temp t = tl->head;
            long cost = (long long) Temp_lookPtr(c.spillCost, t);
            long degree = (long long) G_look(c.degree, temp2Node(t));
            degree = (degree > 0) ? degree : 1;
            float priority = ((float) cost) / degree;
            if ((priority < minSpillPriority)) {
                minSpillPriority = priority;
                m = t;
            }
        }
    }

    if(m != NULL) {
        // 从溢出队列中移动到simpify队列中
#ifdef DEBUG_PRINT
        printf("Spill2Simplify:%d\n", m->num);
#endif
        c.spillWorklist = tempMinus(c.spillWorklist, L(m, NULL));
        c.simplifyWorklist = tempUnion(c.simplifyWorklist, L(m, NULL));
        freezeMoves(m);
    }
}

static void colorMain() {

    // 把initial中的未着色的寄存器变量按照冲突图的度数进行分类
    // 大于可用寄存器最大值的作为溢出的结点
    // 针对Move指令进行特定的处理
    // 小于度数最大阈值的课加入到simplify中
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

struct COL_result COL_color(G_graph ig, Temp_map precolored_map, Temp_tempList regs, Temp_tempList initial,
                            AS_instrList worklistMoves, Temp_map moveList, Temp_map spillCost) {
    struct COL_result ret;

    //预着色节点map
    c.precolored = precolored_map;

    // 临时寄存器集合，其中的元素既没有预着色，也没有被处理
    c.initial = initial;

    // 低度数的传送无关的结点表
    c.simplifyWorklist = NULL;

    // 低度数的传送有关的结点表
    c.freezeWorklist = NULL;

    // 高度数的结点表
    c.spillWorklist = NULL;

    // 本轮重要溢出的结点集合，初始为空
    c.spilledNodes = NULL;

    // 已合并的寄存器集合。当合并u <- v时，将v加入到这个结合总，u被放回到某个工作表中（或反之）
    c.coalescedNodes = NULL;

    // 已成功着色的结点集合
    c.coloredNodes = NULL;

    // 一个包含从图中删除的临时变量的栈
    c.selectStack = NULL;
    c.lastStack = NULL;

    // 已经合并的传送指令集合
    c.coalescedMoves = NULL;

    // 源操作数和目标操作数冲突的传送指令集合
    c.constrainedMoves = NULL;

    // 不再考虑合并的传送指令集合
    c.frozenMoves = NULL;

    // 有可能合并的传送指令集合
    c.worklistMoves = worklistMoves;//所有的mov命令表

    // 还未做好合并准备得传送指令集合
    c.activeMoves = NULL;

    //temp------所有命令中defuse中含有temp的总和
    c.spillCost = spillCost;

    //所有move命令构成的列表
    c.moveList = moveList;
    c.degree = G_empty();

    // 当一条传送指令(u,v)已被合并，并且v已放入到合并结点集合coalesedNodes时，有alias(v) = u
    c.alias = G_empty();

    // 冲突图
    c.nodes = ig;

    //可用的寄存器个数
    c.K = tempCount(regs);

    // c.color 算法为结点选择的颜色，对于预着色结点，其初值为给定的颜色

    Temp_map colors = Temp_layerMap(Temp_empty(), precolored_map);//将一个空表与与着色表连接

    // 冲突图首结点，冲突图中对应节点信息为temp的指针
    G_nodeList nodes = G_nodes(ig);

    // 遍历冲突图，构建寄存器变量与度数的映射表，
    // 同时把冲突图的所有结点对应的临时寄存器变量都作为未着色的变量加入到initial中
    G_nodeList nl;
    for (nl = nodes; nl; nl = nl->tail) {

        // degree为冲突边个数
        long degree = G_degree(nl->head);

        // node2Temp返回temp指针，指向命令中的temp位置
        if (Temp_look(precolored_map, node2Temp(nl->head))) {
            //冲突图中的预着色节点的度数设置为999
            G_enter(c.degree, nl->head, (void *) 999);
            continue;
        }

        //c.degree为每个节点当前度数的数组
        G_enter(c.degree, nl->head, (void *) degree);

        //// c.initial临时寄存器集合，既没有预着色也没有处理
        Temp_temp tn = node2Temp(nl->head);

#ifdef DEBUG_PRINT
        printf("NODE:%d\n", tn->num);
#endif

        c.initial = L(tn, c.initial);
    }

    colorMain();

    // COL_init_spilled_temp_num(-1);

#if 0
    for (nl = nodes; nl; nl = nl->tail) {
        if (Temp_look(precolored_map, node2Temp(nl->head))) {
            continue;
        }
        c.selectStack = L(node2Temp(nl->head), c.selectStack);
    }
#endif

    Temp_tempList okColors = nullptr;

    // 下面开始着色
    while (c.selectStack != NULL) {
        Temp_temp t = c.selectStack->head; // pop
        G_node n = temp2Node(t);
        c.selectStack = c.selectStack->tail;

        G_node w;
        c_string color;

        okColors = cloneRegs(regs);

        G_nodeList adjs = G_adj(n);
        for (; adjs; adjs = adjs->tail) {

            G_node w = adjs->head;
            G_node nw_alias = getAlias(w);
            Temp_temp w_alias = node2Temp(nw_alias);
            if(Temp_inList(w_alias, c.coloredNodes) || Temp_look(precolored_map, w_alias)) {
                if ((color = Temp_look(colors, w_alias)) != NULL) {
                    Temp_temp colorTemp = str2Color(color, precolored_map, regs);
                    if (colorTemp) {
                        okColors = tempMinus(okColors, L(colorTemp, NULL));
                    }
                }
            }
        }

        if (okColors == NULL) {
            c.spilledNodes = L(t, c.spilledNodes);
        } else {
            c.coloredNodes = L(t, c.coloredNodes);
            Temp_enter(colors, t, color2Str(okColors->head, precolored_map));
#ifdef DEBUG_PRINT
            printf("Color: %d -- %d\n", t->num, okColors->head->num);
#endif
        }
    }

    Temp_tempList tl;
    for (tl = c.coalescedNodes; tl; tl = tl->tail) {
        Temp_temp n = tl->head;

        G_node alias = getAlias(n);
        Temp_temp alias_temp = node2Temp(alias);
        c_string aliasColor = Temp_look(colors, alias_temp);

        if(aliasColor) {
            Temp_enter(colors, n, aliasColor);
        }
    }

    ret.coloring = colors;

    ret.colored = NULL;
    Temp_tempList coloredNodes = c.coloredNodes;
    for (; coloredNodes; coloredNodes = coloredNodes->tail) {
        ret.colored = L(coloredNodes->head, ret.colored);
    }

    ret.spills = NULL;
    for (; c.spilledNodes; c.spilledNodes = c.spilledNodes->tail) {
#ifdef DEBUG_PRINT
        printf("spilled: %s\n", tempName(c.spilledNodes->head));
#endif
        ret.spills = L(c.spilledNodes->head, ret.spills);
    }

    ret.coalescedMoves = c.coalescedMoves;
    ret.coalescedNodes = c.coalescedNodes;
    ret.alias = c.alias;

    return ret;
}

long degree(Temp_temp t) {
    return (long long) G_look(c.degree, temp2Node(t));
}

long degree(G_node n) {
    return (long long) G_look(c.degree, n);
}

long degreeAdd(G_node nu, long num) {
    long d = (long long) G_look(c.degree, nu);
    long old = d;
    d += num;
    G_enter(c.degree, nu, (void *) d);
    return old;
}

long degreeAdd(Temp_temp u, long num) {
    G_node nu = temp2Node(u);
    return degreeAdd(nu, num);
}

