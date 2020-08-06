//
// Created by loyx on 2020/5/7.
//
#include <stdio.h>
#include "temp.h"
#include "table.h"

static int temps = 100;

Temp_temp Temp_newTemp() {
    Temp_temp p = (Temp_temp) checked_malloc(sizeof(*p));
    p->num = temps++;
    {
        char temp_inform[16];
        sprintf(temp_inform, "%d", p->num);
        Temp_enter(Temp_name(), p, String(temp_inform));
    }
    return p;
}

Temp_temp Temp_new_special(c_string s, int num) {
    Temp_temp p = (Temp_temp) checked_malloc(sizeof(*p));
    p->num = num;
    {
        char temp_inform[16];
        sprintf(temp_inform, "%s", s);
        Temp_enter(Temp_name(), p, String(temp_inform));
    }
    return p;
}

static int stringLabel = 0;

Temp_label Temp_newStringLabel() {
    char buf[127];
    sprintf(buf, ".LC%d", stringLabel++);
    return Temp_namedLabel(String(buf));
}

static int label = 0;

Temp_label Temp_newLabel() {
    char buf[127];
    sprintf(buf, ".L%d", label++);
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
    static Temp_map m = nullptr;
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
    TAB_enter(m->tab, t, s);
}

c_string Temp_look(Temp_map m, Temp_temp t) {
    c_string s;
    assert(m && m->tab);
    s = (c_string) (TAB_look(m->tab, t));
    if (s) return s;
    else if (m->under) return Temp_look(m->under, t);
    else return NULL;
}

static FILE *outfile;

void showit(Temp_temp t, c_string s) {
    fprintf(outfile, "t%d -> %s\n", t->num, s);
}

void Temp_dumpMap(FILE *out, Temp_map m) {
    outfile = out;
    TAB_dump(m->tab, (void (*)(void *, void *)) showit);
    if (m->under) {
        fprintf(out, "---------\n");
        Temp_dumpMap(out, m->under);
    }
}

Temp_tempList Temp_reverseList(Temp_tempList t) {
    if (t == NULL) {
        return t;
    }
    Temp_tempList tl = NULL;
    for (; t; t = t->tail) {
        tl = Temp_TempList(t->head, tl);
    }
    return tl;
}

bool Temp_equal(Temp_tempList ta, Temp_tempList tb) {
    Temp_temp t;
    Temp_tempList tl = NULL;
    Temp_map m = Temp_empty();
    int ca = 0, cb = 0;

    for (; ta; ta = ta->tail) {
        t = ta->head;
        Temp_enter(m, t, (char *)"e");
        ++ca;
    }

    for (; tb; tb = tb->tail) {
        t = tb->head;
        if (Temp_look(m, t) == NULL) {
            return false;
        }
        ++cb;
    }
    free(m);
    return (ca == cb);
}

Temp_tempList Temp_union(Temp_tempList ta, Temp_tempList tb) {
    Temp_temp t;
    Temp_tempList tl = NULL;
    Temp_map m = Temp_empty();

    for (; ta; ta = ta->tail) {
        t = ta->head;
        if (Temp_look(m, t) == NULL) {
            Temp_enter(m, t, (char *)"u");
            tl = Temp_TempList(t, tl);
        }
    }

    for (; tb; tb = tb->tail) {
        t = tb->head;
        if (Temp_look(m, t) == NULL) {
            Temp_enter(m, t, (char *)"u");
            tl = Temp_TempList(t, tl);
        }
    }
    free(m);
    return tl;
}

Temp_tempList Temp_intersect(Temp_tempList ta, Temp_tempList tb) {
    Temp_temp t;
    Temp_tempList tl = NULL;
    Temp_map m = Temp_empty();

    for (; ta; ta = ta->tail) {
        t = ta->head;
        Temp_enter(m, t, (char *)"i");
    }

    for (; tb; tb = tb->tail) {
        t = tb->head;
        if (Temp_look(m, t) != NULL) {
            tl = Temp_TempList(t, tl);
        }
    }

    free(m);
    return tl;
}

Temp_tempList Temp_minus(Temp_tempList ta, Temp_tempList tb) {
    Temp_temp t;
    Temp_tempList tl = NULL;
    Temp_map m = Temp_empty();

    for (; tb; tb = tb->tail) {
        t = tb->head;
        Temp_enter(m, t, (char *)"m");
    }

    for (; ta; ta = ta->tail) {
        t = ta->head;
        if (Temp_look(m, t) == NULL) {
            tl = Temp_TempList(t, tl);
        }
    }
    free(m);
    return tl;
}

bool Temp_inList(Temp_temp t, Temp_tempList tl) {
    for (; tl; tl = tl->tail) {
        if (tl->head == t) {
            return true;
        }
    }
    return false;
}
void Temp_enterPtr(Temp_map m, Temp_temp t, void *ptr) {
    assert(m && m->tab);
    TAB_enter(m->tab, t, ptr);
}

void* Temp_lookPtr(Temp_map m, Temp_temp t) {
    assert(m && m->tab);
    void *s = TAB_look(m->tab, t);
    if (s) return s;
    else if (m->under) return Temp_lookPtr(m->under, t);
    else return NULL;
}


int Temp_number(Temp_temp test)
{
    return test->num;
}










