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
#define SERVER_NAME "testHttpd0.0"
char *STATUS;

//Return Headers
enum replyHeader {End,Server,Date,ContentType};
enum replyHeader replyHeaderList[] = {Server,Date,ContentType,End};

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

void genHeaders(int sock,struct stat *statOut,struct hashTable *table,enum httpRequest request){
  size_t i = 0;
  size_t headerCount = 0;
  char buff[BUFFER_SIZE];
  // START
  
  // HTTP VERSION
  if(i+strlen(HTTP_VERSION) >= BUFFER_SIZE){error("BUFFER OVERFLOW");}
  memcpy(buff,HTTP_VERSION,strlen(HTTP_VERSION));
  i+=strlen(HTTP_VERSION);

  if(i+3 >= BUFFER_SIZE){error("BUFFER OVERFLOW");}
  memcpy(buff,hashTableGet(table,"STATUS"),3);
  i+=3;
  
  //End,Server,Date,Expires,ContentType
  while(0==0){
    switch(replyHeaderList[headerCount]){
      case Server:

        if(i+8 >= BUFFER_SIZE){error("BUFFER OVERFLOW");}
        memcpy(&(buff[i]),"Server: ",8);
        i += 8;

        if(i+strlen(SERVER_NAME) >= BUFFER_SIZE){error("BUFFER OVERFLOW");}
        memcpy(&(buff[i]),SERVER_NAME,strlen(SERVER_NAME));
        i += strlen(SERVER_NAME);

        if(i+2 >= BUFFER_SIZE){error("BUFFER OVERFLOW");}
        memcpy(&(buff[i]),"\n\r",2);
        i += 2;
        break;
      case Date:
        // time of last access
        if(statOut != NULL){
          if(i+6 >= BUFFER_SIZE){error("BUFFER OVERFLOW");}
          memcpy(&(buff[i]),"Date: ",6);
          i += 6;
          

          if(i+26 >= BUFFER_SIZE){error("BUFFER OVERFLOW");}
          ctime_r(&(statOut->st_mtim.tv_sec),&(buff[i]));
          i += 26;
        } // just skip it
      case ContentType:
        memcpy(&(buff[i]),"ContentType: ",14);
        i+=14;
        break;
      default: // Catchall should hopefully stop some sort of error somewhere
        goto loopExit;
    }
    headerCount += 1;    
  }
  loopExit: ; // jump to break loop
  if(i+2 >= BUFFER_SIZE){error("BUFFER OVERFLOW");}
  memcpy(&(buff[i]),"\n\r",2);
  i += 2;

  //buff to sock
  write(sock,buff,i);
}
void outputReponse(int sock,char *responseBody,struct stat *statOut,struct hashTable *table,enum httpRequest request){
  /**********************
   *void respondToRequest(int sock,int fileSocket,enum httpRequest request)
   * responds to a request, the file is known to be valid
   *  int sock - sock to respond with
   *  int fileSocket - file opened
   *  enum httpRequest request - the request sent 
   *
   *
   **********************/
  genHeaders(sock,statOut,table,request);
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



void respondToRequest(int sock,const char *path,enum httpRequest request,struct hashTable *table){
  /******************
   *int respondToRequest(int sock,enum httpRequest request,struct hashTable *table)
   *  responds to requests
   *  int sock - the socket
   *  enum httpRequest request
   *  struct harshTable *table
   ******************/

  // set grobal
  STATUS = malloc(7*sizeof(char));
  strToHeap("STATUS",&STATUS,6); // this should be fine

  // combine the paths
  struct stat statOut;

  char *responseBody = NULL;
  
  char *requestPath = hashTableGet(table,"PATH");
  char *realPath = malloc(sizeof(char)*(strlen(path)+strlen(requestPath)+1));
  strcpy(realPath,path);
  strcat(realPath,requestPath);
  int file = getFile(realPath,&statOut);

  free(realPath);

  if(file == -1 ){
    debug("FILE NOT FOUND");
    respondFileNotFound(&responseBody,table,request); 
  }if(file == -2){
    debug("DIR");
    //respondDir(&responseBody,table);
  }else{
    debug("FILE");
    //respondFile(&responseBody,table,file);
  }
  
  outputReponse(sock,responseBody,&statOut,table,request);

  free(responseBody);
}


