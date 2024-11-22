#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"
//2017
#define DYNAMIC_ALLOCATOR_DS 0 //ROUNDUP(NUM_OF_KHEAP_PAGES * sizeof(struct MemBlock), PAGE_SIZE)
#define INITIAL_KHEAP_ALLOCATIONS (DYNAMIC_ALLOCATOR_DS) //( + KERNEL_SHARES_ARR_INIT_SIZE + KERNEL_SEMAPHORES_ARR_INIT_SIZE) //
#define INITIAL_BLOCK_ALLOCATIONS ((2*sizeof(int) + MAX(num_of_ready_queues * sizeof(uint8), DYN_ALLOC_MIN_BLOCK_SIZE)) + (2*sizeof(int) + MAX(num_of_ready_queues * sizeof(struct Env_Queue), DYN_ALLOC_MIN_BLOCK_SIZE)))
#define ACTUAL_START ((KERNEL_HEAP_START + DYN_ALLOC_MAX_SIZE + PAGE_SIZE) + INITIAL_KHEAP_ALLOCATIONS)

//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	//TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator
	// Write your code here, remove the panic and write your code
	//panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");
	/*
	 * Return:
       On success: 0
       Otherwise (if no memory OR initial size exceed the given limit): kernel should panic()
	 *
	 */
	if(daStart + initSizeToAllocate > daLimit){
		panic("no memory");
	}
	//1- Initialize the block allocator of kernel heap with the given start address, size & limit
	kheap_st = daStart;
	kheap_sbrk =  (daStart + initSizeToAllocate);
	kheap_hlim =  daLimit;
	startPageAllocator = kheap_hlim + PAGE_SIZE;
	//2- All pages in the given range should be allocated and mapped
    for(uint32 it = kheap_st;it!=kheap_sbrk;it+=PAGE_SIZE ){
    	 struct FrameInfo *ptr_frame_info;
    	 int temp = allocate_frame(&ptr_frame_info) ;
         if(temp!=0){
        	 panic("");
         }
         map_frame(ptr_page_directory,ptr_frame_info,it,PERM_WRITEABLE);
         ptr_frame_info->va=it;
    }

    //3- Remember: call the initialize_dynamic_allocator(..) to complete the initialization
    initialize_dynamic_allocator (daStart,initSizeToAllocate);
    return 0;
}

void* sbrk(int numOfPages)
{
	/* numOfPages > 0: move the segment break of the kernel to increase the size of its heap by the given numOfPages,
	 * 				you should allocate pages and map them into the kernel virtual address space,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, return -1
	 */

	//MS2: COMMENT THIS LINE BEFORE START CODING==========
//	return (void*)-1 ;
	//====================================================

	//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
	if(numOfPages == 0)
		return (void * )kheap_sbrk;
	if(kheap_sbrk + numOfPages * PAGE_SIZE > kheap_hlim)
		return (void *) -1;
	int cnt = numOfPages;
	for(uint32 curVa = kheap_sbrk; cnt >= 1 && curVa < kheap_hlim; cnt--, curVa += PAGE_SIZE)
	{
		struct FrameInfo *ptr_frame_info;
		int temp = allocate_frame(&ptr_frame_info) ;
		if(temp!=0)
			return (void*)-1;
		 map_frame(ptr_page_directory,ptr_frame_info,curVa,PERM_WRITEABLE);
		 ptr_frame_info->va=curVa;
	}

	uint32 ret = kheap_sbrk;
	END_BLK = (uint32 *)(numOfPages * PAGE_SIZE + (char *)END_BLK);
	*END_BLK = 1;
	kheap_sbrk += numOfPages * PAGE_SIZE;
	return (void * )(ret);

}

//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

void* kmalloc(unsigned int size)
{

		if(size <= DYN_ALLOC_MAX_BLOCK_SIZE){
			// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
			if(isKHeapPlacementStrategyFIRSTFIT())
			{	return alloc_block_FF(size);}
			else if(isKHeapPlacementStrategyBESTFIT())
			{return alloc_block_BF(size);}
			else if(isKHeapPlacementStrategyNEXTFIT())
			{return alloc_block_NF(size);}
			else if(isKHeapPlacementStrategyWORSTFIT())
			{return alloc_block_WF(size);}
		}

	uint32 numOfPages = ROUNDUP(size, PAGE_SIZE)/PAGE_SIZE;

	uint32 cur=0;
	uint32 *curFrame;
	bool has_prev = 0;
	struct FrameInfo *lst = NULL;
	for(uint32 i = startPageAllocator; i<KERNEL_HEAP_MAX;)
	{
		struct FrameInfo *temp = get_frame_info(ptr_page_directory , i ,&curFrame);
		if(temp == NULL)
			cur++;
		else
		{
			lst = temp;

			if(temp->nxt != NULL)
			{
				if((temp->nxt->va - (i + (temp->after + 1) * PAGE_SIZE) + PAGE_SIZE - 1) / PAGE_SIZE < numOfPages)
					i = temp->nxt->va;
				else
					i += (temp->after + 1) * PAGE_SIZE;
			}
			else
				i += (temp->after + 1) * PAGE_SIZE;
			cur = 0;
			continue;
		}
		if(cur == numOfPages)
		{
			uint32 initial = i - (numOfPages - 1) * PAGE_SIZE;
			uint32 before = 0, after = numOfPages - 1;
			if(initial == startPageAllocator)
				startPageAllocator += (after + 1) * PAGE_SIZE;

			for(uint32 l2 = initial; l2 <= i; l2 += PAGE_SIZE)
			{
				struct FrameInfo *frame;
				allocate_frame(&frame);
				map_frame(ptr_page_directory,frame,l2,PERM_WRITEABLE);
				frame->va=l2;
                frame->before = before;
                frame->after = after;
                before++;
                after--;
                if(l2 == initial)
                {
                	if(lst != NULL)
                	{
                		frame->nxt = lst->nxt;
                		lst->nxt = frame;
                		frame->bef = lst;
                		if(frame->nxt != NULL)
                			frame->nxt->bef = frame;
                	}
                	else
                	{
                		if(HEAD != NULL && HEAD->va > frame->va)
                		{
                			frame->nxt = HEAD;
                			HEAD = frame;
                		}
                	}
                	if(HEAD == NULL)
                	{
                		HEAD = frame;
                	}
                }
			}
			return (uint32*)initial;

		}
		i += PAGE_SIZE;
	}


   return NULL;

}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
	uint32 page_alloc_st = kheap_hlim + PAGE_SIZE;
    if(virtual_address < (void *)kheap_st || virtual_address > (void *) (KERNEL_HEAP_MAX)){
    	panic("");
    }else if(virtual_address >= (void *)(kheap_st) && virtual_address < (void *)(kheap_sbrk) ){
    	free_block(virtual_address);
    }else if(virtual_address >= (void *)(page_alloc_st) && virtual_address < (void *) KERNEL_HEAP_MAX ){
    	uint32 *tempp;
    //	struct FrameInfo *frame = get_frame_info(ptr_page_directory , (uint32)virtual_address ,&tempp);
    	uint32 curr_va = get_frame_info(ptr_page_directory , (uint32)virtual_address ,&tempp)->va;
    	uint32 before = get_frame_info(ptr_page_directory , (uint32)virtual_address ,&tempp)->before;
    	uint32 after = get_frame_info(ptr_page_directory , (uint32)virtual_address ,&tempp)->after;
    	if(get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp) == HEAD)
    	{
    		if(get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp)->nxt != NULL)
    			HEAD = get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp)->nxt;
    		else
    			HEAD = NULL;
    	}
    	if(get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp)->va < startPageAllocator)
    		startPageAllocator = get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp)->va;
    	if(get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp)->bef != NULL)
    	{

    		get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp)->bef->nxt =
    				get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp)->nxt;
    		if(get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp)->bef->nxt != NULL)
    			get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp)->bef->nxt->bef =
    					get_frame_info(ptr_page_directory , (uint32)virtual_address - before * PAGE_SIZE ,&tempp)->bef;

    	}

    	//kpanic_into_prompt("frame ref ,%d",frame->references);
    	//unmap_frame(ptr_page_directory,curr_va);
    	for(uint32 i=0,it=curr_va-PAGE_SIZE;i<before;i++,it-=PAGE_SIZE){

    		unmap_frame(ptr_page_directory,it);
    		//cprintf("before\n");
    	}
    	for(uint32 i=0; i<= after;i++,curr_va+=PAGE_SIZE){

    	    		unmap_frame(ptr_page_directory,curr_va);
    	    	//	cprintf("after\n");
    	 }

    }else{
    	panic("");
    }
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details


}

unsigned int kheap_physical_address(unsigned int virtual_address)
{

	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");
		 unsigned int off=virtual_address%PAGE_SIZE;//offset
		 unsigned int startAddress=virtual_address-off; // va start address
		 uint32 *tempP;
		 struct FrameInfo *temp = get_frame_info(ptr_page_directory , startAddress ,&tempP);

		 if(temp != 0){
			  uint32 physicalAddress = to_physical_address(temp);
		      return physicalAddress+off;
		     }


		//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
		 return 0;


}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	 unsigned int off=physical_address%PAGE_SIZE;//offset
	 unsigned int framenum=physical_address-off;
	 struct FrameInfo *frame=to_frame_info(framenum);
	 unsigned int va=frame->va;
	 if(frame->references !=0){
	      return frame->va+off;
	     }


	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
	 return 0;
}
//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");
}
