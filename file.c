// for files
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <string.h>

// BUFFER SIZE
#include "parser.h"

//debug
#include "logging.h"

// GET THE ERROR PAGE 
extern char _binary_html_error_start[];
extern char _binary_html_end[];
extern size_t _binary_html_error_size;

int getFile(char *path){
  /**************************
   *int getFile(char *path)
   *  path - the file path to try to get, if it can't find the file it will look for an index file
   *  returns -1 on error
   **************************/
  debug(path);
  if(access(path,R_OK)==0){
    return open(path,O_RDONLY); 
  }else{
   return -1;
  }
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


void 

int respondToRequest(int sock,enum httpRequest request,struct hashTable *table){
  /******************
   *int respondToRequest(int sock,enum httpRequest request,struct hashTable *table)
   *  responds to requests
   *  int sock - the socket
   *  enum httpRequest request
   *  struct harshTable *table
   ******************/
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




