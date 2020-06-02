//
// Created by loyx on 2020/5/7.
//
#include <stdio.h>
#include "temp.h"

struct Temp_temp_ {int num;};

static int temps = 100;
Temp_temp Temp_newTemp(){
    Temp_temp p = checked_malloc(sizeof(*p));
    p->num = temps++;
    return p;
}

static int label = 0;
Temp_label Temp_newLabel(){
    char buf[127];
    sprintf(buf, "L%d", label++);
    return Temp_namedLabel(String(buf));
}
Temp_label Temp_namedLabel(string name){
    return S_Symbol(name);
}
string Temp_labelString(Temp_label s){
    return S_getName(s);
}
Temp_tempList Temp_TempList(Temp_temp head, Temp_tempList tail){
    Temp_tempList p = checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}
Temp_labelList Temp_LabelList(Temp_label head, Temp_labelList tail){
    Temp_labelList p = checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}
