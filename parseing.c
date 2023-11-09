/*
 * HTTP MESSAGE PARSER
 *
*/

#include <string.h>
#include "tree.h"

#define lineBreak "\n\r"




int parseHeaders(char buff[BUFFER_SIZE],ssize_t buffSize,struct t_treeHead head,int reset){
  /*******************
   * int parseHeaders()
   *    buff[BUFFER_SIZE] - char array with http text
   *    ssize_t buffSize  - size of used buffer
   *    int reset - reset the static char
   *  
   *  returns the amount of chars left
   *  return 0 for the next buffer
   *  returns negative number of chars left for the http end
   *
   *  keeps 2 static varables
   *  carryOver carryes unresolved strings from the previus function call
   *  staus bit flags, 
   *  1
   *  AFTER_COLON 2
   *  NEWLINE  4
   *  CARRIAGE_RETURN  8
   *    16
   *    32
   *    64
   *    128
   *    256
   *    512
   *    1024
   *******************/
   static char *carryOver = NULL;// pointer to carryed over values for keys
   static unsigned int status = 0;

   for(ssize_t i = 0;i<buffSiz;i+=1){
    if((status & 4)==4 &&buff[i] == ':'){
      
    }
    if(){

    }
    if(){

    }
   }
}
