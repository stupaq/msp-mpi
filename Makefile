
# Defines that directly influence implementation
OPTIMIZE	?= 0

# Platform specific options
ifeq ($(shell hostname), students)
CFLAGS		+= -std=gnu99 -Wall -Wextra
endif

CFLAGS		+= -O3 -DOPTIMIZE=$(OPTIMIZE)
CLINT		?= cpplint --extensions=c,h --filter=-legal/copyright,-whitespace/braces,-whitespace/newline,-whitespace/parens,-runtime/references,-runtime/int,-readability/casting

MPICC		:= mpicc

HEADERS		:= $(wildcard *.h)
MATGEN_TYPE	?= matgen-mt
MATGEN_FILE	:= $(MATGEN_TYPE).o

all: seq par
seq: msp-seq-naive.exe msp-seq-kadane.exe msp-seq-takaoka.exe
par: msp-par.exe

%.exe: %.o $(MATGEN_FILE)
	$(MPICC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS) Makefile
	$(MPICC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *core *~ *.out *.err *.exe

lint:
	@$(CLINT) msp-seq-kadane.c msp-seq-takaoka.c msp-par.c microprof.h ranking.h

todo:
	@grep -nrIe "\(TODO\|FIXME\)" --exclude-dir=.git --exclude=Makefile . || true

