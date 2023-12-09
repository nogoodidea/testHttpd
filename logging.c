/* DOES LOGING AND ERROR STUFF
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// ERROR HANDLER
void error(char *err){
  fputs("ERROR: ",stderr);
  fputs(err,stderr);
  fputs("\n",stderr);
  exit(1);
}

void debug(char *msg){
  fputs("DEBUG:",stderr);
  fputs(msg,stderr);
  fputs("\n",stderr);
}

void debugChar(char c){
  fputs("DEBUG: ",stderr);
  fputc(c,stderr);
  fputc('\n',stderr);
}

void debugInt(int i){
  fprintf(stderr,"DEBUG: %i\n",i);
}

// errorno handler
void socketError(){
  switch(errno){
    case EAGAIN:
      debug("socket is nonblocking and operation will block");
      break;
    case EBADF:
      error("socket is not a vaid fd");
      break;
    case ECONNREFUSED:
      error("socket connection was refused");
      break;
    case EFAULT:
      error("buffer pointer outside of process's address space");
      break;
    case EINTR:
      error("socket recv call was interrupted by delivery");
      break;
    case EINVAL:
      error("invilid argument passed");
      break;
    case ENOMEM:
      error("kmem alloc failed for recvmsg");
      break;
    case ENOTCONN:
      error("socket not connnected");
      break;
    case ENOTSOCK:
      error("socket not a socket");
      break;
  }
}

void debugIsCommandChar(char c){
  switch(c){
    case '\n':
      debug("NEWLINE");
      break;
    case '\r':
      debug("CARAGE RETURN");
      break;
    case ':':
      debug("COLLON");
      break;
  }

}
