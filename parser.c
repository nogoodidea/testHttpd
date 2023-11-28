/*
 * HTTP MESSAGE PARSER
 *
*/

#include <stdlib.h>
#include <string.h>
#include "tree.h"


//error
#include "logging.h"

//defines

#define BUFFER_SIZE 2048

#define lineBreak "\n\r"

#define CARRYOVER 1
#define AFTER_COLON 2
#define NEWLINE 4
#define CARRIAGE_RETURN 8
#define FIRSTLINE 16


#define AFTER_SPACE 1

const char **percentEncoding = ["20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2F","3A","3B","3D","3F","40","5B","5D"];
const char *percentDecoding = " !\"#$%&\'()*+,/:;=?@[]"

//datatypes
enum httpRequest {GET,POST,HEAD,ERROR};

int parseHeaders(char buff[BUFFER_SIZE],ssize_t buffSize,struct t_treeNode **head){
  /*******************
   * int parseHeaders()
   *    buff[BUFFER_SIZE] - char array with http text
   *    ssize_t buffSize  - size of used buffer
   *  
   *  returns the amount of chars left
   *  returns negative number of chars left for the http end
   *
   *  keeps 2 static varables
   *  carryOver carryes unresolved strings from the previus function call
   *  staus bit flags, 
   *  CARRYOVER 1
   *  AFTER_COLON 2
   *  NEWLINE  4
   *  CARRIAGE_RETURN  8
   *  FIRSTLINE  16
   *  32
   *    64
   *    128
   *    256
   *    512
   *    1024
   *******************/

   static char carryOver[BUFFER_SIZE];// pointer to carryed over values for keys
   static unsigned int status = FIRSTLINE;
   static unsigned int total;
   static unsigned int valueLen;
   static unsigned int keyLen;

   char *key = NULL;
   char *value = NULL;

   static ssize_t i = 0;

   // handle carry over
   if(CARRYOVER && i < BUFFER_SIZE){
    status &= ~CARRYOVER;
    // we assume i is defined 
   }else{
    // no carryover or we can't deal with overflow 
    i = 0;
    total = 0;
    valueLen = 0;
    keyLen = 0;
   }


   for(;i<buffSize;i+=1){
     
    if(buff[i] == ':'){
      status |= AFTER_COLON;
    }
    if(buff[i] == '\r'){
      status |= CARRIAGE_RETURN; 
      continue; // don't want to add \n to bufferTemp
    }
    if( ((status&CARRIAGE_RETURN)==CARRIAGE_RETURN) && (buff[i] == '\n' )){ // NEW BLOCK
      //remove some bit flags
      status &= ~(AFTER_COLON|CARRIAGE_RETURN|NEWLINE);
     
      if(keyLen > 0){
        key = (char*)malloc(sizeof(char)*(keyLen+1));
        memcpy(key,&(carryOver[0]),keyLen);
        key[keyLen] = '\0';
      }

      if(valueLen > 0){
        value = (char*)malloc(sizeof(char)*(valueLen+1));
        memcpy(value,&(carryOver[keyLen+2]),valueLen);
        value[valueLen] = '\0';
      }

      if(0<total){
        if((status&FIRSTLINE)==FIRSTLINE){
          status &= ~(FIRSTLINE);
          addNode(head,"REQUEST",key);
        }else{
          addNode(head,key,value);
        }
      }else{
        // got 2 newlines after each other
        return (i-total)-1;
      }
      total = 0;
      valueLen = 0;
      keyLen = 0;

      continue; // don't want to add \r to bufferTemp
    }else{
      status &= ~CARRIAGE_RETURN;
    }
    carryOver[total] = buff[i];
    if(total >= BUFFER_SIZE){
      error("BUFFER OVERFLOW IN PARSER");
    }

    total +=1;
    if((status & AFTER_COLON) != AFTER_COLON){
      //amount of chars before colon
      keyLen +=1;
    }else{
      valueLen +=1;
    }
   }
   
   if(total != 0){
    status |=CARRYOVER;
    debug("Carrying over");
   }
   return total; // TODO return the right thing
}

char percentDecode(char *code){
  /******************
   * char percentDecode(char *code)
   *  code -  the 2 numbers to decode from %encodeing
   *
   *
   *
   ******************/
  size_t midpoint = 11; // midpoint
  size_t start = 0;
  size_t end = 0;
  

  while( 0!=(out = strcmp(code,percentEncoding[midpoint])) ){
    if(out < 0){
      start = midpoint;
    }else{
      end = midpoint;
    }
    midpoint = (start+end)/2;
        
    }
  }
  return percentDecoding[midpoint];
}



enum httpRequest parseRequest(struct t_treeNode *head, char **requestPath){
  /***********************
   * return enum httpRequest()
   *    struct t_nodeTree root - 
   *    char **requestPath - the path of a request, will need to be freed if it's not NULL also should be defalt at NULL cause i an't freeing shit
   *     
   *    
   * parses a http request 
   *
   **********************/
  char *item = searchTree(head,"REQUEST");
  debug(item);

  unsigned int count = 0;
  unsigned int status = 0;

  size_t i = 0;
  while(item[i] != NULL){
    
    if(item[i] == ' '){
      count += 1;
    }

    if( (status & AFTER_SPACE) == AFTER_SPACE ){
      if(item[i] == ' '){
        
      }

    }
    i+=1;// incroment
  }
  

  return ERROR;

}
