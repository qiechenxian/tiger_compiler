//
// Created by loyx on 2020/5/1.
//

#include "../include/semant.h"
#include "../include/env.h"
#include <stdio.h>
#include <string.h>

/** expty构造函数 */
struct expty Expty(Tr_exp exp, TY_ty ty, bool isConst){
    struct expty e;
    e.exp = exp;
    e.ty = ty;
    e.isConst = isConst;
    return e;
}

/** prototypes for functions local to this module */
static Tr_exp transDec(Tr_frame frame, S_table venv, S_table tenv, A_dec d);
static struct expty transExp(S_table venv, S_table tenv, A_exp a);
static struct expty transVar(S_table venv, S_table tenv, A_var v);
static struct expty transStm(Tr_frame frame, S_table venv, S_table tenv, A_stm stm);

static TY_ty actual_ty(TY_ty ty);
static bool is_equal_ty(TY_ty tType, TY_ty eType);
static TY_ty look_ty(S_table tenv, S_symbol sym);
static TY_tyList makeFormalTys(S_table tenv, A_fieldList params);


/** 辅助函数 */
static U_boolList makeFormalEscapeList(A_fieldList params){
    if (!params)
        return NULL;
    U_boolList tail = makeFormalEscapeList(params->tail);
    return U_BoolList(params->head->escape, tail);
}


static TY_ty anonymousArrayTyDec(S_symbol base, int size, S_table tenv) {
    char arrayStr[512]; // 此处限制了数组的最大维数
    strcpy(arrayStr + size, S_getName(base));
    for (int i = 0; i < size; ++i) {
        arrayStr[i] = '$';
    }
    S_symbol arraySym = S_Symbol(arrayStr);
    TY_ty ty = S_look(tenv, arraySym);
    if (ty == NULL){
        ty = S_look(tenv, base);
        for (int i = 0; i < size; ++i)
            ty = TY_Array(ty);
        S_enter(tenv, arraySym, ty);
    }
    return ty;
}

static TY_tyList makeFormalTys(S_table tenv, A_fieldList params){
    /**
     * 将A_fieldList转为TY_tyList，提取形参类型
     * 这是因为抽象语法使用的是A_，而环境中使用的是TY_
     * 且A_中含有其他无关信息，例如变量名id、是否为const等
     * */
     TY_tyList paramTys = NULL;
     TY_tyList tail_p = paramTys;
     for (A_fieldList iter = params; iter; iter = iter->tail){
         TY_ty t;
         if (iter->head->size == NULL){
             t = look_ty(tenv, iter->head->type);
         } else{
             int lens = 0;
             for (A_expList sIter = iter->head->size; sIter; sIter = sIter->tail, lens++);
             assert(lens);
             t = anonymousArrayTyDec(iter->head->type, lens, tenv);
         }
         if (!t){
             EM_error(iter->head->pos, "unknown type name \'%s\'",
                     S_getName(iter->head->type));
         } else{
            if (paramTys){
                tail_p->tail = TY_TyList(t, NULL);
                tail_p = tail_p->tail;
            } else{
                paramTys = TY_TyList(t, NULL);
                tail_p = paramTys;
            }
         }
     }
     return paramTys;
}

static bool is_equal_ty(TY_ty tType, TY_ty eType){
    /**
     * 比较两个TY_ty是否相等
     * */
    TY_ty actualTType = actual_ty(tType);
    int tyKind = actualTType->kind;
    int eKind = eType->kind;
    if (tyKind != TY_struct)
        return tyKind == eKind;
    return actualTType == eType;
}

static TY_ty actual_ty(TY_ty ty){
    /**
     * 在存在typedef的情况下，获取该类型的真实类型
     * */
    if (ty->kind == TY_typedef)
        return actual_ty(ty->u.ty_typedef.ty);
    else
        return ty;
}

static TY_ty look_ty(S_table tenv, S_symbol sym){
    TY_ty t = S_look(tenv, sym);
    if (t)
        return actual_ty(t);
    else
        return NULL;
}


/** semantic check and transform */
F_fragList SEM_transProgram(S_table venv, S_table tenv, A_decList program){

    for (A_decList iter = program; iter; iter = iter->tail)
        transDec(NULL, venv, tenv, iter->head);

    E_envEntry mainEntry = S_look(venv, S_Symbol("main"));
    if (!mainEntry || mainEntry->kind != E_funEntry)
        EM_error(A_Pos(0,0,0,0), "not find main function");

    return NULL;
}

static Tr_exp transDec(Tr_frame frame, S_table venv, S_table tenv, A_dec d){
    switch (d->kind) {
        case A_arrayDec:{

            /** 检查数组基础类型 */
            assert(d->u.array.base);
            TY_ty type = look_ty(tenv, d->u.array.base);
            if (!type){
                EM_error(d->pos, "unknown type \'%s\'",
                         S_getName(d->u.array.base));
            }
            Tr_access var_access;
            if (frame)
                var_access = Tr_allocLocal(frame, true);
            else
                var_access = NULL; // todo global var const var

            /** 根据数组纬度生成对应的类型， 相当于一个匿名typedef variableDec */
            int lens = 0;
            for (A_expList sIter = d->u.array.size; sIter; sIter = sIter->tail, lens++);
            assert(lens);
            TY_ty array_ty = anonymousArrayTyDec(d->u.array.base, lens, tenv);

            /** 填符号表 */
            S_enter(venv, d->u.array.id, E_VarEntry(d->u.array.isConst, var_access, array_ty));

            /** 检查下标 */
            for (A_expList iter = d->u.array.size; iter; iter = iter->tail){
                A_exp subscript = iter->head;
                if (subscript->kind != A_intExp)
                    EM_error(subscript->pos,
                            "size of array \'%s\' has non-integer type",
                            S_getName(d->u.array.id));
                if (subscript->u.intExp < 0)
                    EM_error(subscript->pos,
                            "size of array \'%s\' is negative",
                            S_getName(d->u.array.id));
            }
            // todo translate

            /** 处理初值 */
            if (d->u.array.init != NULL){
                // todo check init value
                return NULL; // todo translate
            }
            return NULL;
        }
        case A_variableDec:{

            /** 检查变量类型 */
            assert(d->u.var.type);
            TY_ty type = look_ty(tenv, d->u.var.type);
            if (!type){
                EM_error(d->pos, "unknown type name \'%s\'",
                        S_getName(d->u.var.type));
            }
//            Tr_access var_access = Tr_allocLocal(frame, d->u.var.escape); todo escape
            Tr_access var_access;
            if (frame)
                var_access = Tr_allocLocal(frame, true);
            else
                var_access = NULL; // todo global var const var
            S_enter(venv, d->u.var.id, E_VarEntry(d->u.var.isConst, var_access, type));

            /** 处理初始值 */
            if (d->u.var.init != NULL) {
                struct expty e = transExp(venv, tenv, d->u.var.init);
                if (!is_equal_ty(type, e.ty)) {
                    EM_error(d->pos, "type error: %s given, expected %s for expression",
                            TY_toString(e.ty),S_getName(d->u.var.type));
                }
                return NULL; /// todo return assign op
            }
            return NULL; /// no init value and not array
        }
        case A_functionDec:{

            /** 处理函数头 */
            TY_ty resultTy = NULL;
            if (d->u.function.result){
                resultTy = S_look(tenv, d->u.function.result);
                if (!resultTy)
                    EM_error(d->pos, "unknown type name \'%s\' for return type",
                            S_getName(d->u.function.result));
            }
            if (!resultTy) resultTy = TY_Void();


                /// 登记函数id，提取形参类型
            TY_tyList formalTys = makeFormalTys(tenv, d->u.function.params);
            Temp_label fun_label = Temp_newLabel();
            U_boolList formal_escape_list = makeFormalEscapeList(d->u.function.params);
            Tr_frame fun_frame = Tr_newFrame(fun_label, formal_escape_list);
            S_enter(venv, d->u.function.id, E_FunEntry(fun_label, formalTys, resultTy));

            /** 处理函数体 */
            E_envEntry funEntry = S_look(venv, d->u.function.id);
            S_beginScope(venv);
            S_beginScope(tenv);
                /// 登记形参
            TY_tyList typeIter = funEntry->u.fun.formals;
            A_fieldList formalIter = d->u.function.params;
            Tr_accessList argsCL = Tr_getFormals(fun_frame);
            for ( ; typeIter; typeIter = typeIter->tail, formalIter = formalIter->tail){
                // 语义支持const形参，虽然语法没支持
                S_enter(venv, formalIter->head->id,
                        E_VarEntry(formalIter->head->isConst, argsCL->head, typeIter->head));
                argsCL = argsCL->tail;
            }
                /// trans body
            struct expty returnValue = transStm(fun_frame, venv, tenv, d->u.function.body);
                /// 检查返回值
            if (!is_equal_ty(funEntry->u.fun.result, returnValue.ty)){
                EM_error(d->u.function.body->pos,
                        "incorrect return type %s, expected %s",
                        TY_toString(returnValue.ty),
                        TY_toString(funEntry->u.fun.result)
                        );
            }
                /// 检查main函数是否有参数，返回值是否为int
            if (d->u.function.id == S_Symbol("main")){
                if (d->u.function.params != NULL)
                    EM_warning(d->pos, "main function should have no args");
                if (!is_equal_ty(returnValue.ty, TY_Int()))
                    EM_warning(d->pos, "main function should return a integer");
            }

            S_endScope(venv);
            S_endScope(tenv);
            /// todo return tr_noExp
            return NULL;
        }
        case A_typedef:
            assert(0); /// todo typedef
    }
}


static struct expty transStm(Tr_frame frame, S_table venv, S_table tenv, A_stm s){
    switch (s->kind) {
        case A_expStm:{
            transExp(venv, tenv, s->u.expStm);
            // todo translate
            return Expty(NULL, TY_Void(), false);
        }
        case A_ifStm:{
            struct expty test, body, elseBody;
            test = transExp(venv, tenv, s->u.ifStm.test);
            if (test.ty->kind != TY_int){
                EM_error(s->u.ifStm.test->pos, "integer required");
            }
            body = transStm(frame, venv, tenv, s->u.ifStm.body);
            if (s->u.ifStm.elseBody){
                elseBody = transStm(frame, venv, tenv, s->u.ifStm.elseBody);
                // todo translate
                return Expty(NULL, TY_Void(), false);
            }
            // todo translate
            return Expty(NULL, TY_Void(), false);
        }
        case A_whileStm:{
            struct expty test = transExp(venv, tenv, s->u.whileStm.test);
            if (test.ty->kind != TY_int){
                EM_error(s->u.whileStm.test->pos, "integer required");
            }
            // todo translate
            struct expty body = transStm(frame, venv, tenv, s->u.whileStm.body);
            return Expty(NULL, TY_Void(), false);
        }
        case A_blockStm:{
            if (!s->u.blockStm)
                return Expty(NULL, TY_Void(), false);

            struct expty returnTy;
            S_beginScope(tenv);
            S_beginScope(venv);

            A_comStmList comStmIter;
            for(comStmIter = s->u.blockStm; comStmIter; comStmIter = comStmIter->tail){
                if (comStmIter->head->const_var_decStm){
                    A_decList decIter = comStmIter->head->const_var_decStm;
                    for ( ; decIter; decIter = decIter->tail){
                        transDec(frame, venv, tenv, decIter->head);
                    }
                }
                if (comStmIter->head->stmSeq){
                    returnTy = transStm(frame, venv, tenv, comStmIter->head->stmSeq);
                    if (returnTy.ty != TY_Void())
                        break; /// 当出现return语句后立即停止检查 todo translate优化
                    // todo translate
                }
            }
            S_endScope(tenv);
            S_endScope(venv);
            return Expty(NULL, returnTy.ty, false);
        }
        case A_assignStm:{
            struct expty var = transVar(venv, tenv, s->u.assignStm.var);

            /// 检查左值是否为const
            if (var.isConst == true){
                EM_error(s->pos,
                        "assignment of read-only variable \'%s\'",
                        S_getName(s->u.assignStm.var->u.simple));
            }
            /// 检查数组是否定位到了元素
            if (var.ty->kind == TY_array){
                EM_error(s->pos,"assignment to expression with array type");
            }

            struct expty exp = transExp(venv, tenv, s->u.assignStm.exp);
            if (!is_equal_ty(var.ty, exp.ty)){
                EM_error(s->u.assignStm.exp->pos,
                        "expression not of expected type %s",
                        TY_toString(var.ty));
            }
            // todo translate
            return Expty(NULL, TY_Void(), false);
        }
        case A_returnStm:{
            if (!s->u.returnStm){
                return Expty(NULL, TY_Void(), false);
            }
            struct expty returnTy = transExp(venv, tenv, s->u.returnStm);
            // todo 自建 translate
            return Expty(NULL, returnTy.ty, false);
        }
        case A_switchStm:{
            struct expty key = transExp(venv, tenv, s->u.switchStm.exp);
            if (key.ty->kind != TY_int){
                EM_error(s->u.switchStm.exp->pos, "integer required");
            }
            A_caseList caseIter;
            for (caseIter = s->u.switchStm.body; caseIter; caseIter = caseIter->tail){
                struct expty body = transStm(frame, venv, tenv, caseIter->head->body);
                // todo translate
            }
            return Expty(NULL, TY_Void(), false);
        }
        case A_breakStm:
        case A_continue:{
            // todo break and continue
            assert(0);
        }
    }
    assert(0);
}

static struct expty transExp(S_table venv, S_table tenv, A_exp a){
    switch (a->kind) {
        case A_logicExp:{
            assert(0);
        }
        case A_varExp:{
            return transVar(venv, tenv, a->u.varExp);
        }
        case A_intExp:{
            return Expty(NULL, TY_Int(), false);
        }
        case A_charExp:{
            return Expty(NULL, TY_Char(), false);
        }
        case A_callExp:{
            E_envEntry funEntry = S_look(venv, a->u.callExp.id);
            /// 检查是否定义函数
            if (funEntry == NULL || funEntry->kind != E_funEntry){
                EM_error(a->pos, "undefined function %s",
                         S_getName(a->u.callExp.id));
                return Expty(NULL, TY_Void(), false);
            }
            /// 检查参数类型
            TY_tyList formals = funEntry->u.fun.formals;
            A_expList argIter = a->u.callExp.args;
            for ( ; argIter; argIter = argIter->tail, formals = formals->tail){
                struct expty argType = transExp(venv, tenv, argIter->head);
                if (!is_equal_ty(argType.ty, formals->head)){
                    EM_warning(argIter->head->pos,
                            "incorrect type %s, expected %s",
                            TY_toString(argType.ty),
                            TY_toString(formals->head));
                    /// todo translate
                }
            }
            /// 检查参数个数
            if (argIter == NULL && formals != NULL){
                EM_error(a->pos, "not enough arguments!");
            } else if (argIter != NULL && formals == NULL){
                EM_error(a->pos, "too many arguments!");
            }
            // todo translate
            return Expty(NULL, actual_ty(funEntry->u.fun.result), false);
        }
        case A_opExp:{
            A_binOp op = a->u.opExp.op;
            struct expty left = transExp(venv, tenv, a->u.opExp.left);
            struct expty right = transExp(venv, tenv, a->u.opExp.right);

            switch (op) {
                case A_add:
                case A_sub:
                case A_mod:
                case A_mul:
                case A_div:{
                    if (left.ty->kind != TY_int && left.ty->kind != TY_char) {
                        EM_error(a->u.opExp.left->pos,
                                "%s expression given for LHS, expected int",
                                TY_toString(left.ty));
                    }
                    if (right.ty->kind != TY_int && right.ty->kind != TY_char) {
                        EM_error(a->u.opExp.right->pos,
                                 "%s expression given for RHS, expected int",
                                 TY_toString(right.ty));
                    }
                    return Expty(NULL, TY_Int(), false);
                    /// todo return op
                }
                case A_eq:
                case A_ne:{
                    if (left.ty->kind == TY_int){
                        if (is_equal_ty(left.ty, right.ty))
                            printf("int == int");
                            /// todo translate
                    }
                    else if (left.ty->kind == TY_char){
                        if (is_equal_ty(left.ty, right.ty))
                            printf("char == char");
                            /// todo translate
                    }
                    else if (left.ty->kind == TY_array){
                        if (TY_array != right.ty->kind)
                            EM_error(a->u.opExp.right->pos,
                                    "%s expression given for RHS, expected %s",
                                    TY_toString(right.ty), TY_toString(left.ty));
                        /// todo translate refEq
                    }
                    else if(left.ty->kind == TY_struct){
                        if (TY_struct != right.ty->kind)
                            EM_error(a->u.opExp.right->pos,
                                    "%S expression given for RHS, expected struct tyepe",
                                    TY_toString(right.ty));
                        /// todo translate refEq
                    } else{
                        EM_error(a->u.opExp.right->pos,
                                "unexpected %s expression in comparison",
                                TY_toString(right.ty));
                    }
                    return Expty(NULL, TY_Int(),false);
                }
                case A_lt:
                case A_le:
                case A_gt:
                case A_ge:{
                    if (left.ty->kind != right.ty->kind){
                        EM_error(a->u.opExp.right->pos,
                                 "%s expression given for RHS, expected %s",
                                 TY_toString(right.ty), TY_toString(left.ty));
                    }

                    if (left.ty->kind == TY_int){
                        /// todo translate
                    } else if (left.ty->kind == TY_char){
                        /// todo translate
                    } else{
                        EM_error(a->u.opExp.right->pos,
                                 "unexpected %s expression in comparison",
                                 TY_toString(right.ty));
                    }
                    return Expty(NULL, TY_Int(), false);
                }
                case A_and:
                case A_not:
                case A_or:
                    /// todo and or not
                    return Expty(NULL, TY_Int(), false);
                default:
                    assert(0);
            }
        }
    }
    assert(0);
}

static struct expty transVar(S_table venv, S_table tenv, A_var v){
    switch (v->kind) {
        case A_simpleVar:{
            E_envEntry varEntry = S_look(venv, v->u.simple);
            if (varEntry == NULL || varEntry->kind != E_varEntry){
                EM_error(v->pos, "undefined variable %s",
                        S_getName(v->u.simple));
                return Expty(NULL, TY_Int(), varEntry->u.var.isConst);
            } else{
                // todo translate
                return Expty(NULL, actual_ty(varEntry->u.var.ty), varEntry->u.var.isConst);
            }
        }
        case A_arrayVar:{
            struct expty id = transVar(venv, tenv, v->u.arrayVar.id);
            // todo translate
            if (id.ty->kind != TY_array){
                EM_error(v->u.arrayVar.id->pos,
                        "subscripted value is neither array nor pointer nor vector");
                return Expty(NULL, TY_Int(), id.isConst);
            } else{
                struct expty index = transExp(venv, tenv, v->u.arrayVar.index);
                if (index.ty->kind != TY_int){
                    EM_error(v->u.arrayVar.index->pos, "index must be a int");
                }
                // todo translate
                return Expty(NULL, actual_ty(id.ty->u.array), id.isConst);
            }
        }
        case A_structVar:{
            assert(0);
            // todo struct type
        }
    }
    assert(0);
}

