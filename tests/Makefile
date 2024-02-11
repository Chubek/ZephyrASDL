SPECS_DIR = specs
SRC_DIR = src
TESTS_DIR = tests


BUILD_DIR = tests/_build

SRC_YACC = $(SPECS_DIR)/parser.y
SRC_LEX = $(SPECS_DIR)/scanner.l

DEST_YACC = $(BUILD_DIR)/y.tab.c
DEST_LEX = $(BUILD_DIR)/lex.yy.c

YACC_HEADER = y.tab.h

SRC_AST = $(SRC_DIR)/ast.c

SRC_TEST_AST = $(TESTS_DIR)/test_ast.c

INCLUDE = $(SRC_DIR)

DEST_BIN = $(BUILD_DIR)/test_binry

LEX = flex
YACC = bison
CC = gcc

ast_test: $(DEST_BIN)

$(DEST_BIN): $(DEST_YACC)
	$(CC) -I$(INCLUDE) -o $(DEST_BIN) $(SRC_AST) $(DEST_YACC) $(DEST_LEX) $(SRC_TEST_AST)

$(DEST_YACC): $(DEST_LEX)
	$(YACC) -d -o $(DEST_YACC)  $(SRC_YACC)

$(DEST_LEX): $(SRC_LEX)
	mkdir -p $(BUILD_DIR)
	$(LEX) -o $(DEST_LEX) $(SRC_LEX)


clean: 
	rm $(DEST_BIN) $(DEST_YACC) $(DEST_LEX) $(YACC_HEADER)
	

.PHONY : clean
