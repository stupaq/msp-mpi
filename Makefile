#
# A template makefile for the MPI assignment.
# Concurrent and Distributed Programming Course, spring 2014.
# Faculty of Mathematics, Informatics and Mechanics.
# University of Warsaw, Warsaw, Poland.
#
# Copyright (C) Konrad Iwanicki, 2014.
#

CFLAGS		+= -O3
CLINT		?= cpplint --extensions=c,h --filter=-legal/copyright,-whitespace/braces,-whitespace/newline,-whitespace/parens,-runtime/references

MPICC		:= mpicc
MATGEN_TYPE	?= matgen-mt
MATGEN_FILE	:= $(MATGEN_TYPE).o

all: msp-seq-naive.exe msp-par.exe

%.exe: %.o $(MATGEN_FILE)
	$(MPICC) $(CFLAGS) -o $@ $^

%.o: %.c matgen.h microprof.h Makefile
	$(MPICC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *core *~ *.out *.err *.exe

lint:
	@$(CLINT) msp-par.c microprof.h

todo:
	@grep -nrIe "\(TODO\|FIXME\)" --exclude-dir=.git --exclude=Makefile .

