//
// Created by loyx on 2020/7/12.
//
/**
 * array_init 模块提供一个数据结构和3个有关操作
 *
 * 数据结构：INIT_initList
 *  功能：1.保存填充后的数组初值。2.保存数组纬度大小的后缀和
 *
 * 构造函数：INIT_InitList
 *  根据数组声明时的纬度信息、初值信息 生成填充后的初值
 *
 * 查询函数1：INIT_get
 *  查询参数init_list中index位置的数据.
 *  例如查询init_list[2][3], 调用INIT_get(init_list, index).其中index为int数组，内容为：[2,3,-1]
 *  !!注意!!:index结尾必须为-1
 *
 * 查询函数2：INIT_getInt
 *  针对常数数组查值
 *  !!注意!!:index结尾必须为-1
 */

#ifndef COMPILER_2020_ARRAY_INIT_H
#define COMPILER_2020_ARRAY_INIT_H

#include "ast.h"

typedef struct INIT_initList_ *INIT_initList;
struct INIT_initList_{
    A_exp* array;
    int* suffix_size;
};
INIT_initList INIT_InitList(A_expList array_size, A_arrayInitList array_init_list);

A_exp INIT_get(INIT_initList init_list, const int* index);
int INIT_getInt(INIT_initList init_list, int* index);


#endif //COMPILER_2020_ARRAY_INIT_H
