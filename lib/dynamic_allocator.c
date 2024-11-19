/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"
#include "../inc/queue.h"
//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
__inline__ uint32 get_block_size(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (*curBlkMetaData) & ~(0x1);
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (~(*curBlkMetaData) & 0x1) ;
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockElement* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", get_block_size(blk), is_free_block(blk)) ;
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

bool is_initialized = 0;
//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++; //ensure it's multiple of 2
		if (initSizeOfAllocatedSpace == 0)
			return ;
		is_initialized = 1;
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("initialize_dynamic_allocator is not implemented yet");
	LIST_INIT(&freeBlocksList);
	struct BlockElement* FirstFreeBlock = (struct BlockElement*)(daStart + 2 * sizeof(int));
	uint32 *BEG_BLOCK = (uint32*)(daStart);
	uint32 *END_BLOCK = (uint32*)(daStart + initSizeOfAllocatedSpace - sizeof(int));
	END_BLK = END_BLOCK;
	*BEG_BLOCK = *END_BLOCK = 1;
	uint32 *HEADER = (uint32*)(daStart + sizeof(int));
	uint32 *FOOTER = (uint32*)(daStart + initSizeOfAllocatedSpace - 2 * sizeof(int));
	*HEADER = *FOOTER = initSizeOfAllocatedSpace - 2 * sizeof(int);

	LIST_INSERT_HEAD(&freeBlocksList, FirstFreeBlock);

}
//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("set_block_data is not implemented yet");
	//Your Code is Here...

	//printf("I am in set_block_data function");

	/* check if the totalSize satisfy block data size conditions */
	if(totalSize%2 == 1 || totalSize < 16)
	{
		cprintf("Error: Block size must be even and at least 16 Bytes\n");
		return;
	}

	/* header is pointing on the address before va with in 4 bytes */
	uint32 *header = ((uint32 *)va - 1);

	/*
	   the size of data block must be even because the first bit
	   of header or footer is the flag for allocation
	*/
	if(!isAllocated)
	*header = totalSize;
	else
	*header = totalSize + 1;

	/* footer is pointing on the address after va with in totalSize - 8 bytes */
	uint32 *footer = (uint32 *)((char *)va + totalSize - 8);

	if(!isAllocated)
	*footer = totalSize;
	else
	*footer = totalSize + 1;

	/*
	   some of printing to test the function by using the command set_block_data_test
	   , first argument is totalSize and the second argument is isAllocated
	*/
	/*cprintf("initial pointer: %p\n",va);
	cprintf("header address: %p and header size: %u\n",header,*header);
	cprintf("footer address: %p and footer size: %u\n",footer,*footer);*/
	/*
		cprintf("header address: %p \n",header);
		cprintf("footer address: %p \n",footer);
	*/
}

int cnt = 0;
//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{

	if(size == 0)
		return NULL;

	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0) size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
		if (!is_initialized)
		{
			uint32 required_size = size + 2*sizeof(int) /*header & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
			uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE);
			uint32 da_break = (uint32)sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("alloc_block_FF is not implemented yet");
	//Your Code is Here...

	//! Adding the size of the header and footer
	uint32 reqSize = size + 2 * sizeof(int);

	struct BlockElement *targetBlock;
	int curSize = 0;

	short action = -1;
	LIST_FOREACH(targetBlock, &freeBlocksList)
	{
		curSize = get_block_size(targetBlock);
		if(curSize >= reqSize)
		{
			action = (curSize - reqSize < 4 * sizeof(int));
			break;
		}
	}

	if(action == -1)
	{
		uint32 numOfPages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
		void *ret = sbrk(numOfPages);
		uint32 extraSize = numOfPages * PAGE_SIZE;

		if(ret == (void* )-1)
			return NULL;
		struct BlockElement *prv = NULL;
		if(LIST_SIZE(&freeBlocksList) > 0)
			prv = LIST_LAST(&freeBlocksList);
		struct BlockElement *cur = (struct BlockElement *)( ret);
		LIST_INSERT_TAIL(&freeBlocksList, cur);
		set_block_data(cur, extraSize, 0);

		bool hasPrv = (prv != NULL);
		if(prv != NULL)
			hasPrv &= ((struct BlockElement *)((char *)cur - get_block_size(prv)) == prv);

		if(hasPrv)
		{
			LIST_REMOVE(&freeBlocksList, cur);
			uint32 size = get_block_size(prv);
			set_block_data(prv, size + extraSize, 0);
		}


	}
	LIST_FOREACH(targetBlock, &freeBlocksList)
	{
		curSize = get_block_size(targetBlock);
		if(curSize >= reqSize)
		{
			action = (curSize - reqSize < 4 * sizeof(int));
			break;
		}
	}
	uint32 settingSize = (action ? curSize : reqSize);

	if(!action)
	{
		struct BlockElement *newBlock = (struct BlockElement*)((char*) targetBlock + reqSize);
		set_block_data(newBlock, curSize - reqSize, 0);
		LIST_INSERT_AFTER(&freeBlocksList, targetBlock, newBlock);
	}

	set_block_data(targetBlock, settingSize, 1);
	LIST_REMOVE(&freeBlocksList, targetBlock);

	return targetBlock;
}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_BF is not implemented yet");

	//Your Code is Here...
	{
		if (size % 2 != 0) size++;
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE;
	}

	uint32 reqSize = size + 2 * sizeof(int);
	struct BlockElement *targetBlock, *iterator;
	uint32 curSize = 0, targetSize = 0;
	int difference = -1;

	LIST_FOREACH(iterator, &freeBlocksList)
	{
		curSize = get_block_size(iterator);
		if(curSize >= reqSize)
		{
			if(difference == -1 || curSize - reqSize < difference)
			{
				targetSize = curSize;
				difference = curSize - reqSize;
				targetBlock = iterator;
			}
		}
	}

	if(difference == -1)
	{
		sbrk(0);
		return NULL;
	}

	bool x = (targetSize - reqSize < 4 * sizeof(int));

	int settingSize = (x ? targetSize : reqSize);

	if(!x)
	{
		struct BlockElement *newBlock = (struct BlockElement*)((char*) targetBlock + reqSize);
		set_block_data(newBlock, targetSize - reqSize, 0);
		LIST_INSERT_AFTER(&freeBlocksList, targetBlock, newBlock);
	}

	set_block_data(targetBlock, settingSize, 1);
	LIST_REMOVE(&freeBlocksList, targetBlock);
	return targetBlock;
}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("free_block is not implemented yet");
	//Your Code is Here...
	if(va == NULL || is_free_block(va))
		return;
	uint32 currentSize = get_block_size(va);
	set_block_data(va, currentSize, 0);
//	print_blocks_list(freeBlocksList);

	struct BlockElement* currentBlock = (struct BlockElement*)va;

	if(LIST_SIZE(&freeBlocksList) == 0)
	{
		LIST_INSERT_HEAD(&freeBlocksList, currentBlock);
	}
	else
	{
		struct BlockElement *targetBlock, *destinationBlock = NULL;
		LIST_FOREACH(targetBlock, &freeBlocksList)
		{
			if(targetBlock > currentBlock)
				break;
			destinationBlock = targetBlock;
		}
		if(destinationBlock != NULL)
			LIST_INSERT_AFTER(&freeBlocksList, destinationBlock, currentBlock);
		else
			LIST_INSERT_HEAD(&freeBlocksList, currentBlock);
	}
	struct BlockElement *prv = NULL, *nxt = NULL, *iterator;
	LIST_FOREACH(iterator, &freeBlocksList)
	{
		if(iterator > currentBlock)
		{
			nxt = iterator;
			break;
		}
		if(iterator != currentBlock)
			prv = iterator;
	}
	bool hasNext = (nxt != NULL);
	if(nxt != NULL)
		hasNext &= ((struct BlockElement *)((char *)currentBlock + get_block_size(currentBlock)) == nxt);
	bool hasPrv = (prv != NULL);
	if(prv != NULL)
		hasPrv &= ((struct BlockElement *)((char *)currentBlock - get_block_size(prv)) == prv);

	if(hasNext && !hasPrv)
	{
		LIST_REMOVE(&freeBlocksList, nxt);
		uint32 size = get_block_size(nxt);
		set_block_data(currentBlock, size + currentSize, 0);
	}
	else if(hasPrv && !hasNext)
	{
		LIST_REMOVE(&freeBlocksList, currentBlock);
		uint32 size = get_block_size(prv);
		set_block_data(prv, size + currentSize, 0);
	}
	else if(hasPrv && hasNext)
	{
		LIST_REMOVE(&freeBlocksList, nxt);
		uint32 size = get_block_size(nxt);
		LIST_REMOVE(&freeBlocksList, currentBlock);
	    size += get_block_size(prv);
		set_block_data(prv, size + currentSize, 0);
	}

}

//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size)
{
	//TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("realloc_block_FF is not implemented yet");
	//Your Code is Here...


    if(va != NULL && !new_size)
    {
    	free_block(va);
    	return NULL;
    }

    if(va == NULL)
    {
    	if(new_size)
    	{
    		return alloc_block_FF(new_size);
    	}
    	else
    	return NULL;
    }

    /*block size must be even and at least 16 bytes*/
    if (new_size % 2 != 0) new_size++;
	if (new_size < DYN_ALLOC_MIN_BLOCK_SIZE)
		new_size = DYN_ALLOC_MIN_BLOCK_SIZE ;

	/*adding meta data to block size*/
	new_size += 8;

	/*size of allocated block*/
    uint32 actual_size = get_block_size(va);
    void *next_block = (char*)va + actual_size;
	uint32 next_block_size = get_block_size(next_block);
	/*next block is free or not*/
	uint32 next_block_statue = is_free_block(next_block);

    if(new_size > actual_size)
    {
    	/* the next block is free and size of it can hold the rest of new size*/
        if(next_block_statue && next_block_size >= new_size - actual_size)
        {
        	/*
        	   if the rest of next block size more than 16 bytes
        	   then split the next block to allocated block and free block
        	*/
        	if(next_block_size - (new_size - actual_size) >= 16)
        	{
        		set_block_data(va, new_size, 1);
        		set_block_data((void*)((char*)va + new_size), next_block_size - (new_size - actual_size), 0);
        		struct BlockElement *new_free_block = (struct BlockElement*)((char*)va + new_size);
        		LIST_INSERT_AFTER(&freeBlocksList, (struct BlockElement*)next_block, new_free_block);
        		LIST_REMOVE(&freeBlocksList, (struct BlockElement*)next_block);
        	}
        	/* take the size of the entire next block*/
        	else
        	{
        		set_block_data(va, actual_size + next_block_size, 1);
        		LIST_REMOVE(&freeBlocksList, (struct BlockElement*)next_block);
        	}

        	return va;
        }
        else
        {
        	/*copy the data of the allocated block*/
        	char arr[actual_size];
        	for(int i=0 ; i < actual_size ; i++)
        	{
        		arr[i] = *((char*)va + i);
        	}

        	free_block(va);
        	void *new_block_address = alloc_block_FF(new_size - 8);

        	/*paste the data of the allocated block to the new block after reallocation*/
        	for(int i=0 ; i < actual_size; i++)
        	{
        		*((char*)new_block_address + i) = arr[i];
        	}
        	return new_block_address;
        }

    }
    else
    {
    	/*if the rest of actual size more than or equal to 16 bytes*/
    	if(actual_size - new_size >= 16)
    	{
    		set_block_data(va,new_size,1);
            /*
                 if the next block is also free
                 then split block to allocated block and free block and
                 merge the free block with the next block
            */
    		if(!next_block_statue)
    		{
    			set_block_data((void*)((char*)va + new_size),actual_size - new_size, 0);

				struct BlockElement* currentBlock = (struct BlockElement*)((char*)va + new_size);
				struct BlockElement *targetBlock, *destinationBlock = NULL;
				LIST_FOREACH(targetBlock, &freeBlocksList)
				{
					if(targetBlock > currentBlock)
					break;
					destinationBlock = targetBlock;
				}
				if(destinationBlock != NULL)
				LIST_INSERT_AFTER(&freeBlocksList, destinationBlock, currentBlock);
				else
				LIST_INSERT_HEAD(&freeBlocksList, currentBlock);
    		}
    		/*
    		     if next block is not free
    		     then split the block
    		*/
    		else
    		{
    			set_block_data((void*)((char*)va + new_size),(actual_size - new_size) + next_block_size, 0);
    			LIST_INSERT_AFTER(&freeBlocksList, (struct BlockElement*)next_block, (struct BlockElement*)((char*)va + new_size));
    			LIST_REMOVE(&freeBlocksList, (struct BlockElement*)next_block);
    		}
    	}

    	/*
    	    if the rest of actual size is less than 16 bytes
    	    then return the address of allocated block
    	*/

    	return va;
    }
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}
