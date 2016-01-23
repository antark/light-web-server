#include "syn.h"
#include <stdio.h>

/*
    sem_init : not implemented in mac os x, please use
    sem_open : name cannot be diplicated
*/
int x_sem_init(sem_t **s, int f, int n)
{
#ifdef __APPLE__
    static int count = 0;
    ++count;
    static char name[100];
    sprintf(name, "/semaphore-%d", count);
    *s = sem_open(name, O_CREAT, 0644, n);
    if(*s == SEM_FAILED){
        return -1;
    }
    return 0;
#elif __linux__
     *s = malloc(sizeof(sem_t));
     if(*s == NULL){
        return -1;
     }else{
        return sem_init(*s, 0, n);
     }
#endif
}


/* wait operation with semaphore s */
void P(sem_t *s)
{
	sem_wait(s);
}

/* signal operation with semaphore s */
void V(sem_t *s)
{
	sem_post(s);
}

/* init buffer */
void sbuf_init(sbuf_t *sp, int n )
{
    sp->buf = malloc(n*sizeof(int) );       /* alloc buffer memory */
    sp->n = n;      /* buffer holds max of n items */
    sp->front = sp->rear = 0;       /* empty buffer iff front == rear */
    x_sem_init(&sp->mutex, 0, 1);        /* mutex */
    x_sem_init(&sp->slots, 0, n);     /* initially,buf has n empty slots */
    x_sem_init(&sp->items, 0, 0);      /* initially,buf has 0 data items */
}

/* insert item into buffer */
void sbuf_insert(sbuf_t *sp, int item)
{
    P(sp->slots);                    /* wait for available slots */
    P(sp->mutex);                   /* lock the buffer */
    sp->buf[(++sp->rear) % (sp->n)] = item;  /* insert the item */
    V(sp->mutex);                   /* unlock the buffer */
    V(sp->items);                    /* announce available item */
}

/* remove an item from buffer */
int sbuf_remove(sbuf_t *sp)
{
    int item;
    P(sp->items);                    /* wait for available items */
    P(sp->mutex);                   /* lock the buffer */
    item = sp->buf[(++sp->front) % (sp->n) ]; /* remove the item */
    V(sp->mutex);                   /* unlock the buffer */
    V(sp->slots);                    /* announce available slot */
    return item;
}


int sbuf_destroy(sbuf_t *sp)
{
    return 0;
}
