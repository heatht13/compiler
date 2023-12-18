BIN = c-
CC = g++

SRCS = $(BIN).y  $(BIN).l main.c astUtils.c ourgetopt.c symbolTable.cpp semantics.cpp IO.cpp yyerror.cpp emitcode.cpp codegen.cpp
HDRS = scanType.h main.h treeDef.h astUtils.h ourgetopt.h symbolTable.h semantics.h IO.h yyerror.h emitcode.h codegen.h
OBJS = lex.yy.o $(BIN).tab.o main.o astUtils.o ourgetopt.o symbolTable.o semantics.o IO.o yyerror.o emitcode.o codegen.o

$(BIN) : $(OBJS)
	$(CC) -g $(OBJS) -o $(BIN)

lex.yy.c : $(BIN).l $(BIN).tab.h
	flex $(BIN).l

$(BIN).tab.h $(BIN).tab.c : $(BIN).y
	bison -v -t -d $(BIN).y

clean :
	rm -f *~ $(OBJS) $(BIN) lex.yy.c $(BIN).tab.h $(BIN).tab.c $(BIN).output

tar : $(HDR) $(SRCS) makefile
	tar -cvf $(BIN).tar $(HDRS) $(SRCS) makefile
