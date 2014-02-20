#ifndef _WS_H_
#define _WS_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#include<setjmp.h>
#include<signal.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<errno.h>
#include<math.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define  NTHREADS        100
#define  SBUFSIZE        1024
#define  MAXLINE         8192
#define  RIO_BUFSIZE     8192
#define  LISTENQ         1024
#define  MAXBUF          8192 

typedef struct sockaddr SA; 
 
#endif
