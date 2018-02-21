#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "stats.h"
#include <semaphore.h>

sem_t mutex;

int producers = 0;

typedef struct factorystats
{
	// keep track of factory number
	int factory_number;

	// 1. Count the number of candies each factory creates. Called from the candy-factory thread.
	int candies_produced;

	// 2. Count the number of candies that were consumed from each factory.
	int candies_consumed;

	// 3. For each factory, the min, max, and average delays for how long it took from the moment the
	// candy was produced (dynamically allocated) until consumed (eaten by the kid). This will be
	// done by the factory thread calling the stats code when a candy is created, and the kid thread
	// calling the stats code when an item is consumed.
	double min_delay;
	double max_delay;
	double average_delay;
	double total_delay;
	
} factorystats_t;

//initialize factories
factorystats_t* factories;

//initializes factorystats variables
void stats_init(int num_producers)
{
	sem_init(&mutex, 0, 1);

	// set variable for producers
	producers = num_producers;
	// array to track each strat struct
	factories = malloc(sizeof(factorystats_t) * num_producers);

	for (int i = 0; i < num_producers; i++)
	{
		factories[i].factory_number = i;
		factories[i].candies_produced = 0;
		factories[i].candies_consumed = 0;
		factories[i].min_delay = -1;
		factories[i].max_delay = -1;
		factories[i].average_delay = -1;
		factories[i].total_delay = 0;
	}
}

//free factories
void stats_cleanup(void)
{
	free(factories);
	factories = NULL;
}

//records candy production stats
void stats_record_produced(int factory_number)
{
	//waits
	sem_wait(&mutex);

	//increase number produced
	factories[factory_number].candies_produced++;

	//signals
	sem_post(&mutex);

}

//record candy consumption stats
void stats_record_consumed(int factory_number, double delay_in_ms)
{
	//waits
	//locks so can change data without other process affecting stats
	sem_wait(&mutex);

	//increments candies consumed
	factories[factory_number].candies_consumed++;
	
	//sets variables to consumed, min delay, max delay, and total delay to be used
	//for calculations
	// int facnum = factories[i].factory_number;
	// int c_prod = factories[i].candies_produced;		
	double c_cons = (double)factories[factory_number].candies_consumed;
	double mind = factories[factory_number].min_delay;
	double maxd = factories[factory_number].max_delay;
	double totald = factories[factory_number].total_delay;

	//check if still at default value
	if (mind == -1)
	{
		factories[factory_number].min_delay = delay_in_ms;
		factories[factory_number].max_delay = delay_in_ms;
		factories[factory_number].average_delay = delay_in_ms;
		factories[factory_number].total_delay = delay_in_ms;
	}

	//sets new min and max values
	else
	{
		// new min
		if (delay_in_ms < mind)
		{
			factories[factory_number].min_delay = delay_in_ms;
		}

		//new max
		if (delay_in_ms > maxd)
		{
			factories[factory_number].max_delay = delay_in_ms;
		}

		//calculate average using total delay divided by amount consumed
		factories[factory_number].total_delay += delay_in_ms;
		factories[factory_number].average_delay = (totald/c_cons);
	}

	//signals
	sem_post(&mutex);
}

//prints out statistics
void stats_display(void)
{
	printf("\nStatistics:\n\n");
	printf("%s%8s%8s%16s%16s%16s\n", "Factory#", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]");
	
	//displays all stats in struct
	for (int i = 0; i < producers; i++)
	{
		int facnum = factories[i].factory_number;
		int c_prod = factories[i].candies_produced;		
		int c_cons = factories[i].candies_consumed;
		double mind = factories[i].min_delay;
		double avgd = factories[i].average_delay;
		double maxd = factories[i].max_delay;

		printf("%4d%10d%8d%16f%16f%16f\n", facnum, c_prod, c_cons, mind, avgd, maxd); 

		if (c_prod != c_cons)
		{
			printf("ERROR: Mismatch between number made and eaten.\n");
		}
	}
}