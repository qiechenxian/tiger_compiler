//
// Created by zcjsh on 2020/7/26.
//

#ifndef COMPILER_GRAPH_H
#define COMPILER_GRAPH_H

#include <stdio.h>
#include "table.h"

typedef struct G_graph_ *G_graph;  /* 图类型 */
typedef struct G_node_ *G_node;    /* 节点类型 */

typedef struct G_nodeList_ *G_nodeList; /* 节点表*/
struct G_nodeList_ {
    G_node head;
    G_nodeList tail;
};

struct G_node_ {
    G_graph mygraph;
    int mykey;
    G_nodeList succs;
    G_nodeList preds;
    void *info;
};

/* 创建新的图 */
G_graph G_Graph(void);

void G_Graph_free(G_graph graph);

/* 创建新的图节点 */
G_node G_Node(G_graph g, void *info);

/* 创建图节点列表 */
G_nodeList G_NodeList(G_node head, G_nodeList tail);

/* 获得图的节点表 */
G_nodeList G_nodes(G_graph g);

/* 翻转列表 */
G_nodeList G_reverseNodes(G_nodeList l);

/* a是否属于l*/
bool G_inNodeList(G_node a, G_nodeList l);

/* 创建一条新的边 从from到to */
void G_addEdge(G_node from, G_node to);

/* 删除一条边 从from到to */
void G_rmEdge(G_node from, G_node to);

/* 输出一个图中所有的节点和边信息，使用showinfo输出每个节点的信息 */
void G_show(FILE *out, G_nodeList p, void showInfo(void *));

/* 获得n的所有后继 */
G_nodeList G_succ(G_node n);

/* 获得n的所有前驱 */
G_nodeList G_pred(G_node n);

/* 节点n的前驱和后继 */
G_nodeList G_adj(G_node n);

/* 是否有从from到n的边*/
bool G_goesTo(G_node from, G_node n);

/* 节点n的度（入和出） */
int G_degree(G_node n);

/* 获得节点n的信息 */
void *G_nodeInfo(G_node n);

/* 映射节点到任何事物 */
typedef struct TAB_table_ *G_table;

/* 创建一个新表 */
G_table G_empty(void);

/* 在表t中建立node->value */
void G_enter(G_table t, G_node node, void *value);

/* 在表t中查找node->value */
void *G_look(G_table t, G_node node);

void G_free(G_table table);

#endif //COMPILER_GRAPH_H
