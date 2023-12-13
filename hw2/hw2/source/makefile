BIN = c-
CC = gcc

SRCS = $(BIN).y  $(BIN).l astUtils.c ourgetopt.c
HDRS = scanType.h treeDef.h astUtils.h ourgetopt.h
OBJS = lex.yy.o $(BIN).tab.o astUtils.o ourgetopt.o

$(BIN) : $(OBJS)
	$(CC) $(OBJS) -o $(BIN)

lex.yy.c : $(BIN).l $(BIN).tab.h
	flex $(BIN).l

$(BIN).tab.h $(BIN).tab.c : $(BIN).y
	bison -v -t -d $(BIN).y

clean :
	rm -f *~ $(OBJS) $(BIN) lex.yy.c $(BIN).tab.h $(BIN).tab.c $(BIN).output

tar : $(HDR) $(SRCS) makefile
	tar -cvf $(BIN).tar $(HDRS) $(SRCS) makefile
