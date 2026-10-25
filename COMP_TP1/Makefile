
OBJS=scanner.o parser.o ast.o printAbsTree.o main.o
INTERM=scanner.c parser.c parser.h
PROGRAM=./pascal
CFLAGS=-g -Wall -Wextra

all: $(PROGRAM)

scanner.c: scanner.flex parser.h
	flex -o scanner.c scanner.flex

parser.c parser.h: parser.bison
	bison --defines=parser.h -o parser.c parser.bison

# os objetos que usam os códigos dos tokens dependem do parser.h
scanner.o parser.o ast.o printAbsTree.o main.o: ast.h parser.h

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS)
clean:
	rm -f $(PROGRAM) $(OBJS) $(INTERM)

test: $(PROGRAM)
	for f in tests/*.pas; do echo "== $$f"; $(PROGRAM) $$f; echo "(codigo de saida: $$?)"; done
