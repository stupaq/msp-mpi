ifneq ($(shell hostname), notos)
CFLAGS		+= -std=c99 -Wall -Wextra
endif

CFLAGS		+= -O3 -DNDEBUG
CLINT		:= cpplint --extensions=c,h --filter=-legal/copyright,-whitespace/braces,-whitespace/newline,-whitespace/parens,-runtime/references,-runtime/int,-readability/casting

MPICC		:= mpicc

HEADERS		:= $(wildcard *.h)
SEQUENTIAL	?= $(wildcard msp-seq*.c)
PARALLEL	?= $(wildcard msp-par*.c)

MATGEN_TYPE	?= matgen-cr
MATGEN_FILE	:= $(MATGEN_TYPE).o

all: $(SEQUENTIAL:.c=.exe) $(PARALLEL:.c=.exe)

%.exe: %.o $(MATGEN_FILE)
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.o: %.c $(HEADERS) Makefile
	$(MPICC) $(CFLAGS) -c -o $@ $< $(LDFLAGS) $(LDLIBS)

clean:
	rm -f *.o *core *~ *.out *.err *.exe

lint:
	@$(CLINT) $(wildcard *.*)

todo:
	@grep -nrIe "\(TODO\|FIXME\)" --exclude-dir=.git --exclude=Makefile . || true

