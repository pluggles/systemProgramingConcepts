// Included for the `size_t` type. 
#include <string.h>
#include <unistd.h>

//make a stuct for a linked list

typedef struct block {
	size_t size;
	struct block *next;
	int free;
	int magic; //for debugging
} block;

void* malloc(size_t);
void* calloc(size_t, size_t);
void* realloc(void*, size_t);
void free(void*);
struct block *request_space(struct block*, size_t);
struct block *find_free_block(struct block**, size_t);
struct block *get_block_ptr(void*);
