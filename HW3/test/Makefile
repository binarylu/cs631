BIN = client
OBJ = client.o

CFLAGS = -g -Wall -pedantic-errors
LIB = -lbsd -pthread

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $^ $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	-rm -fr $(BIN) $(OBJ)