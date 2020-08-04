//
// Created by loyx on 2020/7/12.
//

#include "array_init.h"
#include <utility>
#include <vector>
#include <algorithm>
#include "errormsg.h"
using namespace std;

typedef pair<int, A_exp> intExpPair;

static A_exp getOneValue(A_arrayInitList init_list) {
    /**
     * paddingInit的辅助函数
     */
    if (init_list->tail){
//        EM_warning(init_list->tail->head->pos, "excess elements in array initializer");
    }
    if (init_list->head->kind == A_initNode_::A_nestedInit){
        return getOneValue(init_list->head->u.nested);
    } else{
        return init_list->head->u.single;
    }
}

static void paddingInit(vector<intExpPair>& exp_array, int pos, int len, const int* suffix_size, A_arrayInitList init_list){
    /**
     * INIT_InitList的辅助函数.
     * 用于填充数组声明时的初值
     * 例如：将int a[4][2] = {{},{3},{5},{7,8}}的初值填充为{{0,0}, {3,0}, {5,0}, {7,8}}
     * 填充行为与C语言保持一致
     */
    int index = 0;
    while (index < len){
        if (init_list->head->kind == A_initNode_::A_singleInit){
//            exp_array[index++] = init_list->head->u.single;
            exp_array.emplace_back(pos++, init_list->head->u.single);
            index++;
        } else{
            int stride = 0, remainder, sub_point;
            for (sub_point = 0; suffix_size[sub_point] != -1; sub_point++){
                remainder = index%suffix_size[sub_point];
                if (remainder == 0) {
                    stride = suffix_size[sub_point];
                    break;
                }
            }
            assert(stride);
            if (stride == 1){
//                exp_array[index] = getOneValue(init_list->head->u.nested);
                exp_array.emplace_back(pos, getOneValue(init_list->head->u.nested));
            } else{
                paddingInit(exp_array/*+index*/, pos+index, stride,
                            suffix_size+sub_point, init_list->head->u.nested);
            }
            index += stride;
            pos += stride;
        }
        if (init_list->tail){
            init_list = init_list->tail;
        } else{
//            while (index<len){
//                exp_array[index++] = A_IntExp(init_list->head->pos, 0);
//            }
            return;
        }
    }
//    EM_warning(init_list->head->pos, "excess elements in array initializer");
}

bool pairCmp(intExpPair a, intExpPair b){
    return a.first < b.first;
}

INIT_initList INIT_InitList(A_expList array_size, A_arrayInitList array_init_list){
    auto init_list = (INIT_initList)checked_malloc(sizeof(INIT_initList_));
    vector<intExpPair> exp_vector;

    int len = 0, total_len=1;
    int temp[256];
    for (A_expList iter = array_size; iter; iter = iter->tail){
        assert(iter->head->kind == A_exp_::A_intExp);
        temp[len++] = iter->head->u.intExp;
        total_len *= iter->head->u.intExp;
    }

    int* suffix_size = (int*)checked_malloc((len+1) * sizeof(int));
    suffix_size[len-1] = 1;
    for (int i = 1; i < len; i++){
        suffix_size[len-1-i] = suffix_size[len-i]*temp[len-i];
    }
    suffix_size[len] = -1;

//    auto exp_array = (A_exp*)checked_malloc(total_len * sizeof(A_exp));
    if (array_init_list){
        paddingInit(exp_vector, 0, total_len, suffix_size, array_init_list);
    } else{
//        for (int i = 0; i<len; i++) {
//            exp_array[i] = A_IntExp(A_Pos(0, 0, 0, 0), 0);
//        }
        exp_vector.emplace_back(total_len, A_IntExp(nullptr, 0));
    }
    sort(exp_vector.begin(), exp_vector.end(), pairCmp);
    if (exp_vector.end()->first != total_len){
        exp_vector.emplace_back(total_len, A_IntExp(nullptr, 0));
    }
    init_list->array = exp_vector;
    init_list->suffix_size = suffix_size;
    init_list->total_size = suffix_size[0] * temp[0];
    return init_list;
}

A_exp getFromVec(vector<intExpPair> &array, int index){
    for (auto & iter : array){
        if (index == iter.first){
            return iter.second;
        }
        else if (index < iter.first){
            return A_IntExp(nullptr, 0);
        }
    }
    return A_IntExp(nullptr, 0);
}

A_exp INIT_get(INIT_initList init_list, const int* index){
    int trans_index = 0;
    for (int i = 0; index[i] != -1; i++){
        trans_index += index[i] * init_list->suffix_size[i];
    }
//    return init_list->array[trans_index];
    return getFromVec(init_list->array, trans_index);
}

int INIT_getInt(INIT_initList init_list, int* index){
    A_exp el = INIT_get(init_list, index);
    assert(el->kind == A_exp_::A_intExp);
    return el->u.intExp;
}
U_pairList INIT_shrinkInitList(INIT_initList init_list){
    /**
     * 服务于全局数组的初值形式，initList中的exp都应为intExp
     */
    U_pairList shrink_array = nullptr;
    U_pairList tail_ptr = shrink_array;
    int pre = 0;
    for (auto & iter : init_list->array){
        int zeros = iter.first - pre - 1;
        pre = iter.first;
        U_intPair pair;
        if (zeros > 0){
            pair = U_IntPair(zeros, 0);
        } else{
            pair = U_IntPair(1, iter.second->u.intExp);
        }
        if (not shrink_array){
            shrink_array = U_PairList(pair, nullptr);
            tail_ptr = shrink_array;
        } else{
            tail_ptr->tail = U_PairList(pair, nullptr);
            tail_ptr = tail_ptr->tail;
        }
    }
    return shrink_array;
}
