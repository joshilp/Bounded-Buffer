#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "bbuff.h"
#include "stats.h"


// The factory creates candy and the kids consume it. The candy will be stored in a bounded buffer. 
// factory_number tracks which factory thread produced the candy item.
// time_stamp_in_ms tracks when the item was created.
typedef struct 
{
	int factory_number; //tracks which factory thread produced the candy item
	double time_stamp_in_ms; //s tracks when the item was created
} candy_t;

// gets the current time in ms
double current_time_in_ms(void)
{
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

// thread continually checks this _Bool variable (often called a flag) to see if it should end
_Bool stop_thread = false;

// creates a candy factory thread (producer)
void* candyfactory_thread(void* arg)
{
	// initialize variables
	int *val_pointer = (int*) arg;
	int factory = *val_pointer;
	

	// printf("creating factory number: %d\n", factory);
	int sleep_time = 0;

	// 1. Loop until main() signals to exit (see below)
	while (!stop_thread)
	{
		// 1. Pick a number of seconds which it will (later) wait. 
		//	  Number randomly selected between 0 and 3 inclusive.
		sleep_time = (rand() % 4);

		// 2. Print a message such as: “\tFactory 0 ships candy & waits 2s”
		printf("\tFactory %d ships candy & waits %ds\n", factory, sleep_time);

		// 3. Dynamically allocate a new candy item and populate its fields.
		candy_t *item = malloc(sizeof(candy_t));
		item->factory_number = factory;
		item->time_stamp_in_ms = current_time_in_ms();

		// 4. Add the candy item to the bounded buffer and record stat.
		bbuff_blocking_insert(item);
		stats_record_produced(factory);

		// 5. Free item, then Sleep/Wait for number of seconds identified in #1.
		// free(item);
		// item = NULL;
		sleep(sleep_time);
	}

	// 2. When the thread finishes, print “Candy-factory 0 done”
	printf("Candy-factory %d done\n",factory);

	//end thread
	pthread_exit(NULL);


}

// creates a kid thread (consumer)
void* kid_thread(void* arg)
{
	// initialize variables
	int factory = 0;
	double time_delay = 0;
	

	//Loop forever
	while (true)
	{
		//1. Extract a candy item from the bounded buffer.
		//This will block until there is a candy item to extract.
		candy_t *item = (candy_t*)bbuff_blocking_extract();

		//2. Process the item. Initially you may just want to printf() it to the screen; 
		// in the next section, you must add a statistics module that will track what candies have been eaten.
		factory = item->factory_number;
		time_delay = (current_time_in_ms() - item->time_stamp_in_ms);
		//printf("factory %d took time of %fms\n", factory, time_delay );
		stats_record_consumed(factory,time_delay);
		
		//3. Free item and Sleep for either 0 or 1 seconds (randomly selected).
		free(item);
		item = NULL;
		sleep((rand() % 2));
	}
	
	//end thread
	pthread_exit(NULL);
}


int main(int argc, char *argv[])
{

	// 1. Extract arguments
	if (argc != 4)
	{
		printf("Please enter 3 arguments greater than 0. Number of Factories, Number of Kids, and Number of Seconds.\n");
		exit(1);
	}

	int factories = atoi(argv[1]);
	// printf("factories entered: %d\n", factories);
	int kids = atoi(argv[2]);
	// printf("kids entered: %d\n", kids);
	int seconds = atoi(argv[3]);
	// printf("seconds entered: %d\n", seconds);

	if (factories <=0 || kids <=0 || seconds <=0)
	{
		printf("Please enter numbers greater than 0.\n");
		exit(1);
	}

	// 2. Initialize modules
	bbuff_init();
	stats_init(factories);

	// 3. Launch candy-factory threads
	//  Store the thread IDs in an array to join on them later
	printf("Starting candy factories...\n");
	//pthread_t factory_tid[factories];
	pthread_t* factory_tid = malloc(sizeof(pthread_t) * factories);
	pthread_attr_t attr1;
	pthread_attr_init(&attr1);
	int factorylist[factories];


	for (int i = 0; i < factories; i++)
	{
		factorylist[i] = i;
		pthread_create(&factory_tid[i], &attr1, candyfactory_thread, &factorylist[i]);
	}

	// 4. Launch kid threads
	// pthread_t kid_tid[kids];
	pthread_t* kid_tid = malloc(sizeof(pthread_t) * kids);
	pthread_attr_t attr2;
	pthread_attr_init(&attr2);

	for (int j = 0; j < kids; j++)
	{
		pthread_create(&kid_tid[j], &attr2, kid_thread, &j);
	}

	// 5. Wait for requested time
	for (int t = 0; t < seconds; t++) 
	{
		sleep(1);
		printf("Time %ds\n", t);
	}

	// 6. Stop candy-factory threads

	stop_thread = true;
	printf("\nStopping candy factories...\n");
	for (int i = 0; i < factories; i++)
	{
		pthread_join(factory_tid[i], NULL);
	}

	// 7. Wait until no more candy
	while (!bbuff_is_empty()) 
	{
		printf("Waiting for all candy to be consumed.\n");
		sleep(1);
	}

	// 8. Stop kid threads
	printf("\nStopping kids.\n");
	for(int i = 0; i < kids; i++)
	{
		pthread_cancel(kid_tid[i]);
		pthread_join(kid_tid[i], NULL);
	}

	// 9. Print statistics
	stats_display();

	// 10. Cleanup any allocated memory
	stats_cleanup();
	free(factory_tid);
	free(kid_tid);
	factory_tid = NULL;
	kid_tid = NULL;
}