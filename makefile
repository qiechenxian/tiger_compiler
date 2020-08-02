cc:=g++ --std=c++14 -Wall

complier: main.o util.o errormsg.o table.o symbol.o ast.o lex.yy.o SysY.tab.o \
	types.o env.o temp.o tree.o arm_frame.o translate.o semant.o prast.o tree.o ast.o \
	codegen.o assem.o array_init.o cannon.o regalloc.o liveness.o  color.o flowgraph.o \
	graph.o
	$(cc) -o complier $^

main.o:main.cpp util.h ast.h semant.h env.h
	$(cc) -c main.cpp

semant.o:semant.cpp semant.h env.h
	$(cc) -c semant.cpp

translate.o:translate.cpp translate.h frame.h
	$(cc)  -c translate.cpp

arm_frame.o:arm_frame.cpp frame.h tree.h temp.h tree.h
	$(cc)  -c arm_frame.cpp

tree.o:tree.cpp tree.h temp.h
	$(cc)  -c tree.cpp

temp.o:temp.cpp temp.h symbol.h
	$(cc) -c temp.cpp

env.o:env.cpp types.h temp.h translate.h
	$(cc) -c env.cpp

types.o:types.cpp types.h
	$(cc) -c types.cpp

SysY.tab.o:SysY.tab.cpp
	$(cc) -c SysY.tab.cpp

lex.yy.o:lex.yy.cpp SysY.tab.h
	$(cc) -c lex.yy.cpp

prast.o:prast.cpp prast.h
	$(cc) -c prast.cpp

ast.o:ast.cpp ast.h
	$(cc) -c ast.cpp

symbol.o:symbol.cpp
	$(cc) -c symbol.cpp

table.o:table.cpp temp.h table.h
	$(cc) -c table.cpp

util.o:util.cpp util.h
	$(cc) -c util.cpp

errormsg.o:errormsg.cpp
	$(cc) -c errormsg.cpp

codegen.o:codegen.cpp
	$(cc) -c codegen.cpp

assem.o:assem.cpp
	$(cc) -c assem.cpp

array_init.o:array_init.cpp
	$(cc) -c array_init.cpp

cannon.o:cannon.cpp
	$(cc) -c cannon.cpp

regalloc.o:regalloc.cpp
	$(cc) -c regalloc.cpp

color.o:color.cpp
	$(cc) -c color.cpp

liveness.o:liveness.cpp
	$(cc) -c liveness.cpp

flowgraph.o:flowgraph.cpp
	$(cc) -c flowgraph.cpp

graph.o:graph.cpp
	$(cc) -c graph.cpp

clean:
	rm -f complier *.o *.gch

