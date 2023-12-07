/*
 * HTTP MESSAGE PARSER
 *
*/

#include <stdlib.h>
#include <string.h>
#include "hashTable.h"


//error
#include "logging.h"

//defines

#define BUFFER_SIZE 256

#define lineBreak "\n\r"

#define CARRYOVER 1
#define AFTER_COLON 2
#define NEWLINE 4
#define CARRIAGE_RETURN 8
#define FIRSTLINE 16


// for ascii encoding
#define MIN_PERCENT_ENCODING 32
#define MAX_PERCENT_ENCODING 126

#define PARSE_HTTP_METHOD 1
#define PERCENT 2
#define PERCENT_ZERO 4
#define PERCENT_ONE 8



//datatypes
enum httpRequest {GET,POST,HEAD,ERROR};
  

void strToHeap(char *data,char **out,size_t length){
  /*************
   *void strToHeap(char *data,char *out,size_t length)
   *  char *data - the string of data, should not be null terminated as one is added
   *  char **out - the pointer to the out, 
   *  size_t length - the length of the outstring
   *************/
  *out = (char*)malloc(sizeof(char)*(length+1));
  memcpy(*out,data,length);
  (*out)[length] = '\0';
}

enum httpRequest parseHttpMethod(char *method){
  /******************
   * enum httpRequest parseHttpMethod(char *method)
   *   char *method - the method to parse to see if it's supported
   ******************/
  if(strcmp("GET",method) == 0){
    return GET; 
  }
  if(strcmp("POST",method) == 0){
    return POST;
  }
  if(strcmp("HEAD",method) == 0){
    return HEAD;
  }
  return ERROR;
}

char *rectrictPath(char *path){
  /**************************
   * char *rectrictPath(char *path)
   *  path - the path to find a file, it will be truncated and the first backslash after any backtraseing will be removed
   **************************/
  
  size_t o = 0;
  size_t i = 0;
  char pathBuff[BUFFER_SIZE];
  

  if(path[0] == '/'){
    i = 1;
  }

  for(;path[i] != '\0';i+=1){
    if(i >= BUFFER_SIZE){
      error("buffer overflow in rectrictPath");
    }
    if(strncmp(&(path[i]),"../",sizeof(char)*3) == 0){
      i+=3;
      continue;
    }

    pathBuff[o] = path[i];
    o+=1;
  }

  char *out = malloc(sizeof(char)*(o+1));
  memcpy(out,pathBuff,o+1);
  out[o] = '\0';

  return out;
}

char percentDecode(char *code){
  /******************
   * char percentDecode(char *code)
   *  code -  the 2 numbers to decode from %encodeing
   ******************/
  char hex[5] = "0x\0"; 
  strcat(hex,code); // assumes 2 chars
  int out = atoi(hex);

  if( (MIN_PERCENT_ENCODING <= out) && (out <= MAX_PERCENT_ENCODING) ){
    return '\0'+out;
  }
  return '\0';
}

void parseFirstline(char buff[BUFFER_SIZE],ssize_t buffSize,char **path,enum httpRequest *request){
  /*****************
   * parses the first line, sets request and returns the path
   *
   *****************/
  char lineBuff[BUFFER_SIZE];
  char hexBuff[3] = "\0\0\0";

  unsigned short int status = PARSE_HTTP_METHOD;
  int count = 0; // amount of spaces controles section parsed

  size_t o = 0; // controler needed to do proper
  for(size_t i = 0; i < buffSize; i += 1){
    if(buff[i] == ' '){
      count +=  1;
      continue;
    }
    if(count == 1){
      if((status & PARSE_HTTP_METHOD)){
        *request = parseHttpMethod(buff);
        status &= ~(PARSE_HTTP_METHOD);

      }
      if(buff[i] == '%'){
        status |= PERCENT;
      }
      switch(status){
        case PERCENT:
          status |=PERCENT_ZERO;
          status &= ~PERCENT;
          break;
        case PERCENT_ZERO:
          hexBuff[0] = buff[i];
          status |= PERCENT_ONE;
          status &= ~PERCENT_ZERO;
          break;
        case PERCENT_ONE:
          status &= ~PERCENT_ONE;
          hexBuff[1] = buff[i];
          lineBuff[o] = percentDecode(hexBuff);  
          o+=1;
          break;
        default:
          lineBuff[o] = buff[i];
          o+=1;
          break;
       }
     }
   }
   lineBuff[o] = '\0';
   (*path) = rectrictPath(lineBuff);
}

int parseHeaders(char buff[BUFFER_SIZE],ssize_t buffSize,struct hashTable *table,enum httpRequest *request){
  /*******************
   * int parseHeaders()
   *    buff[BUFFER_SIZE] - char array with http text
   *    ssize_t buffSize  - size of used buffer
   *    struct hashTable *table - properts added
   *    enum httpRequest *request - the request
   *  
   *  returns the amount of chars left
   *  returns negative number of chars left for the http end
   *
   *  TODO Fix n^3 time complexety 
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
   static short unsigned int status = FIRSTLINE;
   static unsigned int total;
   static unsigned int valueLen;
   static unsigned int keyLen;

   char *key = NULL;
   char *value = NULL;

   static ssize_t i = 0;

   // set request to error if new
   if((status&FIRSTLINE)==FIRSTLINE){
     *request = ERROR;
   }

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

   // if the buffer is done exit
   if(buffSize==0){
    return 0;
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
     
      if( ((status & FIRSTLINE)== FIRSTLINE ) ){
        // FIRST LINE HANDLER
        parseFirstline(buff,keyLen,&key,request);

      }else{
          if(keyLen > 0){
            strToHeap(&(carryOver[0]),&key,keyLen);
        }
      }

      if(valueLen > 0){
        strToHeap(&(carryOver[keyLen+2]),&value,valueLen);
      }

      if(0<total){
        if((status&FIRSTLINE)==FIRSTLINE){
          status &= ~(FIRSTLINE);
          char *path;
          strToHeap("PATH",&path,4);
          hashTableAdd(table,path,key);
        }else{
          hashTableAdd(table,key,value);
        }
      }else{
        // got 2 newlines after each other
        return (i-total);
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



