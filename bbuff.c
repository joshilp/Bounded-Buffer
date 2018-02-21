#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "bbuff.h"
#include <semaphore.h>

// declare variables
int num_items;
void* buffer[BUFFER_SIZE];

// declare semaphores
sem_t mutex;
sem_t full;
sem_t empty;

// initializes variables and semaphores
void bbuff_init(void)
{

	sem_init(&mutex, 0, 1);
	sem_init(&full, 0, BUFFER_SIZE);
	sem_init(&empty, 0, 0);
	num_items = 0;
	//printf("buffer_init\n");
}

//adds item to buffer while blocking
void bbuff_blocking_insert(void* item)
{
	//waits
	sem_wait(&full);
	sem_wait(&mutex);

	//add item to buffer
	buffer[num_items] = item;
	num_items++;

	//signals
	sem_post(&mutex);
	sem_post(&empty);
	//printf("bbuff_blocking_insert\n");
}

//extracts item from buffer while blocking
void* bbuff_blocking_extract(void)
{
	//waits
	sem_wait(&empty);
	sem_wait(&mutex);

	//removes item from buffer
	void* item = buffer[num_items-1];
	num_items--;

	//signals
	sem_post(&mutex);
	sem_post(&full);

	//printf("bbuff_blocking_extract\n");
	return item;
}

//checks if buffer is empty
_Bool bbuff_is_empty(void)
{
	if (num_items == 0)
		return true;
	else
		return false;
}
