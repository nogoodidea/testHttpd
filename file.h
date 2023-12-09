#ifndef FILE_H
#define FILE_H

char *getFileFormat(char *fileName);
int getFile(char *path);
void respondToRequest(int sock,const char *path,enum httpRequest request,struct hashTable *table);


#endif // FILE_H
