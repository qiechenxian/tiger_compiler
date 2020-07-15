complier:main.o util.o errormsg.o table.o symbol.o ast.o lex.yy.o SysY.tab.o types.h types.o env.o temp.o tree.o arm_frame.o translate.o semant.o
	g++ -o complier main.o util.o errormsg.o table.o symbol.o ast.o  lex.yy.o  SysY.tab.o types.h types.o env.o temp.o tree.o arm_frame.o translate.o semant.o

main.o:main.cpp util.h ast.h semant.h env.h
	g++ -c main.cpp util.h ast.h semant.h env.h

semant.o:semant.cpp semant.h env.h
	g++ -c semant.cpp semant.h env.h

translate.o:translate.cpp translate.h frame.h
	g++ -c translate.cpp translate.h frame.h

arm_frame.o:arm_frame.cpp frame.h tree.h temp.h tree.h
	g++ -c arm_frame.cpp frame.h tree.h temp.h tree.h

tree.o:tree.cpp tree.h temp.h
	g++ -c tree.cpp tree.h temp.h

temp.o:temp.cpp temp.h symbol.h
	g++ -c temp.cpp temp.h symbol.h

env.o:env.cpp types.h temp.h translate.h
	g++ -c env.cpp types.h temp.h translate.h

types.o:types.cpp types.h
	g++ -c types.cpp types.h

SysY.tab.o:SysY.tab.cpp lex.yy.o ast.o
	g++ -c SysY.tab.cpp lex.yy.o ast.o

lex.yy.o:lex.yy.cpp SysY.tab.h errormsg.o util.o
	g++ -c lex.yy.cpp SysY.tab.h errormsg.o util.o

ast.o:ast.cpp ast.h
	g++ -c ast.cpp ast.h

symbol.o:symbol.cpp symbol.h table.h
	g++ -c symbol.cpp symbol.h table.h

table.o:table.cpp temp.h table.h
	g++ -c table.cpp temp.h table.h

util.o:util.cpp util.h
	g++ -c util.cpp util.h

errormsg.o:errormsg.cpp errormsg.h
	g++ -c errormsg.cpp errormsg.h

