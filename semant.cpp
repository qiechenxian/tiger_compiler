//
// Created by loyx on 2020/5/1.
//

#include "semant.h"
#include "env.h"
#include <cstdio>
#include <cstring>

/**
 * expty构造函数
 *
 * expty的作用是：在各transXX函数中传递信息。这个名字不太好
 * */
struct expty Expty(Tr_exp exp, TY_ty ty){
    struct expty e{};
    e.exp = exp;
    e.ty = ty;
    e.isConst = false;
    return e;
}

/** prototypes for functions local to this module */
static Tr_exp transDec(Tr_frame frame, S_table venv, S_table tenv, A_dec d,Tr_exp l_break,Tr_exp l_continue);
static struct expty transExp(S_table venv, S_table tenv, A_exp a,Tr_exp l_break,Tr_exp l_continue);
static struct expty transVar(S_table venv, S_table tenv, A_var v,Tr_exp l_break,Tr_exp l_continue);
static struct expty transStm(Tr_frame frame, S_table venv, S_table tenv, A_stm stm,Tr_exp l_break,Tr_exp l_continue);

static TY_ty actual_ty(TY_ty ty);
static bool is_equal_ty(TY_ty tType, TY_ty eType);
static TY_ty look_ty(S_table tenv, S_symbol sym);
static TY_tyList makeFormalTys(S_table tenv, A_fieldList params);



/** 辅助函数 */
static int calculate(A_binOp op, int left, int right) {
    /**
     * 该函数服务于transExp中A_varOp情况。
     * 作用：计算常量表达式结果
     */
    switch (op) {
        case A_or:
            return left || right;
        case A_and:
            return left && right;
        case A_not:
            return !right; /// 文法中将！exp保存为 0 NOT EXP
        case A_lt:
            return left < right;
        case A_le:
            return left <= right;
        case A_gt:
            return left > right;
        case A_ge:
            return left >= right;
        case A_eq:
            return left == right;
        case A_ne:
            return left != right;
        case A_add:
            return left + right;
        case A_sub:
            return left - right;
        case A_mul:
            return left * right;
        case A_div:
            return left / right;
        case A_mod:
            return left % right;
    }
}

static int getConstValue(S_table venv, A_exp a){
    /**
     * 此函数是服务transExp中A_varExp情况的辅助函数
     * @param venv: 值环境符号表
     * @param a: 特指A_varExp
     * @return: const变量转化为int
     */
    A_var const_var = a->u.varExp;
    int access_index[256];
    int index_len = 0;
    while (const_var->kind == A_var_::A_arrayVar){
        A_exp exp_index = const_var->u.arrayVar.index;
        assert(exp_index->kind == A_exp_::A_intExp);
        access_index[index_len++] = exp_index->u.intExp;
        const_var = const_var->u.arrayVar.id;
    }
    for (int i = 0; i < index_len/2; i++){
        int temp = access_index[i];
        access_index[i] = access_index[index_len-i-1];
        access_index[index_len-i-1] = temp;
    }
    auto const_var_entry = (E_envEntry)S_look(venv, const_var->u.simple);
    if (index_len){
        access_index[index_len] = -1;
        return INIT_getInt(const_var_entry->u.var.cValues->u.arrayValue, access_index);
    } else{
        return const_var_entry->u.var.cValues->u.singleValue;
    }
}

static U_boolList makeFormalEscapeList(A_fieldList params){
    if (!params)
        return nullptr;
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
    auto ty = (TY_ty)S_look(tenv, arraySym);
    if (ty == nullptr){
        ty = (TY_ty)S_look(tenv, base);
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
     TY_tyList paramTys = nullptr;
     TY_tyList tail_p = paramTys;
     for (A_fieldList iter = params; iter; iter = iter->tail){
         TY_ty t;
         if (iter->head->size == nullptr){
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
                tail_p->tail = TY_TyList(t, nullptr);
                tail_p = tail_p->tail;
            } else{
                paramTys = TY_TyList(t, nullptr);
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
    if (tyKind != TY_ty_::TY_struct)
        return tyKind == eKind;
    return actualTType == eType;
}

static TY_ty actual_ty(TY_ty ty){
    /**
     * 在存在typedef的情况下，获取该类型的真实类型
     * */
    if (ty->kind == TY_ty_::TY_typedef) {
        return actual_ty(ty->u.ty_typedef.ty);
        assert(0); // 我们没有typedef关键词， 不应该运行到此处。 --loyx 2020/6/25
    }
    else
        return ty;
}

static TY_ty look_ty(S_table tenv, S_symbol sym){
    auto t = (TY_ty)S_look(tenv, sym);
    if (t)
        return actual_ty(t);
    else
        return nullptr;
}


/** semantic check and transform */
F_fragList SEM_transProgram(S_table venv, S_table tenv, A_decList program){

    for (A_decList iter = program; iter; iter = iter->tail)
        transDec(Tr_root_frame(), venv, tenv, iter->head, nullptr, nullptr);

    auto mainEntry = (E_envEntry)S_look(venv, S_Symbol((char *)"main"));
    if (!mainEntry || mainEntry->kind != E_envEntry_::E_funEntry)
        EM_error(A_Pos(0,0,0,0), "not find main function");

    return Tr_getResult();
}

static Tr_exp transDec(Tr_frame frame, S_table venv, S_table tenv, A_dec d,Tr_exp l_break,Tr_exp l_continue){
    switch (d->kind) {
        case A_dec_::A_arrayDec:{

            /** 检查数组基础类型 */
            assert(d->u.array.base);
            auto type = (TY_ty)look_ty(tenv, d->u.array.base);
            if (!type){
                EM_error(d->pos, "unknown type \'%s\'",
                         S_getName(d->u.array.base));
            }

            /** 根据数组纬度生成对应的类型， 相当于一个匿名typedef variableDec */
            int lens = 0;
            for (A_expList sIter = d->u.array.size; sIter; sIter = sIter->tail, lens++);
            assert(lens);
            TY_ty array_ty = anonymousArrayTyDec(d->u.array.base, lens, tenv);

            /** 检查下标 */
            int array_total_size = 1; // 数组全长，供之后使用
            for (A_expList iter = d->u.array.size; iter; iter = iter->tail){
                /**
                 * 下标要求都是可求值非负的constExp
                 * transExp函数保证将constExp表达式优化成单个intExp，并就地修改AST结构
                 */
                A_exp subscript = iter->head;
                transExp(venv, tenv, subscript,l_break,l_continue); /// 转化constExp表达式
                if (subscript->kind != A_exp_::A_intExp) /// 如果下标合法，transExp保证AST被就地修改为intExp
                    EM_error(subscript->pos,
                            "size of array \'%s\' has non-integer type",
                            S_getName(d->u.array.id));
                if (subscript->u.intExp < 0)
                    EM_error(subscript->pos,
                            "size of array \'%s\' is negative",
                            S_getName(d->u.array.id));

                array_total_size *= subscript->u.intExp;
            }

            Tr_access var_access;
            if (frame) {
                var_access = Tr_allocLocal(frame, true, array_total_size);
                /// 这里没有考虑数组特别大以至于栈放不下的情况
            }
            else {
                var_access = Tr_allocGlobal(d->u.array.id);
            }

            /// 数组声明，在符号表中的基本条目
            E_envEntry arrayEntry = E_VarEntry(d->u.array.isConst, var_access, array_ty);

            /** 处理初值 */
            if (d->u.array.init != nullptr){

                INIT_initList init_list = INIT_InitList(d->u.array.size, d->u.array.init);//这下弄中间代码舒服了

                //将init_list中的A_EXP转换为中间代码形式并保存
                auto init_list_tr=(Tr_INIT_initList)checked_malloc(sizeof(Tr_INIT_initList_));
                init_list_tr->array_length=init_list->total_size;
                init_list_tr->array=(Tr_exp*)checked_malloc(init_list->total_size*sizeof(Tr_exp));
                for(int i=0;i < init_list->total_size;i++)
                {
                    struct expty temp=transExp(venv,tenv,init_list->array[i],l_break,l_continue);;
                    init_list_tr->array[i]=temp.exp;
                }

                if (not frame){
                    /// 全局数组的frag处理

                    /**
                     * 全局数组的初始值中必须都为常数。
                     * 在上边的下标检查中可以保证如果是常数都会被优化为int
                     * 但没有检查是否有非常数表达式。官方保证没有
                     * -- loyx 2020/7/23
                     */
                    Tr_newArrayFrag(
                            Tr_getGlobalLabel(var_access),
                            array_total_size,
                            INIT_shrinkInitList(init_list)
                            );
                    S_enter(venv, d->u.array.id, arrayEntry);
                    return Tr_nopExp();
                }

                if (d->u.array.isConst){
                    /**
                     * 常量数组初值处理
                     */
                    arrayEntry->u.var.cValues = E_ArrayValue(init_list);
                    S_enter(venv, d->u.array.id, arrayEntry);
                    return Tr_init_array(var_access, init_list_tr); // todo 优化项：当常数数组只有常量访问时，无需初始化
                } else{
                    /**
                     * 非常量数组初值处理
                     */
                    S_enter(venv, d->u.array.id, arrayEntry);
                    return Tr_init_array(var_access, init_list_tr);
                }
            }

            /** 无初值情况*/

            if (not frame){
                /// 全局数组无初始化，默认全0

                U_intPair pair = U_IntPair(array_total_size, 0);
                U_pairList shrink_zeros = U_PairList(pair, nullptr);

                Tr_newArrayFrag(
                        Tr_getGlobalLabel(var_access),
                        array_total_size,
                        shrink_zeros
                        );
                S_enter(venv, d->u.array.id, arrayEntry);
                return Tr_nopExp();
            }

            if (d->u.array.isConst){
                /**
                 * 常数数组无初始化，默认全0
                 */
                INIT_initList null_init = INIT_InitList(d->u.array.size, nullptr);
                arrayEntry->u.var.cValues = E_ArrayValue(null_init);

                auto init_list_tr=(Tr_INIT_initList)checked_malloc(sizeof(Tr_INIT_initList_));
                init_list_tr->array_length=null_init->total_size;
                init_list_tr->array=(Tr_exp*)checked_malloc(null_init->total_size*sizeof(Tr_exp));
                for(int i=0;i < null_init->total_size;i++)
                {
                    struct expty temp=transExp(venv,tenv,null_init->array[i],l_break,l_continue);;
                    init_list_tr->array[i]=temp.exp;
                }

                return Tr_init_array(var_access, init_list_tr);
            }

            S_enter(venv, d->u.array.id, arrayEntry);
            return Tr_nopExp(); /// 如果是普通数组则只需要在栈内分配空间即可
        }
        case A_dec_::A_variableDec:{

            /** 检查变量类型 */
            assert(d->u.var.type);
            TY_ty type = look_ty(tenv, d->u.var.type);
            if (!type){
                EM_error(d->pos, "unknown type name \'%s\'",
                        S_getName(d->u.var.type));
            }
            Tr_access var_access;
            if (frame){
                var_access = Tr_allocLocal(frame, true, 1);
            }
            else{
                var_access = Tr_allocGlobal(d->u.var.id);
            }

            /// 该变量声明的在符号表中的基本条目
            E_envEntry varEntry = E_VarEntry(d->u.var.isConst, var_access, type);

            /** 处理初始值 */
            if (d->u.var.init != nullptr) {

                /// 检查初值与声明类型是否相符
                struct expty e = transExp(venv, tenv, d->u.var.init,l_break,l_continue);
                if (!is_equal_ty(type, e.ty)) {
                    EM_error(d->pos, "type error: %s given, expected %s for expression",
                            TY_toString(e.ty),S_getName(d->u.var.type));
                }

                if (not frame){
                    /// 无frame表示此变量为全局变量

                    // 全局变量的初值必须为常数表达式，此处前端保证优化为常数
                    assert(d->u.var.init->kind == A_exp_::A_intExp);
                    Tr_newIntFrag(Tr_getGlobalLabel(var_access),  d->u.var.init->u.intExp);

                    /// 对于全局变量的翻译已归frag管理，无需翻译为ir
                    S_enter(venv, d->u.var.id, varEntry);
                    return Tr_nopExp();
                }

                if (d->u.var.isConst){
                    /**
                     * 如果是常量声明，需要判断初值是否为constExp
                     * transExp函数保证，所有constExp都被转换为intExp。
                     * 并且修改AST结构(此处为d->u.var.init)
                     */
                    if (d->u.var.init->kind != A_exp_::A_intExp){
                        EM_error(d->u.var.init->pos,
                                 "The initial value of the constant should be ConstExp");
                    }
                    /// 在符号表中创建该const变量的条目，特别保存const的初始值，供之后使用。
                    varEntry->u.var.cValues = E_SingleValue(d->u.var.init->u.intExp); // transExp保证正确
                    S_enter(venv, d->u.var.id, varEntry);
                    return Tr_nopExp(); /// 常量声明应该到此为止 todo 验证正确性 是否需要assign
                } else{
                    /**
                     * 非常量声明的初值处理
                     */
                    S_enter(venv, d->u.var.id, varEntry);
                    return Tr_assign(Tr_simpleVar(var_access),e.exp); /// todo return assign op
                }
            }

            /** 无初值情况 */

            /// 无初值的全局变量
            if (not frame){
                Tr_newIntFrag(Tr_getGlobalLabel(var_access), 0);
                S_enter(venv, d->u.var.id, varEntry);
                return Tr_nopExp();
            }

            /// 无初值的const变量
            if (d->u.var.isConst) varEntry->u.var.cValues = E_SingleValue(0); /// 无初值常量默认0

            S_enter(venv, d->u.var.id, varEntry);
            return Tr_nopExp(); // no init value just allocl momery
        }
        case A_dec_::A_functionDec:{

            /** 处理函数头 */
            TY_ty resultTy = nullptr;
            if (d->u.function.result){
                resultTy = (TY_ty)S_look(tenv, d->u.function.result);
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
            auto funEntry = (E_envEntry)S_look(venv, d->u.function.id);
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
            struct expty returnValue = transStm(fun_frame, venv, tenv, d->u.function.body,l_break,l_continue);
                /// 检查返回值
            if (returnValue.ty && !is_equal_ty(funEntry->u.fun.result, returnValue.ty)){
                EM_error(d->u.function.body->pos,
                        "incorrect return type %s, expected %s",
                        TY_toString(returnValue.ty),
                        TY_toString(funEntry->u.fun.result)
                        );
            }
                /// 检查main函数是否有参数，返回值是否为int
            if (d->u.function.id == S_Symbol((char *)"main")){
                if (d->u.function.params != nullptr)
                    EM_warning(d->pos, "main function should have no args");
                if (!is_equal_ty(returnValue.ty, TY_Int()))
                    EM_warning(d->pos, "main function should return a integer");
            }
            Tr_procEntryExit(fun_frame, returnValue.exp, argsCL);//将函数栈帧与函数体记录，构建中间代码阶段反回的片段列表
            S_endScope(venv);
            S_endScope(tenv);
            return Tr_nopExp();
            //todo funcdec which have todo after reg alloc
        }
        case A_dec_::A_typedef:
            assert(0); /// todo typedef
    }
}


static struct expty transStm(Tr_frame frame, S_table venv, S_table tenv, A_stm s,Tr_exp l_break,Tr_exp l_continue){
    switch (s->kind) {
        case A_stm_::A_expStm:{
            expty expty_msg = transExp(venv, tenv, s->u.expStm,l_break,l_continue);
            // todo translate
            return Expty(expty_msg.exp, expty_msg.ty);
        }
        case A_stm_::A_ifStm:{
            struct expty test{}, body{}, elseBody{};
            test = transExp(venv, tenv, s->u.ifStm.test,l_break,l_continue);
            if (test.ty->kind != TY_ty_::TY_int){
                EM_error(s->u.ifStm.test->pos, "integer required");
            }
            body = transStm(frame, venv, tenv, s->u.ifStm.body,l_break,l_continue);
            if (s->u.ifStm.elseBody){
                elseBody = transStm(frame, venv, tenv, s->u.ifStm.elseBody,l_break,l_continue);
                // todo translate
                if (body.ty != elseBody.ty){
                    EM_error(s->pos, "return different type in if statement");
                }
                return Expty(Tr_if_else(test.exp,body.exp,elseBody.exp), body.ty);
            }
            // todo translate
            return Expty(Tr_if_else(test.exp,body.exp, nullptr), body.ty);
        }
        case A_stm_::A_whileStm:{
            struct expty test = transExp(venv, tenv, s->u.whileStm.test,l_break,l_continue);
            if (test.ty->kind != TY_ty_::TY_int){
                EM_error(s->u.whileStm.test->pos, "integer required");
            }
            Tr_exp w_done=Tr_doneExp();
            Tr_exp w_init=Tr_initialExp();
            // todo translate
            Tr_exp save_break=Tr_newlabel();
            Tr_exp save_conti=Tr_newlabel();
            struct expty body = transStm(frame, venv, tenv, s->u.whileStm.body,save_break,save_conti);//trans之前应生成while的break与continue  label
            return Expty(Tr_while(test.exp,body.exp,w_done,w_init), body.ty);
        }
        case A_stm_::A_blockStm:{
            if (!s->u.blockStm)
                return Expty(nullptr, TY_Void());
            bool initial_tag= true;
            struct expty returnTy{};
            struct expty save_temp;
            S_beginScope(tenv);
            S_beginScope(venv);
            Tr_exp temp= nullptr;
            A_comStmList comStmIter;
            for(comStmIter = s->u.blockStm; comStmIter; comStmIter = comStmIter->tail){
                if (comStmIter->head->const_var_decStm){
                    A_decList decIter = comStmIter->head->const_var_decStm;
                    for ( ; decIter; decIter = decIter->tail){
                        temp=transDec(frame, venv, tenv, decIter->head,l_break,l_continue);
                        if(initial_tag == true)
                        {
                            save_temp.exp=temp;
                            initial_tag=false;
                        } else
                        {
                            save_temp.exp=Tr_seq(save_temp.exp,temp);
                        }
                    }
                }
                if (comStmIter->head->stmSeq){
                    returnTy = transStm(frame, venv, tenv, comStmIter->head->stmSeq,l_break,l_continue);
                    save_temp.exp=Tr_seq(save_temp.exp,returnTy.exp);
                    /// 一个blockStm中可以有多处return
                    // todo translate
                }
            }
            S_endScope(tenv);
            S_endScope(venv);
            return Expty(save_temp.exp, returnTy.ty);
        }
        case A_stm_::A_assignStm:{
            struct expty var = transVar(venv, tenv, s->u.assignStm.var,l_break,l_continue);

            /// 检查左值是否为const
            if (var.isConst){
                EM_error(s->pos,
                        "assignment of read-only variable \'%s\'",
                        S_getName(s->u.assignStm.var->u.simple));
            }
            /// 检查数组是否定位到了元素
            if (var.ty->kind == TY_ty_::TY_array){
                EM_error(s->pos,"assignment to expression with array type");
            }

            struct expty exp = transExp(venv, tenv, s->u.assignStm.exp,l_break,l_continue);
            if (!is_equal_ty(var.ty, exp.ty)){
                EM_error(s->u.assignStm.exp->pos,
                        "expression not of expected type %s",
                        TY_toString(var.ty));
            }
            // todo translate
            return Expty(Tr_assign(var.exp,exp.exp), TY_Void());
        }
        case A_stm_::A_returnStm:{//to do translate  now translate it as put result in rv reg
            if (!s->u.returnStm){
                return Expty(Tr_nopExp(), TY_Void());
            }
            struct expty returnTy = transExp(venv, tenv, s->u.returnStm,l_break,l_continue);
            // todo 自建 translate
            return Expty(Tr_return(returnTy.exp), returnTy.ty);
        }
        case A_stm_::A_switchStm:{//not required in grammer
            struct expty key = transExp(venv, tenv, s->u.switchStm.exp,l_break,l_continue);
            if (key.ty->kind != TY_ty_::TY_int){
                EM_error(s->u.switchStm.exp->pos, "integer required");
            }
            A_caseList caseIter;
            for (caseIter = s->u.switchStm.body; caseIter; caseIter = caseIter->tail){
                struct expty body = transStm(frame, venv, tenv, caseIter->head->body,l_break,l_continue);
                // todo switch
            }
            return Expty(nullptr, TY_Void());
        }
        case A_stm_::A_breakStm:{
            return Expty(Tr_break(l_break), TY_Void());
        }
        case A_stm_::A_continue:{
            return Expty(Tr_continue(l_continue), TY_Void());
        }
    }
    assert(0);
}


static struct expty transExp(S_table venv, S_table tenv, A_exp a,Tr_exp l_break,Tr_exp l_continue){
    switch (a->kind) {
        case A_exp_::A_varExp:{
            expty var = transVar(venv, tenv, a->u.varExp,l_break,l_continue);
            if (var.isConst){
                a->u.intExp = getConstValue(venv, a);
                a->kind = A_exp_::A_intExp;
                var.ty = TY_Int();
                /// 此处会造成内存泄漏，泄漏对象：a->u.varExp所指的A_var对象
            }
            return var;
        }
        case A_exp_::A_intExp:{
            expty expty_msg = Expty(Tr_intExp(a->u.intExp), TY_Int());
            expty_msg.isConst = true;
            return expty_msg;
        }
        case A_exp_::A_charExp:{
            expty expty_msg = Expty(Tr_intExp(a->u.charExp), TY_Int());
            expty_msg.isConst = true;//todo char translate
            return expty_msg;
        }
        case A_exp_::A_callExp:{
            auto funEntry = (E_envEntry)S_look(venv, a->u.callExp.id);
            /// 检查是否定义函数
            if (funEntry == nullptr || funEntry->kind != E_envEntry_::E_funEntry){
                EM_error(a->pos, "undefined function %s",
                         S_getName(a->u.callExp.id));
                return Expty(nullptr, TY_Void());
            }
            /// 针对putf的特殊处理
            if (strcmp(S_getName(a->u.callExp.id), "putf") == 0){
                if (a->u.callExp.args->head->kind != A_exp_::A_stringExp){
                    EM_error(a->pos, "The first argument of putf must be a String");
                }
                // todo translate
                return Expty(nullptr, TY_Void());
            }
            /// 检查参数类型
            TY_tyList formals = funEntry->u.fun.formals;
            A_expList argIter = a->u.callExp.args;
            Tr_expList params=Tr_ExpList();
            for ( ; argIter && formals; argIter = argIter->tail, formals = formals->tail){
                struct expty argType = transExp(venv, tenv, argIter->head,l_break,l_continue);
                Tr_expList_append(params,argType.exp);
                if (!is_equal_ty(argType.ty, formals->head)){
                    EM_warning(argIter->head->pos,
                            "incorrect type %s, expected %s",
                            TY_toString(argType.ty),
                            TY_toString(formals->head));
                    /// todo translate
                }
            }
            /// 检查参数个数
            if (argIter == nullptr && formals != nullptr){
                EM_error(a->pos, "not enough arguments!");
            }else if (argIter != nullptr){
                EM_error(a->pos, "too many arguments!");
            }
            return Expty(Tr_func_call(funEntry->u.fun.label,params), actual_ty(funEntry->u.fun.result));
        }
        case A_exp_::A_opExp:{
            A_binOp op = a->u.opExp.op;
            struct expty left = transExp(venv, tenv, a->u.opExp.left,l_break,l_continue);
            struct expty right{};

            if (op != A_not) {
                right = transExp(venv, tenv, a->u.opExp.right,l_break,l_continue);

                if (a->u.opExp.left->kind == A_exp_::A_intExp
                && a->u.opExp.right->kind == A_exp_::A_intExp){
                    /// 也可使用left和right的kind是否为A_IntExp判断
                    A_exp temp_left = a->u.opExp.left;
                    A_exp temp_right = a->u.opExp.right;
                    a->u.intExp = calculate(op, temp_left->u.intExp, temp_right->u.intExp);
                    a->kind = A_exp_::A_intExp;
                    expty expty_msg = Expty(nullptr, TY_Int());
                    expty_msg.isConst = true;
                    return expty_msg;
                    /// 此处会造成内存泄漏，泄漏对象：temp_left, temp_right所指对象
                }
            } else if (a->u.opExp.left->kind == A_exp_::A_intExp){
                    A_exp temp_right = a->u.opExp.right;
                    a->u.intExp = calculate(op, 0, temp_right->u.intExp);
                    a->kind = A_exp_::A_intExp;
                    expty expty_msg = Expty(nullptr, TY_Int());
                    expty_msg.isConst = true;
                    return expty_msg;
                    /// 此处会造成内存泄漏，泄漏对象：temp_left所指对象
            }

            switch (op) {
                case A_add:
                case A_sub:
                case A_mod:
                case A_mul:
                case A_div:{
                    if (left.ty->kind != TY_ty_::TY_int && left.ty->kind != TY_ty_::TY_char) {
                        EM_error(a->u.opExp.left->pos,
                                "%s expression given for LHS, expected int",
                                TY_toString(left.ty));
                    }
                    if (right.ty->kind != TY_ty_::TY_int && right.ty->kind != TY_ty_::TY_char) {
                        EM_error(a->u.opExp.right->pos,
                                 "%s expression given for RHS, expected int",
                                 TY_toString(right.ty));
                    }
                    return Expty(Tr_binop(op,left.exp,right.exp), TY_Int());
                }
                case A_eq:
                case A_ne:{
                    if (left.ty->kind == TY_ty_::TY_int){
                        if (is_equal_ty(left.ty, right.ty))
                            printf("int == int");
                            /// todo translate
                    }
                    else if (left.ty->kind == TY_ty_::TY_char){
                        if (is_equal_ty(left.ty, right.ty))
                            printf("char == char");
                            /// todo translate
                    }
                    else if (left.ty->kind == TY_ty_::TY_array){
                        if (right.ty->kind!=TY_ty_::TY_array  )
                            EM_error(a->u.opExp.right->pos,
                                    "%s expression given for RHS, expected %s",
                                    TY_toString(right.ty), TY_toString(left.ty));
                        /// todo translate refEq
                    }
                    else if(left.ty->kind == TY_ty_::TY_struct){
                        if (right.ty->kind!=TY_ty_::TY_struct )
                            EM_error(a->u.opExp.right->pos,
                                    "%S expression given for RHS, expected struct tyepe",
                                    TY_toString(right.ty));
                        /// todo translate refEq
                    } else{
                        EM_error(a->u.opExp.right->pos,
                                "unexpected %s expression in comparison",
                                TY_toString(right.ty));
                    }
                    return Expty(Tr_relop(op,left.exp,right.exp), TY_Int());
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

                    if (left.ty->kind == TY_ty_::TY_int){
                        /// todo translate
                    } else if (left.ty->kind == TY_ty_::TY_char){
                        /// todo translate
                    } else{
                        EM_error(a->u.opExp.right->pos,
                                 "unexpected %s expression in comparison",
                                 TY_toString(right.ty));
                    }
                    return Expty(Tr_relop(op,left.exp,right.exp), TY_Int());
                }

                case A_not:return Expty(Tr_relop(op, nullptr,right.exp), TY_Int());
                case A_and:
                case A_or:return Expty(Tr_relop(op,left.exp,right.exp), TY_Int());
                default:
                    assert(0);
            }
        }
        case A_exp_::A_stringExp:
            break;
    }
    assert(0);
}

static struct expty transVar(S_table venv, S_table tenv, A_var v,Tr_exp l_break,Tr_exp l_continue){
    switch (v->kind) {
        case A_var_::A_simpleVar:{
            auto varEntry = (E_envEntry)S_look(venv, v->u.simple);
            if (varEntry == nullptr || varEntry->kind != E_envEntry_::E_varEntry){
                EM_error(v->pos, "undefined variable %s",
                        S_getName(v->u.simple));
                struct expty expty_msg = Expty(nullptr, TY_Int());
                return expty_msg;
            } else{

                struct expty expty_msg = Expty(Tr_simpleVar(varEntry->u.var.access), actual_ty(varEntry->u.var.ty));
                expty_msg.isConst = varEntry->u.var.isConst;
                return expty_msg;
            }
        }
        case A_var_::A_arrayVar:{//todo 确认array base index
            struct expty id = transVar(venv, tenv, v->u.arrayVar.id,l_break,l_continue);
            // todo translate
            if (id.ty->kind != TY_ty_::TY_array){
                EM_error(v->u.arrayVar.id->pos,
                        "subscripted value is neither array nor pointer nor vector");
                struct expty expty_msg = Expty(nullptr, TY_Int());
                return expty_msg;
            } else{
                struct expty index = transExp(venv, tenv, v->u.arrayVar.index,l_break,l_continue);
                if (index.ty->kind != TY_ty_::TY_int){
                    EM_error(v->u.arrayVar.index->pos, "index must be a int");
                }
                // todo translate
                expty expty_msg = Expty(Tr_subsriptVar(id.exp,index.exp), actual_ty(id.ty->u.array));
                expty_msg.isConst = id.isConst && index.isConst;
                return expty_msg;
            }
        }
        case A_var_::A_structVar:{
            assert(0);
            // todo struct type
        }
    }
    assert(0);
}

