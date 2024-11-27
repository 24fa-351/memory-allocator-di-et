

# MALLOC

* allocates memory
* we have a bunch of memory WE know about but no one else does
* We store it (for some reason) on a HEAP
* reserving space for who called us

# FREE

* deallocating memory
* user gives us memory back
* add that into what we know about (and user is supposed to forget about now)

## HEAP

* a bunch of chunks of memory
`struct chunk_on_heap { int size; char* pointer_to_start };`


* add a chunk back (on free)
`create the chunk_on_heap; heap_insert()`

* get a chunk out (it's not ours any more, it's the users) on malloc
`
try to find this size or bigger;
if found a chunk: cut it into two (actual size, and leftover)
    return the actual size
    heap_insert(leftover)
if no find:
    get more memory from system (sbrk)
    split that, add the rest to heap
`
* MINheap based on size of chunk -- sorted from smallest to largest
* small chunks are more frequently requested

* what do we do when the heap is empty?
  sbrk()
* what do we do when the user requests a larger thing than is in the heap?
  sbrk()

## sbrk()

* Ask linux for a HUGE chunk of memory


##
#define LIST_SIZE 1000
#define MIN_BLOCK_SIZE 4096

typedef struct block
{
    size_t size;
    char *address;
    int is_free;
} block_t;

block_t blocks[LIST_SIZE];

int blocks_in_use = 0;

##

