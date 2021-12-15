//
// Created by zcjsh on 2020/7/10.
//

#ifndef COMPILER_LOYX__PRAST_H
#define COMPILER_LOYX__PRAST_H

#include <cstdio>
#include "util.h"
#include "symbol.h"
#include "ast.h"


void pr_decList(FILE *out, A_decList v, int d);
void pr_var(FILE *out, A_var v, int d);
void pr_dec(FILE *out, A_dec v, int d);
void pr_stm(FILE *out, A_stm v, int d);
void pr_exp(FILE *out, A_exp v, int d);
void pr_ty(FILE *out, A_ty v, int d);
void pr_field(FILE *out, A_field v, int d);
void pr_fieldList(FILE *out, A_fieldList v, int d);
void pr_expList(FILE *out, A_expList v, int d);
void pr_comStm(FILE *out, A_comStm v, int d);
void pr_comStmList(FILE *out, A_comStmList v, int d);
void pr_case(FILE *out, A_case v, int d);
void pr_caseList(FILE *out, A_caseList v, int d);
void pr_initNode(FILE *out, A_initNode v, int d);
void pr_arrayInitList(FILE *out, A_arrayInitList v, int d);

#endif //UNTITLED_PRAST_H
