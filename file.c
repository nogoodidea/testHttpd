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

// BUFFER SIZE
#include "parser.h"

//debug
#include "logging.h"

// error 
#include "text.h"

// HTTP VERSION
#define HTTP_VERSION "HTTP/1.1"

//
#define SERVER_NAME testHttpd0.0
char *STATUS = NULL;

//Return Headers
enum replyHeader {End,Server,Date,Expires,ContentType};
enum replyHeader replyHeaderList[] = {Date,Server,ContentType,Expires,End};

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

char *getFileFormat(char *fileName){
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
    return "text/plain";
  }
  if(strcmp(".css",&(fileName[dot]))){
    return "text/css";
  }
  if(strcmp(".html",&(fileName[dot]))){
    return "text/html";
  }
  if(strcmp(".htm",&(fileName[dot]))){
    return "text/html";
  }
  if(strcmp(".js",&(fileName[dot]))){
    return "text/javascript";
  }
  if(strcmp(".md",&(fileName[dot]))){
    return "text/markdown";
  }
  //IMAGES
  if(strcmp(".apng",&(fileName[dot]))){
    return "image/apng";
  }
  if(strcmp(".avif",&(fileName[dot]))){
    return "image/avif";
  }
  if(strcmp(".gif",&(fileName[dot]))){
    return "image/gif";
  }
  if(strcmp(".png",&(fileName[dot]))){
    return "image/png";
  }
  if(strcmp(".svg",&(fileName[dot]))){
    return "image/svg+xml";
  }
  if(strcmp(".webp",&(fileName[dot]))){
    return "image/webp";
  }

  // we got to the end say fuck it
  return "application/octet-stream";
}

void outputReponse(int sock,char *responseBody,struct hashTable *table,enum httpRequest request){
  /**********************
   *void respondToRequest(int sock,int fileSocket,enum httpRequest request)
   * responds to a request, the file is known to be valid
   *  int sock - sock to respond with
   *  int fileSocket - file opened
   *  enum httpRequest request - the request sent 
   *
   *
   **********************/
  switch(request){
     /*case HEAD:
        debug("HEAD");
        break;*/
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

int respondFileNotFound(char **responseBody,struct hashTable *table,enum httpRequest request){
    char *code;
    strToHeap("404",&code,3);
    // add error code to http
    hashTableAdd(table,STATUS,code);
    size_t len = strlen(textHtml404)+1;// for null byte
    (*responseBody) = malloc(len*sizeof(char));
    memcpy( (*responseBody),textHtml404,len*sizeof(char));
    return 1;
}

void genHeaders(int sock,struct hashTable *table,enum httpRequest request){
  size_t i = 0;
  size_t len = 0;
  char buff[BUFFER_SIZE];
  //End,Server,Date,Expires,ContentType
  while(0==0){
    switch(replyHeaderList[i]){
      case Server:
        memcpy(buff,"Server: ",8);
        buff[7] = '\0';
        debug(buff);
        break;
      case Date:
        break;
      case Expires:
        break;
      case ContentType:
        break;
      default: // Catchall should hopefully stop some sort of error somewhere
        goto loopExit;
    }
    i += 1;    
  }
loopExit: ; // jump to break loop
}

void respondToRequest(int sock,const char *path,enum httpRequest request,struct hashTable *table){
  /******************
   *int respondToRequest(int sock,enum httpRequest request,struct hashTable *table)
   *  responds to requests
   *  int sock - the socket
   *  enum httpRequest request
   *  struct harshTable *table
   ******************/

  // set grobal
  strToHeap("STATUS",&STATUS,6);

  // combine the paths
  struct stat statOut;

  char *responseBody = NULL;
  
  char *requestPath = hashTableGet(table,"PATH");
  char *realPath = malloc(sizeof(char)*(strlen(path)+strlen(requestPath)+1));
  strcpy(realPath,path);
  strcat(realPath,requestPath);
  int file = getFile(realPath,&statOut);

  free(realPath);
  
  respondFileNotFound(&responseBody,table,request); 

  /*if(file == -1 ){
    respondFileNotFound(responseBody,table,request); 
  }if(file == -2){
    respondDir(responseBody,table);
  }else{
    respondFile(responseBody,table,file);
  }*/
  
  outputReponse(sock,responseBody,table,request);
  free(responseBody);
}


