BIN = ls
OBJ = options.o \
	  prints.o \
	  compare.o \
	  ls.o

#CFLAGS = -g -Wall -pedantic-errors
CFLAGS = -g -Wall
LIB = -lbsd -lm

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $^ $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	-rm -fr $(BIN) $(OBJ)