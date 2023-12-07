#ifndef HASH_TABLE_H
#define HASH_TABLE_H

struct hashTableEntry{
  char *key;
  char *value;
};

struct hashTable{
  struct hashTableEntry *table;
  size_t size;
  size_t used;
  size_t (*hashFunc)();
};

struct hashTable *hashTableMk(size_t size,size_t (*hashFunc)());
void hashTableInsert(struct hashTable *table,char *key,char *value,size_t index);
void hashTableReHash(struct hashTable *table,size_t newSize);
void hashTableAdd(struct hashTable *table,char *key,char *value);
char *hashTableGet(struct hashTable *table,char *key);
void hashTableFree(struct hashTable *table);
void hashTablePrint(struct hashTable *table);


#endif //HASH_TABLE_H
