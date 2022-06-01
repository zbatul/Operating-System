/**
 * Name: Batul Zamin
 * NetID: nd9354
 * Class: CS 421 Operating Systems
 * Program 4: Write a program which implements 
 * 	      a multiple producer - multiple consumer environment.
 */

/* Declare headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>	/* for pthread functions */
#include <semaphore.h>	/* for semaphore */
#define BUFFER_SIZE 5 	/* size of shared buffer */

typedef int Buffer_item;
int in = 0;
int out = 0;

/* 2. Initialize buffer */
Buffer_item buffer[BUFFER_SIZE];

/* Declare mutex lock and semaphores full and empty */
pthread_mutex_t mutex;
sem_t full;
sem_t empty;

/** Define function prototypes **/
// the items are inserted and removed in these functions
int insert_item(Buffer_item item);
int remove_item(Buffer_item* item);
// the thread runs in these functions
void *producer(void *param);
void *consumer(void *param);

/** Main Function **/
int main(int argc, char *argv[]) {
	printf("Main program:\n");
   	/* 1. Get command line arguments 
	1. The length of time the program should run.
	2. The number of producer threads.
	3. The number of consumer threads.*/
	int program_length, num_producer, num_consumer;
	program_length = atoi(argv[1]);
	num_producer = atoi(argv[2]);
	num_consumer = atoi(argv[3]);

	printf("Main thread beginning\n");
	pthread_mutex_init(&mutex, NULL);
  	sem_init(&full, 0, 0);
  	sem_init(&empty, 0, BUFFER_SIZE);

  	/* 3. Create producer threads. */
	pthread_t tid_producer[num_producer]; 	/* the thread identifier */
	pthread_attr_t attr_producer; 	/* set of attributes for the thread */
	pthread_attr_init(&attr_producer);	/* get the default attributes */
	
	/* create the threads */
	int i;
	for (i = 0; i < num_producer; i++) { 
	    int* prod = (int *) malloc(sizeof(int));
	    *prod = i;
	    pthread_create(&tid_producer[i], &attr_producer, producer, (void*) prod); 
	    printf("Creating producer thread with id %lu\n", tid_producer[i]);
    }

   	/* 4. Create consumer threads.  */
	pthread_t tid_consumer[num_consumer]; 	/* the thread identifier */
	pthread_attr_t attr_consumer; 	/* set of attributes for the thread */
	pthread_attr_init(&attr_consumer);	/* get the default attributes */

	/* create the threads */
	int j;
	for (j = 0; j < num_consumer; j++) { 
		int* consm = (int *) malloc(sizeof(int));
		*consm = j;
	    pthread_create(&tid_consumer[j], &attr_consumer, consumer, (void*) consm); 
	    printf("Creating consumer thread with id %lu\n", tid_consumer[j]);
    }

   	/* 5.  Sleep. */
	printf("Main thread sleeping for %d seconds\n\n", program_length);
	sleep(program_length);

   	/* 6.  Main thread exits.  */
	printf("\nMain thread exiting\n");
	exit(0);
	return 0;
}

/** Producer Code to produce items and put in shared buffer **/
void *producer(void *tid_producer) {
   Buffer_item item;
   while(1) {
      /* generate random number */
      int r = (rand() % 4) + 1;
      /* sleep for random amount of time */
      int tid = *(int*) tid_producer;
      printf("Producer thread %d sleeping for %d seconds\n", tid, r);
      sleep(r);
      /* generate a random number - this is the producer's product */
      item = (rand() % 50) + 1;
      /* insert the product in the shared buffer */
      if (insert_item(item) < 0) {
         printf("Producer error\n\n");
      }
      else {
	 	 printf("Producer thread %d inserted value %d\n\n", tid, item);
      }
   }
}

/** Consumer Code to consume items and remove from shared buffer **/
void *consumer(void *tid_consumer){
   Buffer_item item;
   while(1) {
      /* generate random number */
      int r = (rand() % 4) + 1;
      /* sleep for random amount of time */
      int tid = *(int*) tid_consumer;
      printf("Consumer thread %d sleeping for %d seconds\n", tid, r);
      sleep(r);
      /* remove the product from the shared buffer */
      if (remove_item(&item) < 0) {
	     printf("Consumer error\n\n"); 
      }
      else {
	 	 printf("Consumer thread %d removed value %d\n\n", tid, item);
      }
   }
}

/** Helper for producer to insert item into buffer **/
int insert_item(Buffer_item next_produced) {
	/* insert item into buffer
	   return 0 if successful, otherwise
	   return -1 indicating an error condition */

	sem_wait(&empty);
	pthread_mutex_lock(&mutex);
	/* add next produced to buffer */
	if (in < BUFFER_SIZE) {
		buffer[in] = next_produced;
		in = (in + 1) % BUFFER_SIZE; 
		printf("\nInsert_item inserted item %d at position %d\n", next_produced, in);
		int i;
		for(i = 0; i < BUFFER_SIZE; i++){
		   if(buffer[i] != 0) {
		      printf("[%d]", buffer[i]);
		   }	
		   else {
		      printf("[empty]");
		   }
		   fflush(stdout);
		}
		printf("  in = %d, out = %d\n", in, out);
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
		return 0;
	}
	else return -1;
}

/** Helper for consumer to remove item from buffer **/
int remove_item(Buffer_item* next_consumed) {
	/* remove an object from buffer placing it in item
	   return 0 if successful, otherwise
	   return -1 indicating an error condition */

	sem_wait(&full);
	pthread_mutex_lock(&mutex);
	/* remove an item from buffer to next consumed */
	if (out < BUFFER_SIZE){
		*next_consumed = buffer[out];
		buffer[out] = 0;
		out = (out + 1) % BUFFER_SIZE;
		printf("\nRemove_item removed item %d at position %d\n", *next_consumed, out);
		int i;
		for(i = 0; i < BUFFER_SIZE; i++) {
		   if(buffer[i] != 0) {
			  printf("[%d]", buffer[i]);
		   }	
		   else {
			  printf("[empty]");
		   }
		   fflush(stdout);
		}
		printf("  in = %d, out = %d\n", in, out);
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
		/* consume the item in next consumed */
		return 0;
	}
	else return -1;
}


