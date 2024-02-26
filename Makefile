# Adjustable variables
CC := gcc
YACC := bison -dy
LEX := flex
BINARY := asdl
DESTDIR := /usr/local/bin
MANDIR := /usr/local/share/man/man1
DOCDIR := /usr/local/share/doc/asdl

# Source files
SRCS := asdl.c
GEN_SRCS := absyn.gen.c translate.gen.c
YACC_SRCS := parse.y
LEX_SRCS := scan.l

# Generated files
OBJS := $(SRCS:.c=.o)
GEN_OBJS := $(GEN_SRCS:.c=.o)
YACC_OBJS := $(YACC_SRCS:.y=.tab.o)
LEX_OBJS := lex.yy.o

# Targets
all: $(BINARY)

$(BINARY): $(OBJS) $(GEN_OBJS) $(YACC_OBJS) $(LEX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

absyn.gen.c: absyn.c
	perl allocpp.pl -i absyn.c -o absyn.gen.c

translate.gen.c: translate.c
	perl allocpp.pl -i translate.c -o translate.gen.c

parse.tab.c parse.tab.h: parse.y
	$(YACC) $(YDBG) -o parse.tab.c parse.y

lex.yy.c: scan.l
	$(LEX) $(LDBG) scan.l

clean:
	rm -f $(BINARY) $(OBJS) $(GEN_OBJS) $(YACC_OBJS) $(LEX_OBJS) \
		absyn.gen.c translate.gen.c parse.tab.c parse.tab.h lex.yy.c
	@echo "Clean complete."

install: $(BINARY) companions/asdl.1
	mkdir -p $(DESTDIR)
	cp $(BINARY) $(DESTDIR)
	mkdir -p $(MANDIR)
	cp companions/asdl.1 $(MANDIR)
	mandb
	cp -r companions $(DOCDIR)
	@echo "Install complete."

.PHONY: all clean install

