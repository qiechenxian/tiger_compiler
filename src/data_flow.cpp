//
// Created by 且趁闲 on 13/8/2020.
//
#include "data_flow.h"
TAB_table global_save_div=NULL;
static bool tempEqual(Temp_tempList ta, Temp_tempList tb) {
    return Temp_equal(ta, tb);
}
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
static Temp_tempList tempMinus(Temp_tempList ta, Temp_tempList tb) {
    return Temp_minus(ta, tb);
}
static Temp_tempList tempUnion(Temp_tempList ta, Temp_tempList tb) {
    return Temp_union(ta, tb);
}
static void entertempMap(G_table t, Temp_temp temp, G_nodeList node_list) {//temp为索引信息
    TAB_enter(t,temp,node_list);
}
static G_nodeList lookuptempMap(G_table t, Temp_temp temp) {
    return (G_nodeList)TAB_look(t,temp);
}
static void enternnodeMap(G_table t, G_node node, G_nodeList node_list) {//node为索引信息
    TAB_enter(t,node,node_list);
}
static G_nodeList lookupnodeMap(G_table t, G_node node) {
    return (G_nodeList)TAB_look(t,node);
}
static void enterstringMap(G_table t, G_node node, c_string string) {//node为索引信息
    TAB_enter(t,node,string);
}
c_string lookupstringMap(G_table t, G_node node) {
    return (c_string)TAB_look(t,node);
}
static void getdefs(G_graph flow, G_table defs, G_table gen, G_table kill,G_table defuses,G_table expression) {//构建defs，gen，kill，defs(t)为所有的为临时变量t定值的节点集合,gen为生成定值的节点，而kills为定值节点杀死的其他的某临时变量的定值
    G_nodeList fl, save;
    G_node n;
    Temp_tempList def_list;
    Temp_tempList def_use;

        for (fl =G_nodes(flow) ; fl; fl = fl->tail) {//fl用于遍历flow结点 构建defs,与defsuses
            n = fl->head;
            if(FG_tag(n))
            def_list=FG_def(n);
            if(!div_mod_call(n))
            {
                def_list=FG_def(n);
            }
            else if(div_mod_call(n))
            {
                def_list=FG_div_def(n);
            }
            for(;def_list;def_list=def_list->tail)//遍历节点def的临时变量
            {
                save=lookuptempMap(defs,def_list->head);
                if(G_inNodeList(n,save))
                {
                    continue;
                } else
                {
                    save=G_NodeList(n,save);
                    entertempMap(defs,def_list->head,save);//defs(temp)=defs(temp)+node n

                }
            }
            if(!div_mod_call(n))
            {
                def_use=Temp_union(FG_def(n),FG_use(n));
            }
            else if(div_mod_call(n))
            {
                def_use=Temp_union(FG_div_use(n),FG_div_def(n));
            }
            for(;def_use;def_use=def_use->tail)//遍历节点def_use的临时变量
            {
                save=lookuptempMap(defuses,def_use->head);
                if(G_inNodeList(n,save))
                {
                    continue;
                } else
                {
                    save=G_NodeList(n,save);
                    entertempMap(defuses,def_use->head,save);//defues的构建涉及到div的常量处理,构建一个特殊的temp，但其无法发被杀死且放入另一个temp散列表中，temp添加一个bool域

                }
            }
        }

    for (fl =G_nodes(flow) ; fl; fl = fl->tail) {//得到defs后遍历第二遍用于构造gen和kill///////////////////////////////////////////
        n = fl->head;
        def_list=FG_def(n);
        if(FG_kind(n)==AS_instr_::O_div||FG_kind(n)==AS_instr_::O_mod)
        {
            def_list=FG_div_def(n);
        }
        if(def_list!=NULL)
        {
            bool tag=false;
            if(FG_kind(n)==AS_instr_::O_div||FG_kind(n)==AS_instr_::O_mod)
            {
                Temp_temp templ_dst=FG_div_def(n)->head;
                int dst_number=Temp_number(templ_dst);
                c_string test=(char *) checked_malloc(sizeof(char) * 40);
                if(!FG_tag(n))
                {
                    Temp_tempList templ=FG_div_use(n);
                    sprintf(test,"%d t%d t%d",FG_kind(n),Temp_number(templ->head),Temp_number(templ->tail->head));
                    if(Temp_number(templ->head)==dst_number||Temp_number(templ->tail->head)==dst_number)
                    {
                        tag=true;
                    }
                }
                else
                {
                    Temp_tempList templ=FG_div_use(n);
                    if(Temp_tag(templ->head)==true)
                    {
                        sprintf(test,"%d #%d t%d",FG_kind(n),Temp_number(templ->head),Temp_number(templ->tail->head));
                        if(Temp_number(templ->tail->head)==dst_number)
                        {
                            tag=true;
                        }
                    }
                    else
                    {
                        sprintf(test,"%d t%d #%d",FG_kind(n),Temp_number(templ->head),Temp_number(templ->tail->head));
                        if(Temp_number(templ->head)==dst_number)
                        {
                            tag=true;
                        }
                    }

                }
                if(lookupstringMap(expression,n)==NULL)
                {
                    enterstringMap(expression,n,test);
                }
                enternnodeMap(kill,n,G_NodeList_minus(lookuptempMap(defuses,def_list->head),G_NodeList(n,NULL)));//包含t的表达式入kill（n）
                if(tag==false)
                {
                    enternnodeMap(gen,n,G_NodeList(n,NULL));//tag==true时表达式定值会自己杀自己
                }

            }
            else if(FG_tag(n)== true&&FG_kind(n)==AS_instr_::O_mov)
            {
                enternnodeMap(gen,n,NULL);
                enternnodeMap(kill,n,NULL);
            }
            else
            {
//                if(FG_tag(n)== false&&FG_kind(n)==AS_instr_::O_mov)
//                {
//                    printf("%s   \n",FG_inst(n)->u.OPER.assem);
//                }

                enternnodeMap(kill,n,G_NodeList_minus(lookuptempMap(defuses,def_list->head),G_NodeList(n,NULL)));//包含t的表达式入kill（n）
//                if(n->mykey==122)
//                {
//                    G_nodeList temp_n=lookupnodeMap(kill,n);
//                    for (;temp_n;temp_n=temp_n->tail) {
//                        printf("122 kill %d\n",temp_n->head->mykey);
//                    }
//                }
            }
        } else
        {
            enternnodeMap(gen,n,NULL);
            enternnodeMap(kill,n,NULL);
        }
    }
}

static void def_value(G_graph flow,G_table gen,G_table kill,G_table def_uses,G_table in,G_table out) {//构建defs，gen，kill，defs(t)为所有的为临时变量t定值的节点集合,gen为生成定值的节点，而kills为定值节点杀死的其他的某临时变量的定值

//    G_show(stderr, G_nodes(flow), printInst);
    G_nodeList fl=NULL;
    G_nodeList sl=NULL;
    G_nodeList li=NULL;
    G_nodeList lo=NULL;
    G_nodeList ci=NULL;
    G_nodeList co=NULL;
    G_table last_in = G_empty();
    G_table last_out = G_empty();
    G_node n, sn;
    bool flag=true;

//    int q=0;
//    printf("\nd------------in-----------------d\n");
//    for (fl = G_nodes(flow); fl; fl = fl->tail) {
//
//        G_nodeList temp_ll;
//
//        n = fl->head;
//        li = lookupnodeMap(in, n);
//        lo = lookupnodeMap(out, n);
//        ci = lookupnodeMap(last_in, n);
//        co = lookupnodeMap(last_out, n);
//
//        printf("\nnode-in:%d:\n",q);
//        for(temp_ll=li;temp_ll;temp_ll=temp_ll->tail)
//        {
//            printf(",%d",temp_ll->head->mykey);
//        }
//        printf("\nnode-out:%d:\n",q);
//        for(temp_ll=lo;temp_ll;temp_ll=temp_ll->tail)
//        {
//            printf(",%d",temp_ll->head->mykey);
//        }
//        q++;
//    }
//    printf("\nup------------in-----------------up\n");
    while(flag)
    {
        for(fl =G_nodes(flow) ; fl; fl = fl->tail)
        {
            n = fl->head;
            li = lookupnodeMap(in, n);
            lo = lookupnodeMap(out, n);
            enternnodeMap(last_in, n, li);
            enternnodeMap(last_out, n, lo);
            ci=G_nodes(flow);
            if(G_pred(n)==NULL)
            {
                ci= nullptr;
            }
            for (sl = G_pred(n); sl; sl = sl->tail) {//G_pre(n)取n的前驱结点表（控制流里的）
                sn = sl->head;
                if(G_pred(sn)==NULL)
                {
                    continue;
                }
                ci = G_nodeList_intersect(ci, lookupnodeMap(out, sn));
            }
            if(G_NodeList_equal(ci,G_nodes(flow)))
            {
                ci=NULL;
            }
//            if(n->mykey==157)
//            {
//                G_nodeList temp_n=lookupnodeMap(kill,n);
//                for (;temp_n;temp_n=temp_n->tail) {
//                    printf("157 kill %d\n",temp_n->head->mykey);
//                }
//            }
            co = G_NodeList_union( lookupnodeMap(gen,n), G_NodeList_minus(lookupnodeMap(in,n),lookupnodeMap(kill,n)));//返回co=gen(n)+(in(n)-kill(n))

            enternnodeMap(in, n, ci);
            enternnodeMap(out, n, co);
        }
        flag = false;
        int i=0;
//-----------------------------------------------------------------------
//        printf("\nd------------in-----------------d\n");
//        for (fl = G_nodes(flow); fl; fl = fl->tail) {
//
//            G_nodeList temp_ll;
//
//            n = fl->head;
//            li = lookupnodeMap(in, n);
//            lo = lookupnodeMap(out, n);
//
//            printf("\nnode-in:%d:  %s\n",n->mykey,FG_inst(n)->u.OPER.assem);
//            for(temp_ll=li;temp_ll;temp_ll=temp_ll->tail)
//            {
//                printf(",%d",temp_ll->head->mykey);
//            }
//            printf("\nnode-out:%d:\n",n->mykey);
//            for(temp_ll=lo;temp_ll;temp_ll=temp_ll->tail)
//            {
//                printf(",%d",temp_ll->head->mykey);
//            }
//            i++;
//        }
//        printf("\nup------------in-----------------up\n");
//-----------------------------------------------------------------------
        for (fl = G_nodes(flow); fl; fl = fl->tail) {

            G_nodeList temp_ll;

            n = fl->head;
            li = lookupnodeMap(in, n);
            lo = lookupnodeMap(out, n);
            ci = lookupnodeMap(last_in, n);
            co = lookupnodeMap(last_out, n);
            if (!G_NodeList_equal(li, ci) || !G_NodeList_equal(lo, co)) {
                flag = true;
                break;
            }
            i++;
        }

    }

}
AS_instrList use_data_flow(G_graph flow,G_table in,G_table out,AS_instrList iList,G_table expression)
{
    G_node n;
    G_nodeList fl;
    AS_instrList save=iList;
    for(fl =G_nodes(flow) ; fl; fl = fl->tail)
    {
        n=fl->head;
        if(FG_kind(n)==AS_instr_::O_div||FG_kind(n)==AS_instr_::O_mod)
        {
            c_string test=NULL;
            c_string testl=NULL;
            if(FG_kind(n)==AS_instr_::O_div||FG_kind(n)==AS_instr_::O_mod)
            {
//                printf("------\n");
//                printf("%s",test);
//                printf("nodekey:%d\n",n->mykey);
                int count=0;
                int key=9999999;
                G_nodeList l_list=lookupnodeMap(in,n);
                G_nodeList temp=NULL;
                for(;l_list;l_list=l_list->tail)
                {
                    test=lookupstringMap(expression,n);
                    testl=lookupstringMap(expression,l_list->head);
                    if(testl!=NULL)
                    {
                        if(strcmp(test,testl)==0)
                        {
                            count++;
                            if(l_list->head->mykey<key)
                            {
                                key=l_list->head->mykey;
                                temp=G_NodeList(l_list->head,NULL);
                            }

                        }
                    }
                }
                if(temp!= nullptr&&count>0&&(temp->head->mykey<n->mykey))//多定值情况下取key最小的定值结果
                {
                    AS_instrList temp_list=iList;
                    for(;temp_list;temp_list=temp_list->tail)
                    {
                        if(temp_list->head==FG_inst(n))
                        {

                            AS_instrList div_list=(AS_instrList)TAB_look(global_save_div,FG_inst(n));
                            for(;div_list;div_list=div_list->tail)
                            {
                                div_list->head->isDead= true;//删除除法指令前后的不必要move
                            }
                            Temp_tempList t_l1=FG_div_def(n);
                            Temp_tempList t_l2=FG_div_def(temp->head);
//                            printf("%d\n",Temp_number(t_l2->head));
                            char *inst8 = (char *) checked_malloc(sizeof(char) * 40);
                            sprintf(inst8, "\tmov     'd0, 's0\n");//取回返回值
                            AS_instr instl=AS_Move(inst8,t_l1, t_l2,false);
                            temp_list->head=instl;
                        }
                    }
                    //todo rewrite program
                }
            }
//            else if(FG_kind(n)==AS_instr_::O_mod)
//            {
//                G_nodeList temp=G_nodeList_intersect(lookupnodeMap(in,n),lookupstringMap(expression,test));
//                if(temp!= nullptr&&temp->tail==NULL)//仅仅只有一个定值到达此处
//                {
//                    AS_instr inst= FG_inst(n);
//                    Temp_temp dst=FG_div_def(temp->head)->head;
//                    char *inst2 = (char *) checked_malloc(sizeof(char) * 40);
//                    sprintf(inst2, "\tmovhahhah     'd0, 's0\n");//取回返回值
//                    inst=AS_Oper(inst2, L(F_R1(), NULL), L(dst, NULL), NULL,0, false, true);
//                    //todo rewrite program
//                }
//            }
        }
    }
    return save;
}
AS_instrList dataflow(F_frame f, AS_instrList iList,TAB_table div_table)
{
    global_save_div=div_table;
    G_graph flow;
    flow = FG_AssemFlowGraph(iList, f);
//    G_show(stderr, G_nodes(flow), printInst);
    G_table defs = G_empty();
    G_table def_uses = G_empty();
    G_table gen = G_empty();
    G_table kill = G_empty();
    G_table expression=G_empty();
    getdefs(flow,defs,gen,kill,def_uses,expression);
    G_table in = G_empty();
    G_table out = G_empty();
    def_value(flow,gen,kill,def_uses,in,out);//得到in，out，每个指令的定值集合
    iList= use_data_flow(flow,in,out,iList,expression);
    free(defs);
    free(def_uses);
    free(gen);
    free(kill);
    free(expression);
    free(in);
    free(out);
    return iList;

}
