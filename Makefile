PEG_SOURCE = asdl.peg
PERL_SOURCE = absyn.c
C_SOURCE = translate.c absyn.gen.c asdltrn.c

PEG_DEST = parse.peg.h
PERL_DEST = absyn.gen.c
BIN_DEST = asdltrn

PERL = perl
CC = gcc
PEG = peg

PERL_SCRIPT = AllocPP.pl

all : $(BIN_DEST)

$(BIN_DEST) : $(PEG_DEST)
	$(CC) -o $(BIN_DEST) $(DEBUG) $(C_SOURCE)

$(PEG_DEST) : $(PERL_DEST)
	$(PEG) -o $(PEG_DEST) $(PEG_SOURCE)

$(PERL_DEST) : 
	$(PERL) $(PERL_SCRIPT) -i $(PERL_SOURCE) -o $(PERL_DEST)

.PHONY : clean
clean : 
	rm -f $(C_BIN) $(PERL_DEST) $(PEG_DEST)
