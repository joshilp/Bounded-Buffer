#ifndef STATS_H
#define STATS_H

//initializes factorystats variables
void stats_init(int num_producers);

//free factories
void stats_cleanup(void);

//records candy production stats
void stats_record_produced(int factory_number);

//record candy consumption stats
void stats_record_consumed(int factory_number, double delay_in_ms);

//prints out statistics
void stats_display(void);

#endif