//
// Created by loyx on 2020/8/10.
//

#include "loop_trans.h"
#include "ast_visitor.h"
#include "prast.h"
#include "env.h"
#include <vector>
using namespace std;

static A_exp copyExp(A_exp src);
static A_stm copyStm(A_stm Stm);
static A_var copyVar(A_var var);
static A_dec copyDec(A_dec dec);

/************************** 数组提升 **********************************
 *
 * 在循环(或多层循环)中，有一些数组访问可以提升到循环外，避免重复计算地址
 *
 * 例如：
 *  例1：
 *      while(j < limit){
 *          array[...][j][...];
 *      }
 *      <=>
 *      int * hoist_array = array[...];
 *      while(j < limit){
 *          hoist_array[j][...];
 *      }
 *  例2：
 *      while(j < limit){
 *          array[...];
 *      }
 *      <=>
 *      int hoist_array = array[...];
 *      while(j < limit){
 *          hoist_array;
 *      }
 *      array[...] = hoist_array;
 *
 * 注意：
 *  0. 该优化仅针对数组访问
 *  1. array[...] 中的[...]不能包含本循环体内定值的变量，
 *      包括: counter(i,j,k等)；循环体内赋值的对象；循环体内声明的变量；
 *  2. array[j][...]这种情况无法外提到循环外，即j不能存在于数组的第一维
 *  3. array[...][j][...] 中的 j 还可表示任意一个在循环体内定值的变量，但目前不需要考虑那么复杂
 *
 * 该优化可优化的测例有：01_mm, 02_mv, 03_sort
 * 无法优化：00_bitset, 04_spmv
 *********************************************************************/

static void findArrayRef(vector<A_var>& array_ref, A_stm stm);
static bool V_findArrayRef(A_exp e, void* arg);
static bool V_findArrayRefInVar(A_var v, void* arg);
static char* makeHoistName();

static bool LOOP_canHoist(A_stm stm, A_exp& counter){
    if (stm->kind != A_stm_::A_whileStm)
        return false;
    if (stm->u.whileStm.test->kind != A_exp_::A_opExp)
        return false;
    A_exp test_left = stm->u.whileStm.test->u.opExp.left;
    if (test_left->kind == A_exp_::A_varExp && test_left->u.varExp->kind == A_var_::A_simpleVar){
        S_symbol var_id = test_left->u.varExp->u.simple;
        if (var_id == S_Symbol((char*)"j")){
            counter = test_left;
            return true;
        }
    }
    A_exp test_right = stm->u.whileStm.test->u.opExp.right;
    if (test_right->kind == A_exp_::A_varExp && test_right->u.varExp->kind == A_var_::A_simpleVar){
        S_symbol var_id = test_right->u.varExp->u.simple;
        if (var_id == S_Symbol((char*)"j")){
            counter = test_right;
            return true;
        }
    }
    return false;
}


/**
 * 提升数组访问计算，仅做最简单情况
 * @param stm
 */
void LOOP_hoistArrayAccess(A_stm stm, S_table venv){
    A_exp counter;
    if (!LOOP_canHoist(stm, counter))
        return;

    A_stm copy_stm = copyStm(stm);
    vector<A_var> array_ref;
    findArrayRef(array_ref, copy_stm);
    if (array_ref.empty())
        return;




    S_symbol counter_symbol = counter->u.varExp->u.simple;
    vector<A_var> hoist_vars;
    vector<S_symbol> hoist_names;
    vector<int*> hoist_suffix_size;
    for (auto arr_var : array_ref){
        A_var check_var = arr_var;
        A_var hoist_var = nullptr;
        A_var replace_var = nullptr;
        bool can_hoist_array = true;
        int i_pos = 0, deep = 0;
        while (check_var->kind == A_var_::A_arrayVar){
            if (check_var->u.arrayVar.index->kind == A_exp_::A_varExp
            && check_var->u.arrayVar.index->u.varExp->kind == A_var_::A_simpleVar
            && check_var->u.arrayVar.index->u.varExp->u.simple == counter_symbol){
                if (check_var->u.arrayVar.id->kind == A_var_::A_simpleVar){
                    can_hoist_array = false;
                }
                hoist_var = check_var->u.arrayVar.id;
                replace_var = check_var;
                i_pos = deep;
            }
            check_var = check_var->u.arrayVar.id;
            deep ++;
        }

        if (!can_hoist_array){
            continue;
        }

        if (hoist_var == nullptr){
            continue; //todo 暂未实现这种情况
        }

        assert(check_var->kind == A_var_::A_simpleVar);
        auto arrayEntry = (E_envEntry)S_look(venv, check_var->u.simple);
        assert(arrayEntry->u.var.suffix_size);
        int index = deep - i_pos - 1;
        int* suffix_size = arrayEntry->u.var.suffix_size + index;

        /// 就地替换stm上的array
        S_symbol new_name = S_Symbol(makeHoistName());
        replace_var->u.arrayVar.id = A_SimpleVar(nullptr, new_name);

        /// 记录提升的部分和对应的变量名，以及suffixSize
        hoist_suffix_size.push_back(suffix_size);
        hoist_names.push_back(new_name);
        hoist_vars.push_back(copyVar(hoist_var));
    }

    if (hoist_names.empty()){
        return;
    }

#if 0
    fprintf(stderr, "\n_______before hoist________\n");
    pr_stm(stderr, stm, 0);
    fprintf(stderr, "\n_______before hoist________\n");
#endif
#if 0
    fprintf(stderr, "\n_______after replace stm________\n");
    pr_stm(stderr, copy_stm, 0);
    fprintf(stderr, "\n_______after replace stm________\n");
#endif

    /// 构造提升出来的声明语句
    A_decList hoist_dec_list = nullptr, hoist_dec_list_tail = nullptr;
    for (int i = 0; i < hoist_names.size(); i ++){
        A_exp hoist_dec_init = A_VarExp(nullptr, hoist_vars[i]);
        A_dec hoist_dec = A_VariableDec(nullptr,
                                        S_Symbol((char*)"int*"),
                                        hoist_names[i],
                                        hoist_dec_init,
                                        false
                                        );
        hoist_dec->u.var.suffixSize = hoist_suffix_size[i];
        if (hoist_dec_list){
            hoist_dec_list_tail->tail = A_DecList(hoist_dec, nullptr);
            hoist_dec_list_tail = hoist_dec_list_tail->tail;
        } else{
            hoist_dec_list = A_DecList(hoist_dec, nullptr);
            hoist_dec_list_tail = hoist_dec_list;
        }
    }

    /// 替换后的原while语句与新构造的声明语句合成一个新的block语句
    A_comStmList new_block_stm = A_ComStmList(A_ComStm(hoist_dec_list, nullptr), nullptr);
    new_block_stm->tail = A_ComStmList(A_ComStm(nullptr, copy_stm), nullptr);

    stm->kind = A_stm_::A_blockStm;
    stm->u.blockStm = new_block_stm;

#if 0
    fprintf(stderr, "\n_______after hoist________\n");
    pr_stm(stderr, stm, 0);
    fprintf(stderr, "\n_______after hoist________\n");
#endif
}

static int new_hoist_variable_id = 0;
static char* makeHoistName(){
    char* name = (char*)checked_malloc(unroll_new_variable_name_size * sizeof(char));
    sprintf(name, "$new_hoist_var_%d", ++new_hoist_variable_id);
    return name;
}

static int find_array_ref_visit_id = -1;
static void findArrayRef(vector<A_var>& array_ref, A_stm stm){
    vector<A_var> arg_vector;
    if (find_array_ref_visit_id == -1){
//        find_array_ref_visit_id = ASTV_registerExpVisitor(V_findArrayRef);
        find_array_ref_visit_id = ASTV_registerVarVisitor(V_findArrayRefInVar);
    }
    ASTV_iterStm(stm, find_array_ref_visit_id, &arg_vector);
    array_ref = arg_vector;
}

static bool V_findArrayRefInVar(A_var v, void* arg){
    auto* array_ref = (vector<A_var>*)arg;
    if (v->kind == A_var_::A_arrayVar){
        array_ref->push_back(v);
    }
    return true;
}

static bool V_findArrayRef(A_exp e, void* arg){
    auto* array_ref = (vector<A_exp>*)arg;
    if (e->kind == A_exp_::A_varExp)
        if( e->u.varExp->kind == A_var_::A_arrayVar){
            array_ref->push_back(e);
        }
    return true;
}

/***************************** 循环展开 ******************************
 *
 *  在GCC中使用#pragma GCC optimize ("unroll-loops")可强制展开所有循环
 *  但考虑到循环展开优化的本质，现在只展开如下情形的循环:
 *
 *  while( i < limit ){
 *      stm; //只有一条语句
 *      i = i + stride;
 *  }
 *
 * 参考GCC优化模式，可循环展开为：
 *
 *  //第一部分
 *  int limit1 = limit % 8 //可优化为and指令
 *  while( i < limit1){
 *      stm;
 *      i = i+1;
 *  }
 *
 *  //第二部分
 *  while( i < limit){
 *      stm; X8
 *      i = i + 8;
 *  }
 *
 *  GCC中的第一部分是switch语句
 *
 *  该项优化涉及到的测例有：01_mm, 02_mv, 03_sort, 04_spmv
 *
 *******************************************************************/

static const int unroll_times = 2;
static char* makeLimitName();
void findVarRefInExp(vector<A_exp>& exp_refs, A_var var, A_stm stm);
bool isWorth(A_stm pStm);

/**
 * 判断改循环是否可以展开
 *
 *  判断是否优化的要点有：
 *      1. while中语句个数为2
 *      2. while中第二条语句（最后一条）形如 "i = i + 1"
 *      3. while的条件形如 "i < limit"
 *
 * @param s: A_stm
 * @return bool
 */
bool LOOP_canUnrolled(A_stm s, A_exp &counter, A_exp &limit, A_stm& re_stm, A_stm& update_stm){
//    return false;
    if (s->kind != A_stm_::A_whileStm)
        return false;

    A_exp test = s->u.whileStm.test;
    if (test->kind != A_exp_::A_opExp)
        return false;
    int test_left_kind = test->u.opExp.left->kind;
    if (test_left_kind != A_exp_::A_varExp && test_left_kind != A_exp_::A_intExp)
        return false;
    int test_right_kind = test->u.opExp.right->kind;
    if (test_right_kind != A_exp_::A_varExp && test_right_kind != A_exp_::A_intExp)
        return false;

    A_exp exp_list[2];
    exp_list[0] = test->u.opExp.left;
    exp_list[1] = test->u.opExp.right;
    A_binOp test_op = test->u.opExp.op;

    A_stm body = s->u.whileStm.body;
    assert(body->kind == A_stm_::A_blockStm);
    int stm_cnt = 0;
    A_exp buf_counter = nullptr, buf_limit = nullptr;
    for (auto iter = body->u.blockStm; iter; iter = iter->tail){
        stm_cnt++;
        if (stm_cnt > 2)
            return false;
        A_comStm com_stm = iter->head;
        if (com_stm->stmSeq){
            bool find_update_stm = false;
            A_stm body_stm = com_stm->stmSeq;

            if (body_stm->kind != A_stm_::A_assignStm)
                return false;

            if(body_stm->kind == A_stm_::A_assignStm){
                A_var lval = body_stm->u.assignStm.var;
                if (lval->kind == A_var_::A_simpleVar){
                    if (exp_list[0]->kind == A_exp_::A_varExp){
                        if (exp_list[0]->u.varExp->kind == A_var_::A_simpleVar){
                            S_symbol id = exp_list[0]->u.varExp->u.simple;
                            if (id == lval->u.simple){
                                if (test_op != A_le && test_op != A_lt)
                                    return false;
                                buf_counter = exp_list[0];
                                buf_limit = exp_list[1];
                                find_update_stm = true;
                                update_stm = body_stm;
                                if (stm_cnt == 1)
                                    return false;
                            }
                        }
                    }
                    if (exp_list[1]->kind == A_exp_::A_varExp){
                        if (exp_list[1]->u.varExp->kind == A_var_::A_simpleVar){
                            S_symbol id = exp_list[1]->u.varExp->u.simple;
                            if (id == lval->u.simple){
                                if (test_op != A_ge && test_op != A_gt)
                                    return false;
                                buf_counter = exp_list[1];
                                buf_limit = exp_list[0];
                                find_update_stm = true;
                                update_stm = body_stm;
                                if (stm_cnt == 1)
                                    return false;
                            }
                        }
                    }
                }
            }

            if (!find_update_stm){
                re_stm = body_stm;
                if (!isWorth(re_stm)){
                    return false;
                }
            }

        }
    }
    if (buf_counter && buf_limit){
        counter = buf_counter;
        limit =buf_limit;
        if (limit->kind == A_exp_::A_varExp){
//            assert(limit->u.varExp->kind == A_var_::A_simpleVar);
            if (limit->u.varExp->kind != A_var_::A_simpleVar)
                return false;
            char* limit_name = S_getName(limit->u.varExp->u.simple);
            if (limit_name[0] == '$')
                return false;
        }
#if 0
        fprintf(stderr, "\n_______before unroll________\n");
        pr_stm(stderr, s, 0);
        fprintf(stderr, "\n_______before unroll________\n");
#endif
        return true;
    }
    return false;
}

bool isWorth(A_stm pStm) {
    if (pStm->kind == A_stm_::A_assignStm &&
    pStm->u.assignStm.var->kind == A_var_::A_arrayVar &&
    pStm->u.assignStm.var->u.arrayVar.index->kind == A_exp_::A_callExp){
        return false;
    }
    return true;
}


/**
 * 循环展开的主函数
 * @param stm 待展开的while语句AST
 */
void LOOP_unrollWhile(A_stm stm){
    A_exp counter, limit;
    A_stm body_stm, update_stm;
    if (!LOOP_canUnrolled(stm, counter, limit, body_stm, update_stm))
        return;
#if 0
    fprintf(stderr, "\n-----\ncounter:\n");
    pr_exp(stderr, counter, 0);
    fprintf(stderr, "\n----\nlimit:\n");
    pr_exp(stderr, limit, 0);
    fprintf(stderr, "\n----\nbody_stm:\n");
    pr_stm(stderr, body_stm, 0);
    fprintf(stderr, "\n----\nupdate_stm:\n");
    pr_stm(stderr, update_stm, 0);
    fprintf(stderr, "\n----\n");
#endif

    A_comStmList new_block_stm, block_tail;

    ///构造第一部分

    // int limit1 = limit % 8;
    A_exp new_limit_init = A_OpExp(nullptr, copyExp(limit), A_mod, A_IntExp(nullptr, unroll_times));
    S_symbol new_limit_name = S_Symbol(makeLimitName());
    A_dec new_limit = A_VariableDec(
            nullptr,
            S_Symbol((char*)"int"),
            new_limit_name,
            new_limit_init,
            false
            );

    //连接到组合语句中
    A_comStm new_stm_1 = A_ComStm(A_DecList(new_limit, nullptr), nullptr);
    new_block_stm = A_ComStmList(new_stm_1, nullptr);
    block_tail = new_block_stm;

    // while(i < limit1){
    //      stm;
    //      i = i + 1;
    // }
    A_exp origin_test = stm->u.whileStm.test;
    A_exp new_test = A_OpExp(nullptr, nullptr, origin_test->u.opExp.op, nullptr);
    if (origin_test->u.opExp.left->kind == A_exp_::A_varExp
    && counter->u.varExp->u.simple == origin_test->u.opExp.left->u.varExp->u.simple){
        new_test->u.opExp.left = copyExp(counter);
        new_test->u.opExp.right = A_VarExp(nullptr, A_SimpleVar(nullptr, new_limit_name));
    } else{
        new_test->u.opExp.right = copyExp(counter);
        new_test->u.opExp.left = A_VarExp(nullptr, A_SimpleVar(nullptr, new_limit_name));
    }
    A_stm new_while = A_WhileStm(nullptr, new_test, nullptr);
    new_while->u.whileStm.body = copyStm(stm->u.whileStm.body);

    //连接到组合语句中
    A_comStm new_stm_2 = A_ComStm(nullptr, new_while);
    block_tail->tail = A_ComStmList(new_stm_2, nullptr);
    block_tail = block_tail->tail;

    ///构造第二部分

    //while (i < limit){
    A_exp new_test_2 = copyExp(origin_test);
    A_stm new_while_2 = A_WhileStm(nullptr, new_test_2, nullptr);

    A_comStmList new_while_2_body, while_2_body_tail;
    //    stm; X8
    new_while_2_body = A_ComStmList(A_ComStm(nullptr, copyStm(body_stm)), nullptr);
    while_2_body_tail = new_while_2_body;
    for (int repeat_offset = 1; repeat_offset < unroll_times; repeat_offset++){
        A_stm repeat_stm = copyStm(body_stm);

        ///获取counter在repeat_stm中的引用，从而将其修改为 i+1, ..., i+8
        vector<A_exp> counter_ref_in_stm;
        assert(counter->kind == A_exp_::A_varExp);
        findVarRefInExp(counter_ref_in_stm,counter->u.varExp, repeat_stm);

        /// 将counter在repeat_stm上的引用就地修改为 i+1, i+2, ..., i+8
        for (A_exp i_ref : counter_ref_in_stm){
            i_ref->kind = A_exp_::A_opExp;
//            A_OpExp(nullptr, counter, A_add, A_IntExp(nullptr, repeat_offset));
            i_ref->u.opExp.left = copyExp(counter);
            i_ref->u.opExp.op = A_add;
            i_ref->u.opExp.right = A_IntExp(nullptr, repeat_offset);
        }

        /// 将修改后的repeat_stm加入while循环体中
        while_2_body_tail->tail = A_ComStmList(A_ComStm(nullptr, repeat_stm), nullptr);
        while_2_body_tail = while_2_body_tail->tail;
    }

    //    i = i + 8;
    A_stm new_update_stm = copyStm(update_stm);
    A_exp iadd = new_update_stm->u.assignStm.exp;
    A_exp update_left = new_update_stm->u.assignStm.exp->u.opExp.left;
    A_exp update_right = new_update_stm->u.assignStm.exp->u.opExp.right;
    if (update_left->kind == A_exp_::A_varExp
    && counter->u.varExp->u.simple == update_left->u.varExp->u.simple){
        A_exp update_8 = A_OpExp(nullptr, update_right, A_mul, A_IntExp(nullptr, unroll_times));
        iadd->u.opExp.right = update_8;
    } else{
        A_exp update_8 = A_OpExp(nullptr, update_left, A_mul, A_IntExp(nullptr, unroll_times));
        iadd->u.opExp.left = update_8;
    }
    while_2_body_tail->tail = A_ComStmList(A_ComStm(nullptr, new_update_stm), nullptr);

    new_while_2->u.whileStm.body = A_BlockStm(nullptr, new_while_2_body);

    //连接到组合语句中
    block_tail->tail = A_ComStmList(A_ComStm(nullptr, new_while_2), nullptr);

    ///就地修改
    stm->u.blockStm = new_block_stm;
    stm->kind = A_stm_::A_blockStm;
#if 0
    fprintf(stderr, "\n_________after unroll________\n");
    pr_stm(stderr, stm, 0);
    fprintf(stderr, "\n_________after unroll________\n");
#endif
}



static int new_limit_variable_id = 0;
static char* makeLimitName(){
    char* name = (char*)checked_malloc(unroll_new_variable_name_size * sizeof(char));
    sprintf(name, "$new_limit_var_%d", ++new_limit_variable_id);
    return name;
}

/**
 * 在树中获取某个变量的A_exp引用
 */
static bool V_findRefExp(A_exp e, void* arg);
typedef struct refArg_ *refArg;
struct refArg_{
    vector<A_exp> A_refs;
    A_var variable;
};

static int find_ref_visitor_id = -1;

void findVarRefInExp(vector<A_exp>& exp_refs, A_var var, A_stm stm){
    auto ref_arg = (refArg)checked_malloc(sizeof(refArg_));
    ref_arg->A_refs.clear();
    ref_arg->variable = var;
    if (find_ref_visitor_id == -1){
        find_ref_visitor_id = ASTV_registerExpVisitor(V_findRefExp);
    }
    ASTV_iterStm(stm, find_ref_visitor_id, ref_arg);
    exp_refs = ref_arg->A_refs;
}

S_symbol getId(A_var var){
    assert(var->kind == A_var_::A_arrayVar);
    while (var->kind != A_var_::A_simpleVar){
        var = var->u.arrayVar.id;
    }
    return var->u.simple;
}

static bool V_findRefExp(A_exp e, void* arg){
    if (e->kind != A_exp_::A_varExp)
        return true;
    auto ref_arg = (refArg)arg;
    A_var variable = ref_arg->variable;
    A_var ast_var = e->u.varExp;
    if (variable->kind != ast_var->kind)
        return true;
    if (ast_var->kind == A_var_::A_simpleVar){
        if (ast_var->u.simple == variable->u.simple){
            ref_arg->A_refs.push_back(e);
            return false;
        }
    }
    if (ast_var->kind == A_var_::A_arrayVar){
        if (getId(ast_var) == getId(variable)){
            ref_arg->A_refs.push_back(e);
            return true;
        }
    }
    return true;
}

/**
 *  复制ast树
 */


static A_exp copyExp(A_exp src){
    if (src == nullptr)
        return nullptr;
    switch (src->kind) {
        case A_exp_::A_varExp:
            return A_VarExp(nullptr, copyVar(src->u.varExp));
        case A_exp_::A_intExp:
            return A_IntExp(nullptr, src->u.intExp);
        case A_exp_::A_charExp:
            return A_CharExp(nullptr, src->u.charExp);
        case A_exp_::A_callExp:{
            A_exp copy = A_CallExp(nullptr, src->u.callExp.id, nullptr);
            A_expList copy_args = nullptr, ptr = nullptr;
            for (A_expList iter = src->u.callExp.args; iter; iter = iter->tail){
                if (copy_args){
                    ptr->tail = A_ExpList(copyExp(iter->head), nullptr);
                    ptr = ptr->tail;
                } else{
                    copy_args = A_ExpList(copyExp(iter->head), nullptr);
                    ptr = copy_args;
                }
            }
            copy->u.callExp.args = copy_args;
            return copy;
        }
        case A_exp_::A_opExp:
            return A_OpExp(nullptr, copyExp(src->u.opExp.left), src->u.opExp.op, copyExp(src->u.opExp.right));
        case A_exp_::A_stringExp:
            return A_StringExp(nullptr, src->u.stringExp);
    }
    assert(0);
}

static A_stm copyStm(A_stm stm) {
    switch (stm->kind) {
        case A_stm_::A_ifStm:
            if (stm->u.ifStm.elseBody){
                return A_IfStm(nullptr,
                               copyExp(stm->u.ifStm.test),
                               copyStm(stm->u.ifStm.body),
                               copyStm(stm->u.ifStm.elseBody)
                               );
            } else{
                return A_IfStm(nullptr,
                               copyExp(stm->u.ifStm.test),
                               copyStm(stm->u.ifStm.body),
                               nullptr
                               );
            }
        case A_stm_::A_whileStm:{
            return A_WhileStm(nullptr, copyExp(stm->u.whileStm.test), copyStm(stm->u.whileStm.body));
        }
        case A_stm_::A_blockStm:{
            A_comStmList copy_list = nullptr, tail_ptr = nullptr;
            for (auto iter = stm->u.blockStm; iter; iter = iter->tail){
                A_comStm copy_stm = A_ComStm(nullptr, nullptr);
                if (iter->head->const_var_decStm){
                    A_decList copy_dec_list = nullptr, dec_tail_ptr = nullptr;
                    for (auto dec_iter = iter->head->const_var_decStm; dec_iter; dec_iter = dec_iter->tail){
                        if (copy_dec_list){
                            dec_tail_ptr->tail = A_DecList(copyDec(dec_iter->head), nullptr);
                            dec_tail_ptr = dec_tail_ptr->tail;
                        } else{
                            copy_dec_list = A_DecList(copyDec(dec_iter->head), nullptr);
                            dec_tail_ptr = copy_dec_list;
                        }
                    }
                    copy_stm->const_var_decStm = copy_dec_list;
                } else if (iter->head->stmSeq){
                    copy_stm->stmSeq = copyStm(iter->head->stmSeq);
                }
                if (copy_list){
                    tail_ptr->tail = A_ComStmList(copy_stm, nullptr);
                    tail_ptr = tail_ptr->tail;
                } else{
                    copy_list = A_ComStmList(copy_stm, nullptr);
                    tail_ptr = copy_list;
                }
            }
            return A_BlockStm(nullptr, copy_list);
        }
        case A_stm_::A_assignStm:{
            return A_AssignStm(nullptr, copyVar(stm->u.assignStm.var), copyExp(stm->u.assignStm.exp));
        }
        case A_stm_::A_switchStm:
            assert(0);
        case A_stm_::A_returnStm:
            if (stm->u.returnStm){
                return A_ReturnStm(nullptr, copyExp(stm->u.returnStm));
            }
            break;
        case A_stm_::A_breakStm:
            return A_BreakStm(nullptr);
        case A_stm_::A_continue:
            return A_ContinueStm(nullptr);
        case A_stm_::A_expStm:
            return A_ExpStm(nullptr, copyExp(stm->u.expStm));
    }
    assert(0);
}

static A_dec copyDec(A_dec dec) {
    switch (dec->kind) {
        case A_dec_::A_variableDec:{
            return A_VariableDec(nullptr,
                                 dec->u.var.type,
                                 dec->u.var.id,
                                 copyExp(dec->u.var.init),
                                 dec->u.var.isConst
                                 );

        }
        case A_dec_::A_arrayDec:{
            return A_ArrayDec(nullptr,
                              dec->u.array.base,
                              dec->u.array.id,
                              dec->u.array.size,
                              dec->u.array.init,
                              dec->u.array.isConst);
        }
        case A_dec_::A_functionDec:
            assert(0);//不会复制函数声明
        case A_dec_::A_typedef:
            assert(0);
    }
}

static A_var copyVar(A_var var) {
    switch (var->kind) {
        case A_var_::A_simpleVar:
            return A_SimpleVar(nullptr, var->u.simple);
        case A_var_::A_arrayVar:
            return A_ArrayVar(nullptr, copyVar(var->u.arrayVar.id), copyExp(var->u.arrayVar.index));
        case A_var_::A_structVar:
            assert(0);
    }
}



