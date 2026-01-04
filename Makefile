CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11
BIN=trace

all: $(BIN)

$(BIN): src/trace.c
	$(CC) $(CFLAGS) -o $(BIN) src/trace.c

clean:
	rm -f $(BIN)
