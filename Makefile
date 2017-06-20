CLIBS=./lib/
_SRC=./src/
ODIR=./obj/
INC=-I$(CLIBS)
SRC=test.c

CC=gcc
CFLAGS=-Wall

_OBJ=$(ODIR)$(SRC) $(ODIR)utils.c $(ODIR)sha3.c 
OBJ=$(patsubst %.c,%.o,$(_OBJ))

sha3_tests: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(INC)

$(patsubst %.c,%.o,$(ODIR)$(SRC)): $(_SRC)$(SRC)
	$(CC) -o $@ -c $< $(CFLAGS) $(INC)

$(ODIR)utils.o: $(CLIBS)utils.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INC)

$(ODIR)sha3.o: $(CLIBS)sha3.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INC)
