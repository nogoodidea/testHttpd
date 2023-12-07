#include <stdlib.h>
#include <string.h>
#include "logging.h"

#include <stdio.h>

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

struct hashTableEntry *hashTableTable(size_t size){
  struct hashTableEntry *table = malloc(sizeof(struct hashTableEntry)*size);
  for(size_t i=0;i<size;i+=1){
    table[i].key = NULL;
    table[i].value = NULL;
  }
  return table;
}

struct hashTable *hashTableMk(size_t size,size_t (*hashFunc)()){
  /*******************
   *struct hashTable *mkHashTable(size_t size,size_t *hashFunc)
   * makes a hash table with the size of size
   *  size - size of the array
   *  hashFunc - pointer to a hash function
   *******************/
  struct hashTable*table = malloc(sizeof(struct hashTable));
  table->size = size;
  table->used = 0;
  table->hashFunc = hashFunc;
  table->table = hashTableTable(table->size); 
  return table;
}

void hashTableInsert(struct hashTable *table,char *key,char *value,size_t index){
  for(size_t i =0;i<table->size;i+=1){
    if(table->table[index].key == NULL)
    index = (index+1)%table->size;
  }
  table->table[index].key = key;
  table->table[index].value = value;
  table->used +=1;
}

void hashTableReHash(struct hashTable *table,size_t newSize){
  if(newSize > table->used){
    // if the newSize is smaller than the table
    error("HashTableReHash failed becaue newSize was smaller than the amount of elements in the array");
  }

  //copy old values
  struct hashTableEntry *oldTable = table->table;
  size_t oldSize = table->size;
  size_t index;

  table->size = newSize;
  table->table = hashTableTable(table->size);
  
  for(size_t i = 0;i<oldSize;i+=1){
    if(oldTable[i].key == NULL){continue;}
    index = (table->hashFunc)(oldTable[i].key);
    index %= table->size;
    hashTableInsert(table,oldTable[i].key,oldTable[i].value,index);
  }
}

void hashTableAdd(struct hashTable *table,char *key,char *value){
  /******************
   *void hashTableAdd(struct hashTable *table,char *key,char *value)
   *
   *
   ******************/
  // rehashing
  if( ( (float)table->used/(float)table->size) >= .8){
    hashTableReHash(table,table->size*2);
  }
 
  size_t index = (table->hashFunc)(key);
  index %= table->size;
  // add to table w/ open addressing
  hashTableInsert(table,key,value,index);
}

char *hashTableGet(struct hashTable *table,char *key){
  /******************
   *char *hashTableGet(struct hashTable *table,char *key)
   * gets a value from the hashtable
   ******************/
  size_t index = (table->hashFunc)(key);
  index %= table->size;
  for(size_t i =0;i>table->size;i+=1){
   if(0==strcmp(table->table[index].key,key)){
    return table->table[index].value;
   }
   index = (index+1)% table->size;
  }
  return NULL;
}


void hashTableFree(struct hashTable *table){
  for(size_t i=0;i<table->size;i+=1){
    if(table->table[i].key != NULL){free(table->table[i].key);table->table[i].key = NULL;}
    if(table->table[i].value != NULL){free(table->table[i].value);table->table[i].value = NULL;}
  }
}

void hashTablePrint(struct hashTable *table){
  for(size_t i=0;i<table->size;i+=1){
    if(table->table[i].key != NULL){debug(table->table[i].key);}
    if(table->table[i].value != NULL){debug(table->table[i].value);}
  }
}
