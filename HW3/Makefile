BIN = main
OBJ = net.o \
	  main.o

CFLAGS = -g -Wall -pedantic-errors
LIB = -pthread

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $^ $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	-rm -fr $(BIN) $(OBJ)