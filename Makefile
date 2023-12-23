CC := gcc
LD := ld
CC_FLAGS := -Wall -pthread  -fsanitize=address -g


HTTP_LD_FLAGS := -r -b binary 
HTTP_LD_FILES := html/error.html html/404.html
OBJS := logging.o hashTable.o parser.o file.o main.o

OUT := server

all: $(OBJS)
	##$(LD) $(HTTP_LD_FLAGS) -o httpFiles.o $(HTTP_LD_FILES)
	$(CC) $(CC_FLAGS) $(OBJS) httpFiles.o -o $(OUT)

$(OBJS): %.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@ 

##
clean:
	rm -v $(OBJS) httpFiles.o $(OUT)
