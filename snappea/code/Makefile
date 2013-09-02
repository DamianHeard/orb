SHELL=/bin/sh

CFLAGS= -c -g -Wall -I../headers
SRCS = ${wildcard *.c}
OBJS= ${SRCS:%.c=%.o}

lib: liborb.a

liborb.a: ${OBJS}
	ar r liborb.a ${OBJS}
	ranlib liborb.a

%.o: %.c
	gcc ${CFLAGS} $*.c -o $@

depend:
	@echo making dependency file Makefile.dependencies ...
	gcc -MM -I../headers *.c > Makefile.dependencies

include Makefile.dependencies
