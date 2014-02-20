#include "sock.h"

int open_clientfd(char *hostname, int port)
{
    int clientfd;                 /* client file(socket) descriptor */
    struct hostent *hp;           /* DNS host entry */
    struct sockaddr_in serveraddr;  /* address structure of server */

    if( (clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;   /* check errno for cause of error */

    /* fill in the server's IP address and port */
    if((hp = gethostbyname(hostname)) == NULL)
        return -2;
    bzero((char*)&serveraddr, sizeof(serveraddr));   /* fill with 0 */
    serveraddr.sin_family = AF_INET;   /* protocol family */
    bcopy((char*)hp->h_addr, (char*)&serveraddr.sin_addr.s_addr, sizeof(hp->h_length));   /* IP addresss */
    serveraddr.sin_port = htons(port);    /* port */

    /* establish a connection with the server */
    if(connect(clientfd, (SA*)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;
    return clientfd;
}

int open_listenfd(int port)
{
    int listenfd, optval = 1;   /* listen file(socket) descriptor */
    struct sockaddr_in serveraddr;   /* address of server */
    /* create a socket descriptor */
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;   /* error */
    /* eliminates "address already in use" error from bind */
    if( setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
                (const void*)&optval, sizeof(int)) < 0)
        return -1;
    /* listenfd will be an endpoint for all request to port
    on any IP address for this host */
    bzero(&serveraddr, sizeof(serveraddr));    /* fill with 0 */
    serveraddr.sin_family = AF_INET;      /* protocol family */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* IP addresss */
    serveraddr.sin_port = htons((unsigned short)port);   /* port */
    if(bind(listenfd, (SA*)&serveraddr, sizeof(serveraddr)) < 0)    /* bind listenfd with the sever's address */
        return -1;
    /* make it a listening socket ready to accept connection requests */
    if(listen(listenfd, LISTENQ) < 0)
        return -1;
    return listenfd;
}
