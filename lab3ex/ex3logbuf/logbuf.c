#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 6
#define MAX_LOOPS 30

char logbuf[MAX_BUFFER_SLOT][MAX_LOG_LENGTH];

int count;
void flushlog();

pthread_mutex_t lock;
pthread_cond_t cond_full_buffer; // Variable to check whether the buffer is full or not

struct _args
{
   unsigned int interval;
};

void *wrlog(void *data)
{
   char str[MAX_LOG_LENGTH];
   int id = *(int*) data;

   usleep(20);

   pthread_mutex_lock(&lock); // Lock the mutex

    // Check whether the buffer is full or not
   while (count == MAX_BUFFER_SLOT) {
    // If the buffer is full, wait for the signal from the synchronous variabla
    pthread_cond_wait(&cond_buffer_full, &lock);
   }

   sprintf(str, "%d", id);
   strcpy(logbuf[count], str);
   count = (count > MAX_BUFFER_SLOT)? count :(count + 1); /* Only increase count to size MAX_BUFFER_SLOT*/
   // printf("wrlog(): %d \n", id);

   pthread_mutex_unlock(&lock); // Unlock the mutex
   return 0;
}

void flushlog()
{
   int i;
   char nullval[MAX_LOG_LENGTH];

   // printf("flushlog()\n");
   pthread_mutex_lock(&lock); // Lock the mutex
   sprintf(nullval, "%d", -1);
   for (i = 0; i < count; i++)
   {
      printf("Slot  %i: %s\n", i, logbuf[i]);
      strcpy(logbuf[i], nullval);
   }

   fflush(stdout);

   /*Reset buffer */
   count = 0;

   // Send the signal to the synchronous variable to announce that the buffer is empty
   pthread_cond_broadcast(&cond_buffer_full);

   pthread_mutex_unlock(&lock); // Unlock the mutex

   return;

}

void *timer_start(void *args)
{
   while (1)
   {
      flushlog();
      /*Waiting until the next timeout */
      usleep(((struct _args *) args)->interval);
   }
}

int main()
{
   int i;
   count = 0;
   pthread_t tid[MAX_LOOPS];
   pthread_t lgrid;
   int id[MAX_LOOPS];

   pthread_mutex_init(&lock, NULL);
   pthread_cond_init(&cond_buffer_full, NULL);

   struct _args args;
   args.interval = 200000;
   /*500 msec ~ 500 * 1000 usec */

   /*Setup periodically invoke flushlog() */
   pthread_create(&lgrid, NULL, &timer_start, (void*) &args);

   /*Asynchronous invoke task writelog */
   for (i = 0; i < MAX_LOOPS; i++)
   {
      id[i] = i;
      pthread_create(&tid[i], NULL, wrlog, (void*) &id[i]);
   }

   for (i = 0; i < MAX_LOOPS; i++)
      pthread_join(tid[i], NULL);

   pthread_cancel(lgrid);
   pthread_mutex_destroy(&lock);
   pthread_cond_destroy(&cond_buffer_full);

   return 0;
}
