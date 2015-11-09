BIN = main
OBJ = util.o \
	  task_queue.o \
	  handle.o \
	  net.o \
	  main.o

CFLAGS = -g -Wall -pedantic-errors
LIB = -lbsd -pthread

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $^ $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	-rm -fr $(BIN) $(OBJ)