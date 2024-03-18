CC := gcc
LD := ld
##CC_FLAGS := -Wall -lm -pthread  -fsanitize=address -g
CC_FLAGS := -Wall -lm


HTTP_LD_FLAGS := -r -b binary 
HTTP_LD_FILES := html/error.html html/404.html
OBJS := logging.o hashTable.o parser.o file.o main.o

OUT := server

all: $(OBJS)
	$(CC) $(CC_FLAGS) $(OBJS) -o $(OUT)

$(OBJS): %.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@ 

##
clean:
	rm -v $(OBJS) $(OUT)
