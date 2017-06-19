CLIBS=./lib/
_SRC=./src/
ODIR=./obj/
INC=-I$(CLIBS)
SRC=test.c

CC=gcc
CFLAGS=-Wfatal-errors

_DEPS=$(CLIBS)utils.c $(CLIBS)utils.h $(CLIBS)sha3.c $(CLIBS)sha3.h $(_SRC)$(SRC) 

_OBJ=$(ODIR)utils.c $(ODIR)sha3.c $(ODIR)$(SRC) 
OBJ=$(patsubst %.c,%.o,$(_OBJ))

sha3_tests: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(INC)*

$(OBJ): $(_DEPS) 
	$(CC) -o $@ -c $< $(CFLAGS) $(INC)*


