#ifndef _SYN_H_
#define _SYN_H_

#include "ws.h"

#ifndef _STRUCT_SBUF_T_
#define _STRUCT_SBUF_T_

/* safe buffer : cycle queue with synchronization */
typedef struct{
    int *buf;           /* buffer array */
    int n;               /* maximum number of slots */
    int front;            /* the index of first item */
    int rear;             /* the index of last item */
    sem_t *mutex;        /* protects access to buf exclusively */
    sem_t *slots;          /* counts available slots */
    sem_t *items;         /* counts available items */
}sbuf_t;

#endif

void P(sem_t *s);   /* wait operation with semaphore s */
void V(sem_t *s);   /* signal operation with semaphore s */

void sbuf_init(sbuf_t *sp, int n);    /* init buffer */
void sbuf_insert(sbuf_t *sp, int item);    /* insert item into buffer */
int  sbuf_remove(sbuf_t *sp);    /* remove an item from buffer */


#endif
