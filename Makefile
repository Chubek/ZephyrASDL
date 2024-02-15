LEX_SOURCE = scan.l
YACC_SOURCE = parse.y
PERL_SOURCE = absyn.c
TRANSLATE_SOURCE = translate.c

LEX_DEST = lex.yy.c
YACC_DEST = y.tab.c
PERL_DEST = absyn.gen.c
BIN_DEST = asdl

YACC_TAB = y.tab.h

ENTRY_POINT = asdl.c

C_SOURCE = $(ENTRY_POINT) $(PERL_DEST) $(TRANSLATE_SOURCE) $(LEX_DEST) $(YACC_DEST)

GEN_FILES = $(LEX_DEST) $(YACC_DEST) $(PERL_DEST) $(YACC_TAB)

PERL = perl
CC = gcc
LEX = flex
YACC = bison

PERL_SCRIPT = AllocPP.pl

all : $(BIN_DEST)

$(BIN_DEST) : $(LEX_DEST)
	$(CC) -o $(BIN_DEST) $(DEBUG) $(C_SOURCE)

$(LEX_DEST) : $(YACC_DEST)
	$(LEX) $(LEX_SOURCE)

$(YACC_DEST) : $(PERL_DEST)
	$(YACC) -dy $(YACC_SOURCE)

$(PERL_DEST) : $(PERL_SOURCE)
	$(PERL) $(PERL_SCRIPT) -i $(PERL_SOURCE) -o $(PERL_DEST)

.PHONY : clean
clean : 
	rm -f $(GEN_FILES)


