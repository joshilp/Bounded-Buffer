#ifndef BBUFF_H
#define BBUFF_H
#define BUFFER_SIZE 10

// initializes variables and semaphores
void bbuff_init(void);

//adds item to buffer while blocking
void bbuff_blocking_insert(void* item);

//extracts item from buffer while blocking
void* bbuff_blocking_extract(void);

//checks if buffer is empty
_Bool bbuff_is_empty(void);

#endif