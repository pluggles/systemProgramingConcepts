#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define BLOCK_SIZE sizeof(block)

void *global_base = NULL;  //head for the linked list

void *malloc(size_t size)

{
  
    block *newblock;


    if (size <= 0)
    {
        return NULL;
    }

    if (!global_base)  //initalize on the first call
    {
        newblock = request_space(NULL, size);
        if (!newblock)
        {
            return NULL; //something failed
        }
        global_base = newblock;
    }
    else //find free block if space has been initialised
    {
         block *last = global_base;
        newblock = find_free_block(&last, size);
        if (!newblock) //if no more space, get more
        {
            newblock = request_space(last, size);
            if (!newblock)
            {
                return NULL;
            }
        }
        else //set the struct
        {
            newblock->free = 0;
            newblock->magic = 0x77777777;
        }

    }
    //  printf("malloc %d bytes\n", size);
    return (newblock + 1); //returns the pointer after our block

}

void *calloc(size_t count, size_t size) //just clears mem before calling malloc
{
    size_t newsize = count * size;
    void *ptr = malloc(newsize);
    memset(ptr, 0, newsize);
    return ptr;
}

void *realloc(void *ptr, size_t size)
{
    if (!ptr) //realloc will just malloc if passed a null
    {
        return malloc(size);
    }
     block *block_ptr = get_block_ptr(ptr);
    if (block_ptr->size >= size) //have more then enough mem, may wanna do some spliting later
    {
        return ptr;
    }
    //mallocs more size if need
    void *new_ptr;
    new_ptr = malloc(size);
    if (!new_ptr)
    {
        return NULL;
    }
    memcpy(new_ptr, ptr, block_ptr->size);
    free(ptr);
    return new_ptr;
}

void free(void *ptr)
{
    if (!ptr)
    {
        return;
    }
     block *block_ptr = get_block_ptr(ptr);
    //assert(block_ptr->free == 0);
    //assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
    block_ptr->free = 1;
    block_ptr->magic = 0x5555555;

}
/*
* since using a linked list it is easy enough to iterate through the list
* to find the next free block
*/
 block *find_free_block( block **last, size_t size)
{
    block *current = global_base;
    while (current && !(current->free && current->size >= size))
    {
        *last = current;
        current = current->next;
    }
    return current;
}

/*
* If free space is not found use sbrk to request more space
* only calls sbrk when needed
*/

block *request_space(block *last, size_t size)
{
    block *newblock;
    newblock = sbrk(0);
    void *request = sbrk(size + BLOCK_SIZE); //adds space for size plus our struct
    assert((void *)newblock == request);
    if (request == (void *) - 1)
    {
        return NULL; //this would mean sbrk failed
    }
    if (last)
    {
        last->next = newblock;
    }
    newblock->size = size;
    newblock->next = NULL;
    newblock->free = 0;
    newblock->magic = 0x12345678;
    return newblock;
}

block *get_block_ptr(void *ptr) //getd address of struct
{
    return (block *)ptr - 1;
}
