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

// sever name
#define SERVER_NAME "testHttpd0.0"

struct httpReply {
  int statusCode;
  char *statusText;
  size_t statusTextLen;
  char * contentType;
  size_t contentTypeLen;
  size_t contentSize;
  //for chunked tranpher of data
  bool chukedEncoding;
};

//Return Headers
enum replyHeader {End,Server,ContentType,ContentLength,Date};
const enum replyHeader replyHeaderList[] = {Server,ContentType,ContentLength,Date,End};

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

void intHttpReply(struct httpReply *httpStatus){
  /***************
   *  void intHttpReply(struct httpReply *httpStatus)
   *    struct httpReply the struct to int
   *    sets default values for the httpReply struct
   ***************/
  httpStatus->statusCode = 500;
  httpStatus->statusText = NULL;
  httpStatus->statusTextLen = 0;
  httpStatus->contentType = NULL;
  httpStatus->contentTypeLen = 0;
  httpStatus->contentSize = 0;
  httpStatus->chukedEncoding = false;
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

void sendHeaders(int sock,struct httpReply httpInfo){
  /********************
   *
   *
   * Generates headers for transmission
   *
   *
   *
   ********************/
  size_t i = 0;
  size_t headerCount = 0;
  size_t len = 0;
  char buff[BUFFER_SIZE];
  char tempBuff[64];
  printf("\nbuff:%p\n",buff);
  // START
  
  // HTTP VERSION
  bufferAdd(&i,buff,HTTP_VERSION,strlen(HTTP_VERSION));

  //edge case
  sprintf(tempBuff,"%i",httpInfo.statusCode);
  bufferAdd(&i,buff,tempBuff,3);
  bufferAdd(&i,buff," ",1);
  bufferAdd(&i,buff,httpInfo.statusText,httpInfo.statusTextLen);

  bufferAdd(&i,buff,"\r\n",2);

  //End,Server,Date,Expires,ContentType
  while(replyHeaderList[headerCount] != End){
    switch(replyHeaderList[headerCount]){
      case Server:
        bufferAdd(&i,buff,"Server: ",8);
        bufferAdd(&i,buff,SERVER_NAME,strlen(SERVER_NAME));
        break;
      case Date:
          bufferAdd(&i,buff,"Date: ",6);
          time_t date = time(NULL);
          strftime(tempBuff,64,"%a, %d %b %Y %h:%m:%s GMT",gmtime(&date));
          bufferAdd(&i,buff,tempBuff,26);
          break;
      case ContentType:
        bufferAdd(&i,buff,"Content-Type: ",14);
        // if you don't know guess
        if(httpInfo.contentType == NULL){
          bufferAdd(&i,buff,"application/octet-stream",24);
        }else{
          bufferAdd(&i,buff,httpInfo.contentType,httpInfo.contentTypeLen);
        }
        break;        
      case ContentLength:
        if(!httpInfo.chukedEncoding){
          bufferAdd(&i,buff,"Content-Length: ",16);
          sprintf(tempBuff,"%lu",httpInfo.contentSize);
          len = strlen(tempBuff);
          bufferAdd(&i,buff,tempBuff,len);
        }// checked Encoding makes me want to shoot myself
        else{
        bufferAdd(&i,buff,"Transfer-Encoding: chunked",26);
        }
        break;
      default:
        error("ERROR IN HEADER PARSER");
    }
    bufferAdd(&i,buff,"\r\n",2);
    headerCount += 1;    
  }
  bufferAdd(&i,buff,"\r\n",2);

  //buff to sock
  write(sock,buff,i);
}

void sendBody(int sock,char responceBody[BUFFER_SIZE],struct httpReply httpInfo,int file){
  /***************
   *
   ***************/
  char buff[64];
  if(httpInfo.chukedEncoding){
    while(true){
      sprintf(buff,"%lx",httpInfo.contentSize);
      debug(buff);
      write(sock,buff,strlen(buff));
      write(sock,"\r\n",2);
      write(sock,responceBody,httpInfo.contentSize);
      write(sock,"\r\n",2);
      if(httpInfo.contentSize == 0){break;}
      httpInfo.contentSize = read(file,responceBody,BUFFER_SIZE);
    }
  }else{
    write(sock,responceBody,httpInfo.contentSize);
  }
}

void sendResponse(int sock,char responseBody[BUFFER_SIZE],char *path,struct stat *statOut,struct httpReply httpInfo,struct hashTable *table,enum httpRequest request,int file){
  /**********************
   *void respondToRequest(int sock,int fileSocket,enum httpRequest request)
   * responds to a request, the file is known to be valid
   *  int sock - sock to respond with
   *  int fileSocket - file opened
   *  enum httpRequest request - the request sent 
   *
   *
   **********************/
  sendHeaders(sock,httpInfo);
  switch(request){
     case HEAD:
        break;
      case GET:
      case POST:
        sendBody(sock,responseBody,httpInfo,file);
        break;
      case ERROR:
        error("SERVER ERROR");
      default:
      error("Unseported Method");
  }
}

void respondFileNotFound(char *responseBody,struct httpReply *httpInfo){
    // add error code to http
    size_t len = strlen(textHtml404);// for null byte
    memcpy( responseBody,textHtml404,len*sizeof(char));
    httpInfo->statusCode = 404;
    httpInfo->statusText = "Page Not Found";
    httpInfo->statusTextLen =14;
    httpInfo->contentType = "text/html";
    httpInfo->contentTypeLen = 9;
    httpInfo->contentSize = len;
}

void respondFile(char *responseBody,struct httpReply *httpInfo,struct stat fileStat,char *path,int file){
  char buff[64];
  getFileFormat(path,buff);
  size_t len = strlen(buff);


  httpInfo->statusCode = 200;
  httpInfo->statusText = "OK";
  httpInfo->statusTextLen=2;
  // chuncked data transpher
  if(fileStat.st_size > BUFFER_SIZE){
    httpInfo->chukedEncoding = true;
  }
  httpInfo->contentSize=read(file,responseBody,BUFFER_SIZE);
  // contentTypeLen
  httpInfo->contentType = malloc(len*sizeof(char)+1);
  strcpy(httpInfo->contentType,buff);
  httpInfo->contentTypeLen = len;
}

void respondDir(char (*responseBody)[BUFFER_SIZE],struct httpReply *httpInfo,int file){
  // it should work    

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

  char responseBody[BUFFER_SIZE];

  char *realPath = NULL;
  char *requestPath = NULL;
  
  if(hashTableHas(table,"PATH")==true){
    requestPath = hashTableGet(table,"PATH");
    realPath = malloc(sizeof(char)*(strlen(path)+strlen(requestPath)+1));
    strcpy(realPath,path);
    strcat(realPath,requestPath);
  }
    
  int file = getFile(realPath,&statOut);

  struct httpReply httpInfo;
  intHttpReply(&httpInfo);


  if(file == -1 ){
    debug("FILE NOT FOUND");
    respondFileNotFound(responseBody,&httpInfo); 
  }else{
    if(file == -2){
      debug("DIR");
      respondDir(&responseBody,&httpInfo,file);
    }else{
      debug("FILE");
      respondFile(responseBody,&httpInfo,statOut,requestPath,file);
    }
  }
  
  sendResponse(sock,responseBody,realPath,&statOut,httpInfo,table,request,file);

  free(realPath);
}


