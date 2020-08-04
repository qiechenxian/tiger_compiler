//
// Created by �ҳ��� on 22/7/2020.
//

/*
 * printtree.c - functions to print out intermediate representation (IR) trees.
 *
 */
#include <cstdio>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "print_ir_tree.h"
#include "frame.h"

/* local function prototype */
static void pr_tree_exp(FILE *out, T_exp exp, int d);

static void indent(FILE *out, int d) {
    int i;
    for (i = 0; i <= d; i++) fprintf(out," ");
}

static char bin_oper[][12] = {
        "T_add", "T_sub", "T_mul", "T_div",
        "T_lshift", "T_rshift", "T_arshift",
        "T_xor", "T_mod"};

static char rel_oper[][12] = {
        "T_lt", "T_le", "T_gt", "T_ge", "T_eq", "T_ne", "T_not", "T_and", "T_or"};

static void pr_stm(FILE *out, T_stm stm, int d) {
    if (stm == nullptr) {
        fprintf(out, "nullptr");
        return;
    }
    switch (stm->kind) {
        case T_stm_::T_SEQ:
            indent(out, d);
            fprintf(out, "SEQ(\n");
            pr_stm(out, stm->u.SEQ.left, d + 1);
            fprintf(out, ",\n");
            pr_stm(out, stm->u.SEQ.right, d + 1);
            fprintf(out, ")");
            break;
        case T_stm_::T_LABEL:
            indent(out, d);
            fprintf(out, "LABEL %s", S_name(stm->u.LABEL));
            break;
        case T_stm_::T_JUMP:
            indent(out, d);
            fprintf(out, "JUMP(\n");
            pr_tree_exp(out, stm->u.JUMP.exp, d + 1);
            fprintf(out, ")");
            break;
        case T_stm_::T_CJUMP:
            indent(out, d);
            fprintf(out, "CJUMP(%s,\n", rel_oper[stm->u.CJUMP.op]);
            pr_tree_exp(out, stm->u.CJUMP.left, d + 1);
            fprintf(out, ",\n");
            pr_tree_exp(out, stm->u.CJUMP.right, d + 1);
            fprintf(out, ",\n");
            indent(out, d + 1);
            fprintf(out, "%s,", S_name(stm->u.CJUMP.trues));
            fprintf(out, "%s", S_name(stm->u.CJUMP.falses));
            fprintf(out, ")");
            break;
        case T_stm_::T_MOVE:
            indent(out, d);
            fprintf(out, "MOVE(\n");
            pr_tree_exp(out, stm->u.MOVE.dst, d + 1);
            fprintf(out, ",\n");
            pr_tree_exp(out, stm->u.MOVE.src, d + 1);
            fprintf(out, ")");
            break;
        case T_stm_::T_EXP:
            indent(out, d);
            fprintf(out, "EXP(\n");
            pr_tree_exp(out, stm->u.EXP, d + 1);
            fprintf(out, ")");
            break;
        case T_stm_::T_STMFD:
        case T_stm_::T_LDMFD:
        default:

            // TODO 支持操作
            fprintf(out, "not support\n");
            break;
    }
}

static void pr_tree_exp(FILE *out, T_exp exp, int d) {
    switch (exp->kind) {
        case T_exp_::T_BINOP:
            indent(out, d);
            fprintf(out, "BINOP(%s,\n", bin_oper[exp->u.BINOP.op]);
            pr_tree_exp(out, exp->u.BINOP.left, d + 1);
            fprintf(out, ",\n");
            pr_tree_exp(out, exp->u.BINOP.right, d + 1);
            fprintf(out, ")");
            break;
        case T_exp_::T_MEM:
            indent(out, d);
            fprintf(out, "MEM");
            fprintf(out, "(\n");
            pr_tree_exp(out, exp->u.MEM, d + 1);
            fprintf(out, ")");
            break;
        case T_exp_::T_TEMP:
            indent(out, d);
            fprintf(out, "TEMP t%s", Temp_look(Temp_name(), exp->u.TEMP));
            break;
        case T_exp_::T_ESEQ:
            indent(out, d);
            fprintf(out, "ESEQ(\n");
            pr_stm(out, exp->u.ESEQ.stm, d + 1);
            fprintf(out, ",\n");
            pr_tree_exp(out, exp->u.ESEQ.exp, d + 1);
            fprintf(out, ")");
            break;
        case T_exp_::T_NAME:
            indent(out, d);
            fprintf(out, "NAME %s", S_name(exp->u.NAME));
            break;
        case T_exp_::T_CONST:
            indent(out, d);
            fprintf(out, "CONST %d", exp->u.CONST);
            break;
        case T_exp_::T_CALL: {
            T_expList args = exp->u.CALL.args;
            indent(out, d);
            fprintf(out, "CALL(\n");
            pr_tree_exp(out, exp->u.CALL.fun, d + 1);
            for (; args; args = args->tail) {
                fprintf(out, ",\n");
                pr_tree_exp(out, args->head, d + 2);
            }
            fprintf(out, ")");
        }
            break;

        case T_exp_::T_RELOP:
        default:
            fprintf(out, "not support\n");
            break;

    } /* end of switch */
}

void printStmList(FILE *out, F_fragList stmList) {
    fprintf(out, "\n↓---------------IR TREE BEFORE CANNO PRINTF START---------------↓\n");
    if (out == nullptr) {
        fprintf(out, "File open false");
    }
    for (; stmList; stmList = stmList->tail) {
        pr_stm(out, stmList->head->u.proc.body, 0);
    }
    fprintf(out, "\n↑---------------IR TREE BEFORE CANNO PRINTF OVER---------------↑\n");
}

void printcannoList(FILE *out, T_stmList stmList) {
    fprintf(out, "\n↓---------------IR TREE AFTER CANNO PRINTF START---------------↓\n");
    if (out == nullptr) {
        fprintf(out, "File open false");
    }
    for (; stmList; stmList = stmList->tail) {
        pr_stm(out, stmList->head, 0);
        fprintf(out, "\n----dived line----\n");
    }
    fprintf(out, "\n↑---------------IR TREE AFTER CANNO PRINTF OVER---------------↑\n");
}