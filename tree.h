#ifdef TREE_H
#define TREE_H

#define MAX_CHILDREN 2
enum nodeColor {RED,BLACK};

struct t_treeNode{
  char *key;
  char *value;
  struct t_treeNode *parent;
  struct t_treeNode *child[MAX_CHILDREN];
};

struct t_treeRoot{
  int *sort;// sorting function return int repersenting child node to use
  struct t_treeNode root;
};

const char *searchTree(const struct t_treeRoot tree,const char *key);
void *makeNode(struct t_treeNode *parent,enum nodeColor color,char *key,char *value);
void freeNodes(struct t_treeNode *node);
void addNode(struct t_treeRoot tree,char *key,char *value);

#endif //TREE_H
