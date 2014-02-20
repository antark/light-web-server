#include "syn.h"

void P(sem_t *s)
{
	sem_wait(s);
}

void V(sem_t *s)
{
	sem_post(s);
}


void sbuf_init( sbuf_t *sp, int n )
{
    sp->buf = calloc( n, sizeof(int) );          /* alloc memory */
    sp->n = n;                            /* buffer holds max of n items */
    sp->front = sp->rear = 0;                 /* empty buffer iff front == rear */
    sem_init( &sp->mutex, 0, 1 );             /* mutex */
    sem_init( &sp->slots, 0, n );               /* initially,buf has n empty slots */
    sem_init( &sp->items, 0, 0 );              /* initially,buf has 0 data items */
}
void sbuf_insert(sbuf_t *sp, int item)
{
    P( &sp->slots );                    /* wait for available slots */
    P( &sp->mutex );                   /* lock the buffer */
    sp->buf[ (++sp->rear)%(sp->n) ] = item; /* insert the item */
    V( &sp->mutex );                   /* unlock the buffer */
    V( &sp->items );                    /* announce available item */
}
int sbuf_remove(sbuf_t *sp)
{
    int item;
    P( &sp->items );                    /* wait for available items */
    P( &sp->mutex );                   /* lock the buffer */
    item = sp->buf[ (++sp->front)%(sp->n) ]; /* remove the item */
    V( &sp->mutex );                   /* unlock the buffer */
    V( &sp->slots );                    /* announce available slot */
    return item;
}
