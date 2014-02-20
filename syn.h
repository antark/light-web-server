#ifndef _SYN_H_
#define _SYN_H_

#include "ws.h"

#ifndef _STRUCT_SBUF_T_
#define _STRUCT_SBUF_T_

typedef struct{
    int *buf;            /* buffer array */
    int n;               /* maximum number of slots */
    int front;            /* the index of first item */
    int rear;             /* the index of last item */
    sem_t mutex;        /* protects accesses to buf */
    sem_t slots;          /* counts available slots */
    sem_t items;         /* counts available itmes */
} sbuf_t;

#endif

void P(sem_t *s);
void V(sem_t *s);

void sbuf_init( sbuf_t *sp, int n );
void sbuf_insert(sbuf_t *sp, int item);
int  sbuf_remove(sbuf_t *sp);


#endif
