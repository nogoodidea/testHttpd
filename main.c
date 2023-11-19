/* nogoodideas 2023
 * Goals:
 *   Implement HTTP 1.1 Complience (rfc9112 rfc9110)
 *   Speed
 *   Multi Connection Support
 *   	forking or threads
 *   ipv4/ipv6
 *   path blocking (/../../../passwd)
 *   	chroot() might help assuming there are no rce problems
 *   	
 *   Implement API Paths and Respeonces
 *   	Idealy no user inputs (strings)
 *   	Service Status
 *   		Logs
 *   	Service Start,Restart,Shutdown
 *   	Update Service
 *   		curl / unzip / restart
 * Not Goals:
 * 	SSL Support (proxy with an other server)
 * 	Auth Support (same deal)
 *
 *
 *
 * Resources:
 * 	https://unixism.net/2019/04/linux-applications-performance-introduction/
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>

// memory map
#include <unistd.h>
#include <sys/mman.h>

// socket lib
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

// errors
#include "logging.h"

//poll
#include <poll.h>

// tree
#include "tree.h"

// header parser
#include "parser.h"


// global struct 
typedef struct {
  bool exit;
} t_global;

// GLOBAL
t_global GLOBAL;

// ints the global struct
void initGlobal(){
  GLOBAL.exit = true;
}


//does the socket reading
ssize_t readSocket(int sock,char buf[],ssize_t bufLen){
  return recv(sock,buf,bufLen,0);
}

// socket server the threaded part
void handleConnection(int sock){
  // THE BUFFER
  // loops over request copys line from main buffer to line buf 
  char buf[BUFFER_SIZE];
  ssize_t bufLen = 1; // can't be started at zero
  struct t_treeNode node;
  while(bufLen != 0){
    bufLen = readSocket(sock,buf,sizeof(buf));
    if(bufLen == -1){//error handler
      socketError(); 
    }
    // handles a socket
    int headerStatus = parseHeaders(buf,bufLen,&node);
    if(headerStatus != 0){
      printNodes(node);
    }
  }
  // react to the headers
}

// socket listener start
int initListenSocket(int port){
  struct sockaddr_in sockAddr;
  
  // socket
  int sock = socket(PF_INET,SOCK_STREAM,0);
  if(sock == -1){
    error("socket creation failed");
  }

  int enable =1;
  if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int)) < 0){
	  error("socket option reuse addr failed");
  }

  bzero(&sockAddr,sizeof(sockAddr)); 
  // connection socket
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = htons(port);
  sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind
  if(bind(sock,(struct sockaddr*)&sockAddr,sizeof(sockAddr))<0){
	  error("socket binding failed");
  }
  
  if(listen(sock,10) < 0){
    error("socket set queue amount failed");
  }
  
  return (sock);
}

// main listenSocket loop
void listenSocketLoop(int sockListen){ 
  int newSock;
  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  pid_t pid;
  while(1){
    newSock = accept(sockListen,(struct sockaddr*) &clientAddr,&clientAddrLen);
    if(newSock ==-1){
      error("socket accept failed");
    }

    //focking block
    pid = fork();

    if(pid == 0){
      //fork
      debug("Forking");
      handleConnection(newSock);
      close(newSock);
      exit(0);
    }else{
      // parent
      close(newSock);
    }
  }
}

// main thread controler
int main(int argc,char **argv){
  initGlobal();
  
  int port;
  if (argc > 1){
  	port = atoi(argv[1]);
  }else {
	  port = 8000;
  }

  int listenSocket = initListenSocket(port); 
  debug("listenSocket is up");
  listenSocketLoop(listenSocket);

  //exit
  close(listenSocket);
  exit(0);
}
