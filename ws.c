#include "ws.h"
#include "sock.h"
#include "rio.h"
#include "syn.h"

void *thread(void *vargp );    /* thread routine */
void doit(int connfd );        /* serve the client's request */
void read_requesthdrs(rio_t *rp);     /* process the request's headers */
void get_filetype(char *filename, char *filetype);    /* get the file type */
int parse_url(char *url, char *filename, char *cgiargs);     /* parse URL */
void serve_static(int fd, char *filename, int filesize);     /* static service */
void serve_dynamic(int fd, char *filename, char *cgiargs);   /* dynamic service */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);    /* error response */

sbuf_t sbuf;     /* connection buffer pool */

int main(int argc, char *argv[] )
{
    int listenfd, connfd, port;
    unsigned int clientlen = sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    pthread_t tid;
    int ret, i;    /* return-value, index */

    if(argc != 2){    /* arg number != 2 ? */
        fprintf(stderr, "usage: %s <port>\n", argv[0]);    /* e.t. usage: name 1234 */
        exit(1);
    }
    port = atoi(argv[1]);    /* numeric port */
    
    sbuf_init(&sbuf, SBUFSIZE);    /* init connection buffer */
    listenfd = open_listenfd(port);   /* open listen port and get listen fd */

    for(i = 0; i < NTHREADS; i++){   /* create worker threads */
        ret = pthread_create(&tid, NULL, thread, NULL);
        if(ret != 0){
            fprintf(stderr,  "create worker thread %d failed. \n", i);
        }
    }
    while(1){
        connfd = accept(listenfd, (SA*)&clientaddr, &clientlen);   /* get request from client by IPv4 network */
        printf("client (%s:%d) has established connection, and connfd is %d\n",
               inet_ntoa(clientaddr.sin_addr),    /* client's IP address */
               ntohs(clientaddr.sin_port), connfd);    /* client's port */
        sbuf_insert(&sbuf, connfd);    /* insert connfd into buffer */
    }
    return 0;
}

void *thread(void *vargp)   /* thread routine */
{
    pthread_detach(pthread_self());   /* detach thread, and clean thread automatically */
    while(1){
        int connfd = sbuf_remove(&sbuf);     /* remove connfd from buffer */
        doit(connfd);                       /* serve client */
        close(connfd);                      /* close connfd */
        printf("connfd %d has closed.\n", connfd);
    }
}

void doit(int fd)     /* support service through fd */
{
    int is_static;   /* is static page ? */
    struct stat sbuf;   /* file stat */
    char buf[MAXLINE], method[MAXLINE], url[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;    /* RIO buffer, not standard I/O */

    rio_readinitb(&rio, fd);    /* init RIO buffer */
    
    rio_readlineb(&rio, buf, MAXLINE );   /* read the request line */
    sscanf(buf, "%s %s %s", method, url, version);    /* method, url, version */
    printf("%s", buf);
    
    if( strcasecmp(method, "GET") ){    /* method is not GET ? */
        clienterror(fd, method, "501", "Not implemented", "ws does not implement this method except GET");
        return ;
    }
    read_requesthdrs(&rio);   /* process request headers */
    
    /* parse url */
    is_static = parse_url( url, filename, cgiargs );
    /* response */
    if( stat(filename, &sbuf) < 0 ){  /* file existing ? */
        clienterror( fd, filename, "404", "Not Found",
                   "ws could't find this file");
        return ;
    }
    if(is_static){   /* is static page ? */
        if( !(S_ISREG( sbuf.st_mode )) || !(S_IRUSR & sbuf.st_mode) ){  /* don't have permission? */
        clienterror( fd, filename, "403", "Forbidden",
                    "ws couldn't read this file");
             return ;
        }
        serve_static( fd, filename, sbuf.st_size);   /* static page service */
    }else{ /* dynamic page */
        if( !(S_ISREG( sbuf.st_mode )) || !(S_IXUSR & sbuf.st_mode) ){  /* don't have permission? */
            clienterror( fd, filename, "403", "Forbidden",
                       "ws couldn't read this file");
            return ;
        }
        serve_dynamic( fd, filename, cgiargs);  /* dynamic page service*/
    }
}

void clienterror( int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
    char buf[MAXLINE], body[MAXBUF];

    /* build the HTTP response body */
    sprintf( body , "<html><title>ws:a simple server</title>");
    sprintf( body, "%s<body bgcolor=""fffff"">\r\n", body);
    sprintf( body, "%s%s:%s\r\n", body, errnum, shortmsg );
    sprintf( body, "%s%s:%s:\r\n", body, longmsg, cause );
    sprintf( body, "%s<hr><em>a simpler web server</em>\r\n", body);

   /* print the HTTP response */
    sprintf( buf, "HTTP/1.0 %s %s \r\n", errnum, shortmsg);
    rio_writen( fd, buf, strlen(buf) );
    sprintf( buf, "Content-Type: text/html\r\n");
    rio_writen(fd,buf, strlen(buf) );
    sprintf( buf, "Content-length: %ld\r\n\r\n", strlen(body) );
    rio_writen( fd, buf, strlen(buf) );

    /* print the response body */
    rio_writen( fd, body, strlen(body) );
}


void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE], key[MAXLINE], value[MAXLINE];
    char *p;
    
    rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")){   /* until encounter the empty line */
        p = strchr(buf, ':');
        if(p){
            *p = '\0';
            sscanf(buf, "%s", key);
            sscanf(p+1, "%s", value);
            printf("%s : %s\n", key, value);
        }
        rio_readlineb(rp, buf, MAXLINE );    /* next line */
    }
    return ;
}

int parse_url( char *url, char *filename, char *cgiargs)
{
    char *ptr;
    if( !strstr(url, "cgi-bin") ){    /* static content ? */
        strcpy( cgiargs, "");   /* no cgi arguments */
        strcpy( filename, ".");  /* ., current directory */
        strcat( filename, url);  /* */
        if( url[ strlen(url) -1 ] == '/')  /* append default page */
            strcat( filename, "home.html");
        return 1;   /* is static page */
    }else{    /* dynamic page */
        ptr = index( url, '?');  /* find the position of '?' */
        if( ptr ){   /* has arguments ? */
             strcpy( cgiargs, ptr+1 ) ;
            *ptr = '\0';
    }else{  /* no arguments */
        strcpy(cgiargs, "");
    }
    strcpy( filename, ".");
    strcat( filename, url );
    return 0;   /* no static page */
    }
}
void serve_static( int fd, char *filename, int filesize)
{
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];

    /* send response headers to client */
    get_filetype( filename, filetype );
    sprintf( buf, "HTTP/1.0 200 OK \r\n" );
    sprintf( buf, "%sServer: a simple web server\r\n", buf );
    sprintf( buf, "%sContent-length: %d\r\n", buf, filesize );
    sprintf( buf, "%sContent-Type: %s\r\n\r\n", buf, filetype );
    rio_writen( fd, buf, strlen(buf) );

    /* send response body to client */
    srcfd = open( filename, O_RDONLY, 0);
    /* map the file srcfd to virtual memory */
    srcp = mmap( 0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0 );  close( srcfd );    /* close the fd */
    rio_writen( fd, srcp, filesize );    /* send the file to client */
    munmap( srcp, filesize);    /* free the virtual memory */
}

/* get the request file's type */
void get_filetype(char *filename, char *filetype)
{
    if( strstr(filename, ".html") )     /* html */
        strcpy( filetype, "text/html" );
    else if(strstr(filename, ".xml"))
        strcpy(filetype, "application/xml");
    else if(strstr(filename, ".pgn"))
        strcpy(filetype, "image/pgn");
    else if( strstr( filename, ".jpg") )   /* jpg */
        strcpy( filetype, "image/jpeg");
    else if( strstr(filename, ".gif") )    /* gif */
        strcpy( filetype, "image/gif");
    else if(strstr(filename, ".ico"))
        strcpy(filetype, "image/x-icon");
    else if(strstr(filename, ".pdf"))
        strcpy(filetype, "application/pdf");
    else  /* others */
        strcpy( filetype, "text/plain");
}
void serve_dynamic( int fd, char *filename, char *cgiargs)
{
    char buf[MAXLINE], *emptylist [] = {NULL};
    extern char **environ;

    /* return first part of HTTP response */
    sprintf( buf, "HTTP/1.0 200 OK\r\n");    /* status line */
    rio_writen( fd, buf, strlen(buf) );
    sprintf( buf, "Server: this is a simple server \r\n");  /* response headers */
    rio_writen( fd, buf, strlen(buf));

    if( fork() == 0 ){   /* child process */
        setenv( "QUERY_STRING", cgiargs, 1);
        dup2( fd, STDOUT_FILENO);   /* redirect stdout to client */
        execve( filename, emptylist, environ);   /* run CGI program */
    }
    wait(NULL);    /* wait the child process to end */
}

