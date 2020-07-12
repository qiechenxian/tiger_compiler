//
// Created by loyx on 2020/7/12.
//

#include <cstdio>
#include "array_init.h"
#include "errormsg.h"

static A_exp getOneValue(A_arrayInitList init_list) {
    if (init_list->tail){
        EM_warning(init_list->tail->head->pos, "excess elements in array initializer");
    }
    if (init_list->head->kind == A_initNode_::A_nestedInit){
        return getOneValue(init_list->head->u.nested);
    } else{
        return init_list->head->u.single;
    }
}

static void paddingInit(A_exp* exp_array, int len, const int* suffix_size, A_arrayInitList init_list){
    int index = 0;
    while (index < len){
        if (init_list->head->kind == A_initNode_::A_singleInit){
            exp_array[index++] = init_list->head->u.single;
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
                exp_array[index] = getOneValue(init_list->head->u.nested);
            } else{
                paddingInit(exp_array+index, stride,
                            suffix_size+sub_point, init_list->head->u.nested);
            }
            index += stride;
        }
        if (init_list->tail){
            init_list = init_list->tail;
        } else{
            while (index<len){
                exp_array[index++] = A_IntExp(init_list->head->pos, 0);
            }
            return;
        }
    }
    EM_warning(init_list->head->pos, "excess elements in array initializer");
}

INIT_initList INIT_InitList(A_expList array_size, A_arrayInitList array_init_list){
    auto init_list = (INIT_initList)checked_malloc(sizeof(INIT_initList_));

    int len = 0, total_len=1;
    int temp[256];
    for (A_expList iter = array_size; iter; iter = iter->tail){
        assert(iter->head->kind == A_exp_::A_intExp);
        temp[len++] = iter->head->u.intExp;
        total_len *= iter->head->u.intExp;
    }
    auto exp_array = (A_exp*)checked_malloc(total_len * sizeof(A_exp));
    int* suffix_size = (int*)checked_malloc((len+1) * sizeof(int));

    suffix_size[len-1] = 1;
    for (int i = 1; i < len; i++){
        suffix_size[len-1-i] = suffix_size[len-i]*temp[len-i];
    }
    suffix_size[len] = -1;

    if (array_init_list){
        paddingInit(exp_array, total_len, suffix_size, array_init_list);
    } else{
        for (int i = 0; i<len; i++) {
            exp_array[i] = A_IntExp(A_Pos(0, 0, 0, 0), 0);
        }
    }

    init_list->array = exp_array;
    init_list->suffix_size = suffix_size;
    return init_list;
}

A_exp INIT_get(INIT_initList init_list, const int* index){
    int trans_index = 0;
    for (int i = 0; index[i] != -1; i++){
        trans_index += index[i] * init_list->suffix_size[i];
    }
    return init_list->array[trans_index];
}

int INIT_getInt(INIT_initList init_list, int* index){
    A_exp el = INIT_get(init_list, index);
    assert(el->kind == A_exp_::A_intExp);
    return el->u.intExp;
}
