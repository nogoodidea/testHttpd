#ifndef H_PARSER
#define H_PARSER

#include <string.h>
#include "tree.h"

enum httpRequest {GET,POST,HEAD,ERROR};

#define BUFFER_SIZE 2048

int parseHeaders(char buff[BUFFER_SIZE],ssize_t buffSize,struct t_treeNode **head);
enum httpRequest parseRequest(struct t_treeNode *head,char **requestPath);

#endif //H_PARSER 
