#ifndef _RIO_H_
#define _RIO_H_

#include "ws.h"

#ifndef _STRUCT_RIO_T_
#define _STRUCT_RIO_T_

typedef struct{
    int rio_fd;                      /* descriptor for this internal buf */
    int rio_cnt;                     /* unread bytes in internal buf */
    char *rio_bufptr;                  /* next unread byte in internal  buf  */
    char rio_buf[ RIO_BUFSIZE ];      /* internal buffer */
}rio_t;

#endif

void rio_readinitb( rio_t *rp, int fd);
ssize_t rio_readlineb( rio_t *rp, void *buf, size_t maxlen);
ssize_t rio_read( rio_t *rp, char *buf, size_t n );
ssize_t rio_readnb( rio_t *rp, void *buf, size_t n);
ssize_t rio_writen(int fd, void *buf, size_t n);

#endif
