#ifndef H_PARSER
#define H_PARSER

#include <string.h>
#include "hashTable.h"

enum httpRequest {GET,POST,HEAD,ERROR};

#define BUFFER_SIZE 256

void strToHeap(char *data,char **out,size_t length);

int parseHeaders(char buff[BUFFER_SIZE],size_t buffSize,struct hashTable *table,enum httpRequest *request);

#endif //H_PARSER 
