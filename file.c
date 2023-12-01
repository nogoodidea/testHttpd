// for files
#include <stdio.h>
#include <dirent.h>

#include <string.h>

char *getFileFormat(char *fileName){
  /***********************
   *char *getFileFormat(char *fileName)
   * char *fileName - the name of the file
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



