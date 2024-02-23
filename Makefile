BIN_DEST = asdl
MAN_DEST = man/man1

YACC_TAB = parse.tab.h

PERL_ABSYN_DEST = absyn.gen.c
LEX_DEST = lex.yy.c
YACC_DEST = parse.tab.c

ENTRY_POINT = asdl.c

YACC_SOURCE = parse.y
LEX_SOURCE = scan.l
PERL_ABSYN_SOURCE = absyn.c
TRANS_SOURCE = translate.c
C_SOURCE = $(ENTRY_POINT) $(PERL_ABSYN_DEST) $(TRANS_SOURCE) $(LEX_DEST) $(YACC_DEST)

GEN_FILES = $(LEX_DEST) $(YACC_DEST) $(PERL_ABSYN_DEST) $(YACC_TAB) $(BIN_DEST)

PERL = perl
CC = gcc
LEX = flex
YACC = bison
INSTALL = install
MANDB = mandb

MAN_SOURCE = asdl.1

PERL_SCRIPT = AllocPP.pl

DESTDIR = /usr/local/
BINDIR = bin

all : $(BIN_DEST)

$(BIN_DEST) : $(LEX_DEST)
	$(CC) -o $(BIN_DEST) $(DEBUG) $(C_SOURCE)

$(LEX_DEST) : $(YACC_DEST)
	$(LEX) $(LDBG) $(LEX_SOURCE)

$(YACC_DEST) : $(PERL_ABSYN_DEST)
	$(YACC) -d$(YDBG) $(YACC_SOURCE)

$(PERL_ABSYN_DEST) : $(PERL_ABSYN_SOURCE)
	$(PERL) $(PERL_SCRIPT) -i $(PERL_ABSYN_SOURCE) -o $(PERL_ABSYN_DEST)

.PHONY : clean
clean :
	rm -f $(GEN_FILES)

install :
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 755 $(BIN_DEST) $(DESTDIR)$(BINDIR)
	$(INSTALL) -d $(DESTDIR)$(MAN_DEST)
	$(INSTALL) -m 644 $(MAN_SOURCE) $(DESTDIR)$(MAN_DEST)
	$(MANDB)

uninstall :
	rm -f $(DESTDIR)$(BINDIR)/$(BIN_DEST)
	rm -f $(DESTDIR)$(MAN_DEST)/asdl.1
	$(MANDB)

