CLIBS=./lib/
_SRC=./src/
ODIR=./obj/
INC=-I$(CLIBS)
SRC=test.c

CC=gcc
CFLAGS=-Wall -O3

_DEPS=$(_SRC)$(SRC) $(CLIBS)utils.c $(CLIBS)sha3.c

_OBJ=$(ODIR)$(SRC) $(ODIR)utils.c $(ODIR)sha3.c
OBJ=$(_OBJ:.c=.obj)

sha3_tests: $(OBJ)
	$(CC) $** $(CFLAGS)

$(OBJ)%.obj: $(_DEPS) 
	$(CC) -c -o sha3_tests $(ODIR) $** $(CFLAGS) $(INC)

