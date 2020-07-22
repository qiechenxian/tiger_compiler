//
// Created by loyx on 2020/5/7.
//
#include <stdio.h>
#include "temp.h"
#include "table.h"

struct Temp_temp_ {
    int num;
};

static int temps = 100;

Temp_temp Temp_newTemp() {
    Temp_temp p = (Temp_temp) checked_malloc(sizeof(*p));
    p->num = temps++;
    return p;
}

static int label = 0;

Temp_label Temp_newLabel() {
    char buf[127];
    sprintf(buf, "L%d", label++);
    return Temp_namedLabel(String(buf));
}

Temp_label Temp_namedLabel(c_string name) {
    return S_Symbol(name);
}

c_string Temp_labelString(Temp_label s) {
    return S_getName(s);
}

Temp_tempList Temp_TempList(Temp_temp head, Temp_tempList tail) {
    Temp_tempList p = (Temp_tempList) checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

Temp_labelList Temp_LabelList(Temp_label head, Temp_labelList tail) {
    Temp_labelList p = (Temp_labelList) checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

////用于assem中寄存器临时变量映射
struct Temp_map_ {
    TAB_table tab;
    Temp_map under;
};

Temp_map Temp_name(void) {
    static Temp_map m = NULL;
    if (!m) m = Temp_empty();
    return m;
}

Temp_map newMap(TAB_table tab, Temp_map under) {
    Temp_map m = (Temp_map) checked_malloc(sizeof(*m));
    m->tab = tab;
    m->under = under;
    return m;
}

Temp_map Temp_empty(void) {
    return newMap(TAB_empty(), NULL);
}

Temp_map Temp_layerMap(Temp_map over, Temp_map under) {
    if (over == NULL)
        return under;
    else
        return newMap(over->tab, Temp_layerMap(over->under, under));
}

void Temp_enter(Temp_map m, Temp_temp t, c_string s) {
    assert(m && m->tab);
    TAB_enter(m->tab,t,s);
}

c_string Temp_look(Temp_map m, Temp_temp t) {
    c_string s;
    assert(m && m->tab);
    s = (c_string )(TAB_look(m->tab, t));
    if (s) return s;
    else if (m->under) return Temp_look(m->under, t);
    else return NULL;
}

static FILE *outfile;
void showit(Temp_temp t,c_string s){
    fprintf(outfile, "t%d -> %s\n", t->num, s);
}

void Temp_dumpMap(FILE *out, Temp_map m) {
    outfile=out;
    TAB_dump(m->tab,(void (*)(void *, void*))showit);
    if (m->under) {
        fprintf(out,"---------\n");
        Temp_dumpMap(out,m->under);
    }
}













