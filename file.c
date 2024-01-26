// for files
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// stat
#include <sys/stat.h>
#include <sys/sysmacros.h>
// dirnet stuff
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <string.h>
#include <stdbool.h>

// BUFFER SIZE
#include "parser.h"

//debug
#include "logging.h"

// error 
#include "text.h"

// HTTP VERSION
#define HTTP_VERSION "HTTP/1.1 "

//
#define SERVER_NAME "testHttpd0.0"

struct httpReply {
  int statusCode;
  size_t contentSize;
};

//Return Headers
enum replyHeader {End,Server,Date,ContentType,ContentLength};
enum replyHeader replyHeaderList[] = {Server,Date,ContentType,ContentLength,End};

int getFile(char *path,struct stat *statOut){
  /**************************
   *int getFile(char *path)
   * gets the file spesified by the path
   * returns -1 if error
   * returns -2 if dir
   * retuns fd otherwise
   * path - the file path to try to get, if it can't find the file it will look for an index file
   **************************/
  if(path == NULL){
    debug("PATH IS NULL");
    return -1;
  }
  debug(path);
  if(stat(path,statOut)==0){
    if(statOut->st_mode & S_IFDIR){
      return -2;
    }else{
      return open(path,O_RDONLY); 
    }
  }
 debug("FILE DOES NOT EXIST");
 return -1;
}

void getFileFormat(char *fileName,char *buff){
  /***********************
   *char *getFileFormat(char *fileName)
   * char *fileName - the path of the file
   * 
   * returns the proper file MIME (IANA media types)
   * if we don't know what sourt of file it is it uses 
   *  basicly just going through this list here and useing the most common filename
   *  extend as needed
   *  https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types
   *
   ***********************/
  // parse the string and find the last instance of .
  size_t i = 0;
  size_t dot=0;

  while(fileName[i] != '\0'){
    if(fileName[i] == '.'){
      dot=i;
    }
    i+=1;
  }

  // now we got the end, do something with it
  //BASIC
  if(strcmp(".txt",&(fileName[dot]))){
    strcpy(buff,"text/plain");
    return;
  }
  if(strcmp(".css",&(fileName[dot]))){
    strcpy(buff,"text/css");
    return;
  }
  if(strcmp(".html",&(fileName[dot]))){
    strcpy(buff,"text/html");
    return;
  }
  if(strcmp(".htm",&(fileName[dot]))){
    strcpy(buff,"text/html");
    return;
  }
  if(strcmp(".js",&(fileName[dot]))){
    strcpy(buff,"text/javascript");
    return;
  }
  if(strcmp(".md",&(fileName[dot]))){
    strcpy(buff,"text/markdown");
    return;
  }
  //IMAGES
  if(strcmp(".apng",&(fileName[dot]))){
    strcpy(buff,"image/apng");
    return;
  }
  if(strcmp(".avif",&(fileName[dot]))){
    strcpy(buff,"image/avif");
    return;
  }
  if(strcmp(".gif",&(fileName[dot]))){
    strcpy(buff,"image/gif");
    return;
  }
  if(strcmp(".png",&(fileName[dot]))){
    strcpy(buff,"image/png");
    return;
  }
  if(strcmp(".svg",&(fileName[dot]))){
    strcpy(buff,"image/svg+xml");
    return;
  }
  if(strcmp(".webp",&(fileName[dot]))){
    strcpy(buff,"image/webp");
    return;
  }

  // we got to the end say fuck it
  strcpy(buff,"application/octet-stream");
}

bool bufferAdd(size_t *i,char *buff,char *str,size_t len){
  if(*i+len >= BUFFER_SIZE){error("BUFFER OVERFLOW");return true;}
  memcpy(&(buff[*i]),str,len);
  *i += len;
  return false;
}

void genHeaders(int sock,char *path,struct stat *statOut,struct httpReply httpInfo,struct hashTable *table,enum httpRequest request){
  size_t i = 0;
  size_t headerCount = 0;
  size_t len = 0;
  char buff[BUFFER_SIZE];
  char contentTypeBuff[64];
  printf("\nbuff:%p\n",buff);
  // START
  
  // HTTP VERSION
  bufferAdd(&i,buff,HTTP_VERSION,strlen(HTTP_VERSION));

  //edge case
  if(i+3 >= BUFFER_SIZE){error("BUFFER OVERFLOW");}
  sprintf(&(buff[i]),"%i",httpInfo.statusCode);
  i+=3;

  bufferAdd(&i,buff,"\n\r",2);

  //End,Server,Date,Expires,ContentType
  while(replyHeaderList[headerCount] != End){
    switch(replyHeaderList[headerCount]){
      case Server:
        bufferAdd(&i,buff,"Server: ",8);
        bufferAdd(&i,buff,SERVER_NAME,strlen(SERVER_NAME));
        bufferAdd(&i,buff,"\n\r",2);
        break;
      case Date:
        // time of last access
        if(statOut != NULL){
          bufferAdd(&i,buff,"Date: ",6);
          
          //edge case
          if(i+26 >= BUFFER_SIZE){error("BUFFER OVERFLOW");}
          ctime_r(&(statOut->st_mtim.tv_sec),&(buff[i]));
          i += 26;
          bufferAdd(&i,buff,"\n\r",2);
        } // just skip it
        break;
      case ContentType:
        bufferAdd(&i,buff,"Content-Type: ",15);
        getFileFormat(path,contentTypeBuff);
        len = strlen(contentTypeBuff);
        bufferAdd(&i,buff,contentTypeBuff,len);
        bufferAdd(&i,buff,"\n\r",2);

        break;        
      case ContentLength:
        bufferAdd(&i,buff,"Content-Length: ",17);
        sprintf(contentTypeBuff,"%lu",httpInfo.contentSize);
        len = strlen(contentTypeBuff);
        bufferAdd(&i,buff,contentTypeBuff,len);
        bufferAdd(&i,buff,"\n\r",2);
        break;
    }
    headerCount += 1;    
  }
  bufferAdd(&i,buff,"\n\r",2);

  //buff to sock
  write(sock,buff,i);
}
void outputReponse(int sock,char *responseBody,char *path,struct stat *statOut,struct httpReply httpInfo,struct hashTable *table,enum httpRequest request){
  /**********************
   *void respondToRequest(int sock,int fileSocket,enum httpRequest request)
   * responds to a request, the file is known to be valid
   *  int sock - sock to respond with
   *  int fileSocket - file opened
   *  enum httpRequest request - the request sent 
   *
   *
   **********************/
  genHeaders(sock,path,statOut,httpInfo,table,request);
  switch(request){
     case HEAD:
        debug("HEAD");
        break;
      case GET:
        debug("GET");
        break;
      case POST:
        debug("POST");
        break;
      case ERROR:
        error("SERVER ERROR");
      default:
        debug("Unsuported method");
  }
}

int respondFileNotFound(char **responseBody,struct httpReply *httpInfo,struct hashTable *table,enum httpRequest request){
    // add error code to http
    size_t len = strlen(textHtml404)+1;// for null byte
    (*responseBody) = malloc(len*sizeof(char));
    memcpy( (*responseBody),textHtml404,len*sizeof(char));
    httpInfo->statusCode = 404;
    httpInfo->contentSize = len;
    return 1;
}



void respondToRequest(int sock,const char *path,enum httpRequest request,struct hashTable *table){
  /******************
   *int respondToRequest(int sock,enum httpRequest request,struct hashTable *table)
   *  responds to requests
   *  int sock - the socket
   *  enum httpRequest request
   *  struct harshTable *table
   ******************/

  // combine the paths
  struct stat statOut;

  char *responseBody = NULL;
  
  char *requestPath = hashTableGet(table,"PATH");
  char *realPath = malloc(sizeof(char)*(strlen(path)+strlen(requestPath)+1));
  strcpy(realPath,path);
  strcat(realPath,requestPath);
  int file = getFile(realPath,&statOut);

  struct httpReply httpInfo;


  if(file == -1 ){
    debug("FILE NOT FOUND");
    respondFileNotFound(&responseBody,&httpInfo,table,request); 
  }if(file == -2){
    debug("DIR");
    //respondDir(&responseBody,table);
  }else{
    debug("FILE");
    //respondFile(&responseBody,table,file);
  }
  
  outputReponse(sock,responseBody,realPath,&statOut,httpInfo,table,request);

  free(realPath);
  free(responseBody);
}


