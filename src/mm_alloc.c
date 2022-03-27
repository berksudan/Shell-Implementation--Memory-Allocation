/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines.
 */

#include "mm_alloc.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MIN_UNIT_SIZE 4
#define DEF_DASH_SIZE 30
#define align4(x) (((((x)-1) /(MIN_UNIT_SIZE))*(MIN_UNIT_SIZE) )+(MIN_UNIT_SIZE))
#define Block_Size (sizeof(struct MEM_Block))
void* root = NULL; 	// Root of the memory block is NULL at the beginning.

typedef long unsigned int addrtype;
typedef struct MEM_Block *Block_Ptr;
struct MEM_Block {
	size_t size;
	int free;
	Block_Ptr next;
	Block_Ptr prev;
};
Block_Ptr coalescewithNextFreeBlock(Block_Ptr mem_block);
void fillZero(Block_Ptr mem_block);
void splitInTwo(Block_Ptr prevBlock, size_t size);
Block_Ptr createMoreSpace(Block_Ptr prevBlock, size_t size);
Block_Ptr findWithfirstFit(Block_Ptr root, size_t size);
void mm_print_mem();



void *mm_malloc(size_t size) {
	Block_Ptr mem_block;
	if (size == 0)  // Return NULL if the requested size is 0.
		return NULL;

	size = align4(size); // Size must be multiple of four, since four is the unit value.

	if (root != NULL) {	// If root(the first element) is initialized.
		mem_block = findWithfirstFit(root, size);

		if (mem_block->next != NULL) { // Found a chunk.
			if (mem_block->size >= size + Block_Size + MIN_UNIT_SIZE) // If possible split in two.
				splitInTwo(mem_block, size);
			mem_block->free = 0;	// Chunk is allocated successfully.
		} else {	 // Cannot found a chunk.
			if ((mem_block = createMoreSpace(mem_block, size)) == NULL) // Break move failed.
				return NULL;

		}
	} else { // Initiate the memory.
		mem_block = createMoreSpace(NULL, size); // Extend the memory.
		if (mem_block == NULL) { // Break move failed.
			return NULL;
		}
		mem_block->prev = NULL;
		root = mem_block;	// New root is the initialized one.
	}
	fillZero(mem_block);	// Fill memory block with zero.

	return (mem_block + 1);



}

void *mm_realloc(void *ptr, size_t size) {
	Block_Ptr mem_block;
	void* adr_reallocedBlock;
	size_t oldSize;

	if (ptr == NULL) { // If size is ZERO; mm_malloc will return NULL and mm_realloc either.
					   // If Not ZERO then mm_malloc(n) will be applied.
		return mm_malloc(size);
	}
	if (size == 0)	// if size is 0 then free this data and return NULL.
		mm_free(ptr);
	return NULL;

	if (root == NULL) // Not initialized yet.
		return NULL;

	size = align4(size);	// Align to four.
	mem_block = ptr;	//Get block address.
	mem_block = mem_block - 1;

	oldSize = mem_block->size;	// Keep the old size
	if (mem_block->size >= size) {// The original size is greater than or equal the argument size.
		if (Block_Size + size + MIN_UNIT_SIZE <= mem_block->size) { // If possible split in two.
			splitInTwo(mem_block, size);
		}
		return ptr;
	} else { // The original size is less than the argument size.
		if (mem_block->next != NULL && mem_block->next->free == 1) {
			if (mem_block->size + mem_block->next->size + Block_Size >= size) { // If block can coalesce with next block.
				fillZero(mem_block->next);
				coalescewithNextFreeBlock(mem_block);

				if (Block_Size + size + MIN_UNIT_SIZE <= mem_block->size) // If possible split in two.
					splitInTwo(mem_block, size);
			}
			return ptr;
		} else {
			if ((adr_reallocedBlock = mm_malloc(size)) == NULL) // Call mm_malloc and get the address of the new data.
				return NULL;	// If mm_malloc result is NULL, return NULL.
			mem_block = (void*) adr_reallocedBlock;	// Get the address of the new block.
			mem_block--;
			mm_free(ptr);	// Free the current block.

			memcpy(adr_reallocedBlock, ptr, oldSize); // Copy the current memory to the new memory.

			return adr_reallocedBlock;	// Return the address of the new data.
		}
	}

	return NULL;
}

void mm_free(void *ptr) {
	Block_Ptr mem_block;
	if (ptr == NULL) 	// if passed pointer is NULL, do nothing.
		return;

	mem_block = ptr;	// Get block address.
	mem_block = mem_block - 1;
	mem_block->free = 1;	// Mark free as 1.

	if (mem_block->next != NULL && mem_block->next->free == 1)
		mem_block = coalescewithNextFreeBlock(mem_block);
	if (mem_block->prev != NULL && mem_block->prev->free == 1)
		mem_block = coalescewithNextFreeBlock(mem_block->prev);

	if (mem_block->next == NULL) {	// If this block is the last block.
		if (mem_block->prev == NULL) // Also there's no other block left.
			root = NULL;
		else
			mem_block->prev->next = NULL;
		brk(mem_block); //Set break to the beginning of this block.
	}
}

Block_Ptr coalescewithNextFreeBlock(Block_Ptr mem_block) {
// This function takes the block address as parameter and
// if possible coalesce with the next free block.
// Return value is the address of the coalesced block.
	if (mem_block->next != NULL && mem_block->next->free) {
		mem_block->size += Block_Size + mem_block->next->size;
		mem_block->next = mem_block->next->next;
		if (mem_block->next != NULL)
			mem_block->next->prev = mem_block;
	}
	return mem_block;
}

void fillZero(Block_Ptr mem_block) {
// This function gets the address of the block and
// Fills the data of the address with zero.
	int i;
	int* intPtr;
	intPtr = (void*) mem_block + Block_Size; // Get data address.

	for (i = 0; i < (mem_block->size / sizeof(int)); i++)	// Fill with zero byte by byte.
		intPtr[i] = 0;
}

void splitInTwo(Block_Ptr prevBlock, size_t size) {
// This function splits the free data into two
// in order to keep the fragmentation minimum.
// First part's size is the requested size and
// Second part's size is the minimum 4 byte.
// Return value is void.
	Block_Ptr newBlock;

	newBlock = prevBlock + Block_Size + size;	// Get the address of the block of second part.
	fillZero(newBlock);		// Fill the data of second part with zero.
	newBlock->free = 1;		// Free second part.
	newBlock->next = prevBlock->next;	// Define next pointer of the block of second part.
	newBlock->prev = prevBlock;	// Define prev pointer of the block of second part.
	newBlock->size = prevBlock->size - (Block_Size + size); // Define size of the block of second part.
	prevBlock->next = newBlock;	// Assign first part's next as the second part's address.
	prevBlock->size = size; // Reduce previous block in requested size.
}

Block_Ptr createMoreSpace(Block_Ptr prevBlock, size_t size) {
// This function extends the heap and create a block.
// Return value is the created block.
	void *breakMoveSuccess;
	Block_Ptr newBlock;
	newBlock = sbrk(0); // Get the address of the current break.
	breakMoveSuccess = sbrk(Block_Size + size); // Move the break as block size + requested size.
	if (breakMoveSuccess == (void*) -1) // Break move has been failed, return NULL.
		return NULL;

	newBlock->next = NULL;
	newBlock->prev = prevBlock;
	newBlock->size = size;
	if (prevBlock != NULL)  // If previous block is NULL, return the new one.
		prevBlock->next = newBlock;
	newBlock->free = 0;		// Make new block non-free.
	return newBlock;

}

Block_Ptr findWithfirstFit(Block_Ptr root, size_t size) {
// This function finds the appropriate data block
// by applying first fit algorithm. Takes the root address
// of the heap and requested size and finds the right chunk.
// Return value is the found lock
	Block_Ptr tmpBlock;
	Block_Ptr tmpOldBlock;

	if (root == NULL) // The memory space has not been initialized.
		return NULL;
	tmpBlock = root;	
	while (tmpBlock && (tmpBlock->free == 0 || size > tmpBlock->size)) {
		tmpOldBlock = tmpBlock;		// Keep the last block
		tmpBlock = tmpBlock->next;
	}
	if (tmpBlock == NULL) // There is no allocated block for the requested size.
		return tmpOldBlock;

	return tmpBlock; // A block with free is 1 and size >= the requested size has found.
}



void mm_print_mem() {

   // linked list head pointer root
    printf( "\nstart_addr\tsize\tfree\tprev\tnext\n");
    printf("=============================\n");

    // check if first block is free

    printf("HEAD OF LL %ld\n", root);
   Block_Ptr currentPtr = root;
    int i = 0;
    while (currentPtr!= NULL  && i <= 10) {
        printf("%ld\t%ld\t%d\t%ld\t%ld\n", (addrtype) currentPtr, currentPtr->size,
            currentPtr->free,currentPtr->prev,currentPtr->next);
        if (currentPtr->next == NULL) break;
        currentPtr = currentPtr->next;
        i++;
    }
}