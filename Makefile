CC=clang
BIN=simple
SANITIZERS=-fsanitize=address -fno-strict-aliasing
CFLAGS=-Wall -Wextra -std=c99 -fno-omit-frame-pointer -g -O0

.PHONY : clean

$(BIN) : simple.c canvas.c
	$(CC) $(CFLAGS) simple.c canvas.c -lSDL3 -o $(BIN)

clean :
	rm -f $(BIN) *~

run : $(BIN)
	./$(BIN)

debug: $(BIN)
	gdb ./$(BIN)

valrun : $(BIN)
	valgrind --fullpath-after= --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BIN)
