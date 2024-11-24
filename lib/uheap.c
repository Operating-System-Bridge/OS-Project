#include <inc/lib.h>
uint32 mazen[122879]={0};//marked - > va , unmarked -> 0
//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return (void *)NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	//return NULL;
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy

	// Block Allocation
	if(size <= DYN_ALLOC_MAX_BLOCK_SIZE)
	{
		if(sys_isUHeapPlacementStrategyFIRSTFIT())
			return alloc_block_FF(size);
		else if(sys_isUHeapPlacementStrategyBESTFIT())
			return alloc_block_BF(size);

	}
	//	return (void *)NULL;


	// Page Allocation

	uint32 startAddress = myEnv->hlimit + PAGE_SIZE;
	// Start of page allocator is after the hard limit with one page

	int cnt = 0;
	uint32 goal = ROUNDUP(size ,PAGE_SIZE) / PAGE_SIZE; // num of needed pages
	for(uint32 i = startAddress,it=0;i<(uint32)USER_HEAP_MAX;i+=PAGE_SIZE,it++){
		//cprintf("it %d\n",it);
		if(mazen[it]==0){
			cnt++;
		}else{
			startAddress = i + PAGE_SIZE;
			cnt = 0;
		}
		if(cnt==goal){
			for(int j=0;j<goal;j++)
				mazen[it-j]=startAddress;
			sys_allocate_user_mem(startAddress, cnt * PAGE_SIZE);
			return (void *)startAddress;
		}
	}
	//char* va = (char*)startAddress;
	//char* potentialStart = (char*) startAddress;
	//bool foundEnough = 0; // If this is 0 after the for loop return -1

   return (void *)NULL;
}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	uint32 size = 0;
	uint32 st_page =myEnv->hlimit + PAGE_SIZE;
	if(virtual_address>=(void *)USER_HEAP_START && virtual_address < (void *)myEnv->brk){
		free_block(virtual_address);
	}else if(virtual_address>=(void *)st_page && virtual_address <(void *)USER_HEAP_MAX){
		int done = 0;
		for(int i=0;i<122879;i++){
			if(mazen[i]==(uint32)virtual_address){
				mazen[i]=0;
				done = 1;
				size+=PAGE_SIZE;
			}else if(done==1){
				break;
			}
		}
	 sys_free_user_mem((uint32)virtual_address,size);

	}else{
	panic("invalid virtual address");
	}
}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
	panic("smalloc() is not implemented yet...!!");
	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
	// Write your code here, remove the panic and write your code
	panic("sget() is not implemented yet...!!");
	return NULL;
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [USER SIDE] - sfree()
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}


//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//[PROJECT]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}
