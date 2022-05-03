# Paths to files
# source files
SRCDIR=src
# objects files
ODIR=obj
# headers
IDIR=include
# executable binaries
BINDIR=bin




all: directories obj
	./bin/out

debug: directories obj
	gdb ./bin/out

obj: 
	gcc -Wall -g -o $(BINDIR)/out include/* src/*

clean:
	rm bin/*


directories:
	mkdir -p bin












