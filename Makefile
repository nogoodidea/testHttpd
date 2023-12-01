CC := gcc
CC_FLAGS := -Wall -pthread  -fsanitize=address -g
LD_FLAGS := 
FILES := tree.c parser.c logging.c main.c file.c


all:
	$(CC) $(FILES) $(CC_FLAGS) $(LD_FLAGS) -o server

clean:
	rm server
