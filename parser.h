#ifndef H_PARSER
#define H_PARSER

#include <string.h>
#include "tree.h"

enum httpRequest {GET,POST,HEAD,ERROR};

#define BUFFER_SIZE 256 

int parseHeaders(char buff[BUFFER_SIZE],ssize_t buffSize,struct t_treeNode **head,enum httpRequest *request);

#endif //H_PARSER 
