#ifndef H_PARSER
#define H_PARSER

#include <string.h>
#include "tree.h"

#define BUFFER_SIZE 2048

int parseHeaders(char buff[BUFFER_SIZE],ssize_t buffSize,struct t_treeNode *head);

#endif //H_PARSER 
