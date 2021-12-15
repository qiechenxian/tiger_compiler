//
// Created by zcjsh on 2020/7/26.
//

#include "util.h"
#include "errormsg.h"
#include "graph.h"
#include "temp.h"
#include "color.h"

G_graph G_Graph(F_frame frame) {
    G_graph g = (G_graph) checked_malloc(sizeof *g);
    g->nodecount = 0;
    g->mynodes = NULL;
    g->mylast = NULL;
    g->frame = frame;
    return g;
}

void G_Graph_free(G_graph graph)
{
    G_nodeList l = graph->mynodes;
    G_nodeList nl;
    for (; l;) {
        nl = l;
        l = l->tail;
        free(nl);
    }

    free(graph);
}

F_frame G_frame(G_graph graph)
{
    return graph->frame;
}

G_nodeList G_NodeList(G_node head, G_nodeList tail) {
    G_nodeList n = (G_nodeList) checked_malloc(sizeof *n);
    n->head = head;
    n->tail = tail;
    return n;
}
G_nodeList G_nodeList_intersect(G_nodeList ta, G_nodeList tb) {
    G_node t;
    G_nodeList tl = NULL;
    TAB_table m = TAB_empty();

    for (; ta; ta = ta->tail) {
        t = ta->head;
        TAB_enter(m, t, (char *)"i");
    }

    for (; tb; tb = tb->tail) {
        t = tb->head;
        if (TAB_look(m, t) != NULL) {
            tl = G_NodeList(t, tl);
        }
    }

    free(m);
    return tl;
}
G_nodeList G_NodeList_minus(G_nodeList table, G_nodeList rmove) {
    G_nodeList minus=NULL;
    bool tag;
    TAB_table m = TAB_empty();
    TAB_table l = TAB_empty();
    G_nodeList temp_table=table;
    for(;temp_table;temp_table=temp_table->tail) {
        TAB_enter(m,temp_table->head,(char*)"m");
    }
    temp_table=rmove;
    for(;temp_table;temp_table=temp_table->tail){
        TAB_enter(l,temp_table->head,(char*)"m");
    }
    temp_table=table;
    for(;temp_table;temp_table=temp_table->tail) {
        if(TAB_look(m,temp_table->head)!=NULL)
        {
            if(TAB_look(l,temp_table->head)==NULL)
            {
                minus=G_NodeList(temp_table->head,minus);
            }
        }
    }
    free(m);
    return minus;
}
G_nodeList G_NodeList_union(G_nodeList na, G_nodeList nb) {
    G_nodeList union_nodelist=NULL;
    bool tag;
    TAB_table m = TAB_empty();
    G_nodeList temp_table=na;
    for(;temp_table;temp_table=temp_table->tail) {
        TAB_enter(m,temp_table->head,(char*)"m");
        union_nodelist=G_NodeList(temp_table->head,union_nodelist);
    }
    temp_table=nb;
    for(;temp_table;temp_table=temp_table->tail){
        if(TAB_look(m,temp_table->head)==NULL)
        {
            TAB_enter(m,temp_table->head,(char*)"m");
            union_nodelist=G_NodeList(temp_table->head,union_nodelist);
        }
    }
    free(m);
    return union_nodelist;
}
bool G_NodeList_equal(G_nodeList na, G_nodeList nb) {
    G_node t;
    G_nodeList tl = NULL;
    TAB_table m = TAB_empty();
    int ca = 0, cb = 0;

    for (; na; na = na->tail) {
        t = na->head;
        TAB_enter(m, t, (char *)"e");
        ++ca;
    }

    for (; nb; nb = nb->tail) {
        t = nb->head;
        if (TAB_look(m, t) == NULL) {
            return false;
        }
        ++cb;
    }
    free(m);
    return (ca == cb);
}
/* generic creation of G_node */
G_node G_Node(G_graph g, void *info) {
    G_node n = (G_node) checked_malloc(sizeof *n);
    G_nodeList p = G_NodeList(n, NULL);
    EM_ASSERT_CODE=-63;
    assert(g);
    n->mygraph = g;
    n->mykey = g->nodecount++;

    if (g->mylast == NULL)
        g->mynodes = g->mylast = p;
    else g->mylast = g->mylast->tail = p;

    n->succs = NULL;
    n->preds = NULL;
    n->info = info;
    return n;
}

G_nodeList G_nodes(G_graph g) {
    EM_ASSERT_CODE=-64;
    assert(g);
    return g->mynodes;
}

G_nodeList G_reverseNodes(G_nodeList l) {
    G_nodeList nl = NULL;
    for (; l; l = l->tail)
        nl = G_NodeList(l->head, nl);
    return nl;
}

/* return true if a is in l list */
bool G_inNodeList(G_node a, G_nodeList l) {
    G_nodeList p;
    for (p = l; p != NULL; p = p->tail)
        if (p->head == a) return true;
    return false;
}

void G_addEdge(G_node from, G_node to) {
    EM_ASSERT_CODE=-65;
    assert(from);
    EM_ASSERT_CODE=-66;
    assert(to);
    EM_ASSERT_CODE=-67;
    assert(from->mygraph == to->mygraph);
    if (G_goesTo(from, to)) return;
    to->preds = G_NodeList(from, to->preds);
    from->succs = G_NodeList(to, from->succs);
}

static G_nodeList del(G_node a, G_nodeList l) {
    EM_ASSERT_CODE=-68;
    assert(a && l);
    if (a == l->head)
        return l->tail;
    else
        return G_NodeList(l->head, del(a, l->tail));
}

void G_rmEdge(G_node from, G_node to) {
    EM_ASSERT_CODE=-69;
    assert(from && to);
    to->preds = del(from, to->preds);
    from->succs = del(to, from->succs);
}

/**
 * Print a human-readable dump for debugging.
 */
void G_show(FILE *out, G_nodeList p, void showInfo(void *)) {
    for (; p != NULL; p = p->tail) {
        G_node n = p->head;
        G_nodeList q;
        EM_ASSERT_CODE=-70;
        assert(n);
        if (showInfo)
            showInfo(n->info);
        fprintf(out, " (%d): ", n->mykey);
        for (q = G_pred(n); q != NULL; q = q->tail)
            fprintf(out, "%d ", q->head->mykey);
        fprintf(out, "\n");
    }
}

G_nodeList G_succ(G_node n) {
    EM_ASSERT_CODE=-71;
    assert(n);
    return n->succs;
}

G_nodeList G_pred(G_node n) {
    EM_ASSERT_CODE=-72;
    assert(n);
    return n->preds;
}

bool G_goesTo(G_node from, G_node n) {
    return G_inNodeList(n, G_succ(from));
}

bool G_goesTo(Temp_temp from, Temp_temp n) {
    G_node from_node = temp2Node(from);
    G_node to_node = temp2Node(n);

    return G_inNodeList(to_node, G_succ(from_node));
}

/* return length of predecessor list for node n */
static int inDegree(G_node n) {
    int deg = 0;
    G_nodeList p;
    for (p = G_pred(n); p != NULL; p = p->tail) deg++;
    return deg;
}

/* return length of successor list for node n */
static int outDegree(G_node n) {
    int deg = 0;
    G_nodeList p;
    for (p = G_succ(n); p != NULL; p = p->tail) deg++;
    return deg;
}

int G_degree(G_node n) { return inDegree(n) + outDegree(n); }

/* put list b at the back of list a and return the concatenated list */
static G_nodeList cat(G_nodeList a, G_nodeList b) {
    if (a == NULL) return b;
    else return G_NodeList(a->head, cat(a->tail, b));
}

/* create the adjacency list for node n by combining the successor and
 * predecessor lists of node n */
G_nodeList G_adj(G_node n) { return cat(G_succ(n), G_pred(n)); }

void *G_nodeInfo(G_node n) { return n->info; }


/* G_node table functions */

G_table G_empty(void) {
    return TAB_empty();
}

void G_enter(G_table t, G_node node, void *value) {
    TAB_enter(t, node, value);
}

void *G_look(G_table t, G_node node) {
    return TAB_look(t, node);
}

void G_free(G_table table)
{
    TAB_free(table);
}