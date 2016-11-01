CC	      = gcc
CFLAGS        = -gdwarf-3 -O -Wall
HDRS	      = define.h
LDFLAGS	      =
LIBS	      =
OBJS	      = main.o lex.o oparser.o parse.o
PROGRAM	      = token

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(HDRS)
		$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

clean:;		rm -f *.o *~

###
main.o: main.c define.h
lex.o:  lex.c  define.h
oparser.o: oparser.c define.h
parse.o: parse.c define.h