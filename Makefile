# Paths to files
# source files
SRCDIR=src
# objects files
ODIR=obj
# headers
IDIR=include
# executable binaries
BINDIR=bin



debug:
	gcc -Wall -D DEBUG_TRACEEXECUTION -g -o $(BINDIR)/debug include/* src/*
	./bin/debug

clean:
	rm bin/*














