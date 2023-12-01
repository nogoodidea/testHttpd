#include <stdlib.h>
#include <string.h>
#include "logging.h"

#include <stdio.h>

#define MAX_CHILDREN 2

struct t_treeNode{
  char *key;
  char *value;
  struct t_treeNode *child[MAX_CHILDREN];
};


//sorting function
int sorting(struct t_treeNode *node,const char *key){
  int out = strcmp(node->key,key);
  if(out == 0){
    return MAX_CHILDREN;
  }
  if(out <0){
    return 0;
  }
  return 1;
}


char *searchTree(struct t_treeNode *node,const char *key){
  int sortReturn;
  while (node != NULL){
    sortReturn = sorting(node,key);
    if(sortReturn == MAX_CHILDREN){
      return node->value;
    }
    if(sortReturn < MAX_CHILDREN){
      break;
    }
    node = node->child[sortReturn]; 
  }
  return NULL;
}


// make a node
void makeNode(struct t_treeNode **node,char *key,char *value){
   (*node) = malloc(sizeof(struct t_treeNode)*MAX_CHILDREN);
   (*node)->key = key;
   (*node)->value = value;
   for(size_t i=0;i<MAX_CHILDREN;i+=1){
    (*node)->child[i] = NULL;
   }
}


// add node
void addNode(struct t_treeNode **node,char *key,char *value){
  int sortReturn;
  while (*node != NULL){
    sortReturn = sorting(*node,key);
    if(sortReturn == MAX_CHILDREN){
      debug("Tree add failed");
      if(key != NULL){free(key);}
      if(value != NULL){free(value);}
      return;  // can't add
    }
    node = &((*node)->child[sortReturn]); 
  }
  makeNode(node,key,value);
}

// recurively free the nodes under this one
void freeNodes(struct t_treeNode **node){
   for(size_t i=0;i<MAX_CHILDREN;i+=1){
    if((*node)->child[i] != NULL){
      if((*node)->key != NULL){free((*node)->key);}
      if((*node)->value != NULL){free((*node)->value);}
      freeNodes(&(*node)->child[i]);
      }
   }
  free(*node);
  *node = NULL;
}

// prints out the tree, for error checking
void printNodes(struct t_treeNode node){
  for(size_t i=0;i<MAX_CHILDREN;i+=1){
    if(node.child[i] != NULL){
      printNodes(*(node.child[i]));
    };
  }
  debug("NODE");
  if(node.key !=NULL){debug(node.key);}
  if(node.value !=NULL){debug(node.value);}
}
