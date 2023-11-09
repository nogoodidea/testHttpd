CC := gcc
CC_FLAGS := -Wall -pthread -fsanitize=address -g
LD_FLAGS := 
FILES := main.c tree.c


all:
	$(CC) $(FILES) $(CC_FLAGS) $(LD_FLAGS) -o server

clean:
	rm server
