#ifndef TREE_H
#define TREE_H

#define MAX_CHILDREN 2

struct t_treeNode{
  char *key;
  char *value;
  struct t_treeNode *parent;
  struct t_treeNode *child[MAX_CHILDREN];
};

const char *searchTree(const struct t_treeNode tree,const char *key);
void *makeNode(struct t_treeNode *parent,char *key,char *value);
void freeNodes(struct t_treeNode *node);
void addNode(struct t_treeNode *tree,char *key,char *value);

#endif //TREE_H
