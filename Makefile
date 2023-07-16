BIN=markandsweep

.PHONY : clean 

$(BIN) : main.c
	$(CC) -fsanitize=undefined -ggdb -std=gnu99 -fno-strict-aliasing -Wall -Wextra main.c -o $(BIN)

clean :
	rm -f $(BIN) *~

run : $(BIN)
	valgrind  --leak-check=yes ./$(BIN)

