CC := gcc
LD := ld
CC_FLAGS := -Wall -pthread  -fsanitize=address -g
LD_FLAGS := -r -b binary 

HTTP_LD_FILES := html/404.html html/500.html 
OBJS := logging.o tree.o parser.o file.o main.o

OUT := server

all: $(OBJS)
	$(LD) $(LD_FLAGS) -o httpFiles.o $(HTTP_LD_FILES)
	$(CC) $(CC_FLAGS) $(OBJS) httpFiles.o -o $(OUT)

$(OBJS): %.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@ 

##
clean:
	rm -v $(OBJS) httpFiles.o $(OUT)
