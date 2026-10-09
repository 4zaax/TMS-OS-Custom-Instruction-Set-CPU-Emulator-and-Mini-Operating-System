CC      ?= cc
CFLAGS  ?= -std=c11 -O2 -Wall
SRC     := src/os.c src/handlers.c src/helpers.c
HDR     := src/os.h src/handlers.h src/helpers.h
BIN     := tmsos

.PHONY: all run test clean

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o $@ $(SRC)

run: $(BIN)
	./$(BIN)

test: $(BIN)
	sh tests/run_tests.sh ./$(BIN)

clean:
	rm -f $(BIN) $(BIN).exe
