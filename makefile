cc:=g++ --std=c++14

complier:main.o util.o errormsg.o table.o symbol.o ast.o lex.yy.o SysY.tab.o types.h types.o env.o temp.o tree.o arm_frame.o translate.o semant.o



	$(cc) -o complier main.o util.o errormsg.o table.o symbol.o ast.o  lex.yy.o  SysY.tab.o types.h types.o env.o temp.o tree.o arm_frame.o translate.o semant.o







main.o:main.cpp util.h ast.h semant.h env.h



	$(cc) -c main.cpp util.h ast.h semant.h env.h







semant.o:semant.cpp semant.h env.h



	$(cc) -c semant.cpp semant.h env.h







translate.o:translate.cpp translate.h frame.h



	$(cc)  -c translate.cpp translate.h frame.h







arm_frame.o:arm_frame.cpp frame.h tree.h temp.h tree.h



	$(cc)  -c arm_frame.cpp frame.h tree.h temp.h tree.h







tree.o:tree.cpp tree.h temp.h



	$(cc)  -c tree.cpp tree.h temp.h







temp.o:temp.cpp temp.h symbol.h



	$(cc) -c temp.cpp temp.h symbol.h







env.o:env.cpp types.h temp.h translate.h



	$(cc) -c env.cpp types.h temp.h translate.h







types.o:types.cpp types.h



	$(cc) -c types.cpp types.h







SysY.tab.o:SysY.tab.cpp lex.yy.o ast.o



	$(cc) -c SysY.tab.cpp lex.yy.o ast.o







lex.yy.o:lex.yy.cpp SysY.tab.h errormsg.o util.o



	$(cc) -c lex.yy.cpp SysY.tab.h errormsg.o util.o







ast.o:ast.cpp ast.h



	$(cc) -c ast.cpp ast.h







symbol.o:symbol.cpp symbol.h table.h



	$(cc) -c symbol.cpp symbol.h table.h







table.o:table.cpp temp.h table.h



	$(cc) -c table.cpp temp.h table.h







util.o:util.cpp util.h



	$(cc) -c util.cpp util.h







errormsg.o:errormsg.cpp errormsg.h



	$(cc) -c errormsg.cpp errormsg.h



clean:

	rm complier main.o semant.o translate.o arm_frame.o tree.o temp.o env.o types.o SysY.tab.o lex.yy.o ast.o symbol.o table.o util.o errormsg.o

