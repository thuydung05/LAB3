#ifndef PTHREAD_H
#include <pthread.h>
#endif

typedef struct pthread_seqlock { /* TODO: implement the structure */
   int seq;
} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t *rw)
{
   rw->seq = 0;
}

static inline void pthread_seqlock_wrlock(pthread_seqlock_t *rw)
{
   while (rw->sequence != 0);

   rw->seq++;
}

static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw)
{
   if (rw->seq != 0)
   {
      rw->seq--;
   }
}

static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *rw)
{
   while (rw->seq);
   return 1;
}

static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *rw)
{
   if (rw->seq != 0) pthread_seqlock_rdlock(rw);
   return 1;
}

