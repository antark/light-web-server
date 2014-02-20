#include "rio.h"

ssize_t rio_writen(int fd, void *buf, size_t n)
{
    size_t nleft = n;     /* number bytes left */
    ssize_t nwritten;     /* number of written */
    char *bufp = buf;    /* buffer */

    while(nleft > 0){
        if( (nwritten = write(fd ,bufp, nleft) ) < 0 ){    /* error */
            if( errno == EINTR )    /* interrupted by sig handler return */
                nwritten = 0;       /* read again */
            else
                return -1;           /* errorno set by write() */
    }
    nleft -= nwritten;
    bufp += nwritten;
    }
    return n;
}
void rio_readinitb( rio_t *rp, int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}
ssize_t rio_read( rio_t *rp, char *buf, size_t n )
{
   int cnt;
   while( rp->rio_cnt <=0 ){    /* refill if buf is empty */
       rp->rio_cnt = read( rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf) );
       if( rp->rio_cnt < 0 ){
           if( errno != EINTR )   /* not interrupted by sig handler return */
               return -1;
       }else if( rp->rio_cnt == 0)    /* EOF */
           return 0;
    else 
        rp->rio_bufptr =  rp->rio_buf;    /* reset buffer ptr */
}
/* copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
    cnt = n;
    if( rp->rio_cnt < n )
        cnt = rp->rio_cnt;
    memcpy( buf, rp->rio_bufptr, cnt );
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}
ssize_t rio_readlineb( rio_t *rp, void *buf, size_t maxlen)
{
    int n, rc;
    char c, *bufp = buf;
    for( n = 1; n < maxlen; n++ ){    /* max number of bytes: maxlen - 1 */
        if( (rc = rio_read(rp, &c, 1) ) == 1 ){     /* read one byte */
        *bufp ++ = c;
        if( c == '\n' )        /* is LF ? */
            break;
        }else if( rc == 0){    /* encounter EOF */
            if( n == 1 )
                return 0;    /* EOF, no data read */
            else
                break;    /* EOF, some data had been read */
        }else
            return -1;     /* error */
    }
    *bufp = 0;
    return 0;
}
ssize_t rio_readnb( rio_t *rp, void *buf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = buf;
    while( nleft > 0){
        if( (nread = rio_read( rp, bufp, nleft ) ) < 0){
            if( errno == EINTR )    /* interrupted by sig handler return */
                nread = 0;      /* call read() again */
            else     
                return -1;     /* errno set by read() */
        }else if( nread == 0)
                  break;            /* EOF */
        nleft -= nread;
        bufp += nread;
    }
    return n - nleft;       /* return >= 0 */
}
