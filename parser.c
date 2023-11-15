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

int parseHeaders(char buff[BUFFER_SIZE],ssize_t buffSize,struct t_treeNode *head){
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
   static unsigned int total=0;
   static unsigned int valueLen=0;
   static unsigned int keyLen=0;

   char *key = NULL;
   char *value = NULL;

   // handle carry over
   if(CARRYOVER){
    // TODO handle
    status &= ~CARRYOVER;
   }

   

   for(ssize_t i = 0;i<buffSize;i+=1){
    
    if((status & AFTER_COLON)!= AFTER_COLON && buff[i] == ':'){
      status |= AFTER_COLON;   
    }
    if(buff[i] == '\r'){
      status |= CARRIAGE_RETURN; 
      continue; // don't want to add \n to bufferTemp
    }
    if(((status&CARRIAGE_RETURN)==CARRIAGE_RETURN)&&(buff[i] == '\n')){ // NEW BLOCK
      status &= ~(AFTER_COLON|CARRIAGE_RETURN|NEWLINE);
      debug("PARSER");
      // add null left
      
      key = (char*)malloc(sizeof(char)*(keyLen+1));
      value = (char*)malloc(sizeof(char)*(valueLen+1));
      
      memcpy(key,&(carryOver[0]),keyLen);
      memcpy(value,&(carryOver[keyLen]),valueLen);

      key[keyLen] = '\0';
      value[valueLen] = '\0';

      if(0<total){
        addNode(head,key,value);
        debug(key);
        debug(value);
      }else{
        // got 2 newlines after each other
        return i-total;
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
      error("ERROR: BUFFER OVERFLOW IN PARSER");
    }

    total +=1;
    if((status & AFTER_COLON) != AFTER_COLON){
      //amount of chars before colon
      valueLen +=1;
    }else{
      //
      keyLen +=1;
    }
   }
   
   if(total != 0){
    status |=CARRYOVER;
   }
   return total; // TODO return the right thing
}
