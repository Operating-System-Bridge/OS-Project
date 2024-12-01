#include <inc/memlayout.h>
#include "shared_memory_manager.h"

#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/queue.h>
#include <inc/environment_definitions.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/syscall.h>
#include "kheap.h"
#include "memory_manager.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
struct Share* get_share(int32 ownerID, char* name);

//===========================
// [1] INITIALIZE SHARES:
//===========================
//Initialize the list and the corresponding lock
void sharing_init()
{
#if USE_KHEAP
	LIST_INIT(&AllShares.shares_list) ;
	init_spinlock(&AllShares.shareslock, "shares lock");
#else
	panic("not handled when KERN HEAP is disabled");
#endif
}

//==============================
// [2] Get Size of Share Object:
//==============================
int getSizeOfSharedObject(int32 ownerID, char* shareName)
{
	//[PROJECT'24.MS2] DONE
	// This function should return the size of the given shared object
	// RETURN:
	//	a) If found, return size of shared object
	//	b) Else, return E_SHARED_MEM_NOT_EXISTS
	//
	struct Share* ptr_share = get_share(ownerID, shareName);
	if (ptr_share == NULL)
		return E_SHARED_MEM_NOT_EXISTS;
	else
		return ptr_share->size;

	return 0;
}

//===========================================================


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
//===========================
// [1] Create frames_storage:
//===========================
// Create the frames_storage and initialize it by 0
inline struct FrameInfo** create_frames_storage(int numOfFrames)
{
    //TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_frames_storage()
    //COMMENT THE FOLLOWING LINE BEFORE START CODING
//    panic("create_frames_storage is not implemented yet");
    //Your Code is Here...
    /*
     * Create an array of pointers to struct FrameInfo of size numOfFrames
        initialize it by ZEROs
        Return:
        if succeed: pointer to the created array
        if failed: NULL
        Testing:
         Will be tested during the other tests…
     *
     */
    struct FrameInfo** frames = (struct FrameInfo**)(kmalloc(numOfFrames * sizeof(struct FrameInfo*)));
    if (frames == NULL)
    	return NULL;

    memset(frames, 0, numOfFrames * sizeof(struct FrameInfo*));

    return frames;
}

//=====================================
// [2] Alloc & Initialize Share Object:
//=====================================
//Allocates a new shared object and initialize its member
//It dynamically creates the "framesStorage"
//Return: allocatedObject (pointer to struct Share) passed by reference
struct Share* create_share(int32 ownerID, char* shareName, uint32 size, uint8 isWritable)
{
    //TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_share()
    //COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("create_share is not implemented yet");
    //Your Code is Here...
        // Allocate memory for a new Share object
        acquire_spinlock(&AllShares.shareslock);
        struct Share* newShare = (struct Share*)kmalloc(sizeof(struct Share));
        if (newShare == NULL)
        {
            release_spinlock(&AllShares.shareslock);
            return NULL;
        }
        // Initialize the fields of the Share object
        newShare->ID = (int32)newShare & 0x7FFFFFFF; // Use the address as ID, masking MSB to make it positive
        newShare->ownerID = ownerID;
        strncpy(newShare->name, shareName, sizeof(newShare->name) - 1);
        newShare->size = size;
        newShare->isWritable = isWritable;
        newShare->references = 1;

        // Calculate the number of frames required and allocate framesStorage
        int numOfFrames = ROUNDUP(size, PAGE_SIZE)/PAGE_SIZE;
        newShare->framesStorage = create_frames_storage(numOfFrames);

        if (newShare->framesStorage == NULL)
        {
            kfree(newShare); // Free the Share object if framesStorage allocation fails
            release_spinlock(&AllShares.shareslock);
            return NULL;
        }

        release_spinlock(&AllShares.shareslock);

        return newShare;
}

//=============================
// [3] Search for Share Object:
//=============================
//Search for the given shared object in the "shares_list"
//Return:
//	a) if found: ptr to Share object
//	b) else: NULL
struct Share* get_share(int32 ownerID, char* name)
{
	//TODO: [PROJECT'24.MS2 - #17] [4] SHARED MEMORY - get_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("get_share is not implemented yet");
	//Your Code is Here...
	struct Share *curShare;
	acquire_spinlock(&AllShares.shareslock);
	bool found = 0;
	LIST_FOREACH(curShare, &AllShares.shares_list)
	{
		if(strcmp(curShare->name, name) == 0 && curShare->ownerID == ownerID)
		{
			found = 1;
			break;
		}
	}

	release_spinlock(&AllShares.shareslock);
	if(found)
		return curShare;
	else
		return NULL;

}

//=========================
// [4] Create Share Object:
//=========================
int createSharedObject(int32 ownerID, char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #19] [4] SHARED MEMORY [KERNEL SIDE] - createSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("createSharedObject is not implemented yet");

	//Your Code is Here...
	struct Env* myenv = get_cpu_proc(); //The calling environment

	if(get_share(ownerID, shareName) != NULL)
		return E_SHARED_MEM_EXISTS;

	int req = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	if(LIST_SIZE(&MemFrameLists.free_frame_list) < req)
		return E_NO_SHARE;

	struct Share *newShare = create_share(ownerID, shareName, size, isWritable);
	if(newShare == NULL)
		return E_NO_SHARE;



	for(uint32 va = (uint32)virtual_address, i = 0; i < req; va += PAGE_SIZE, i++)
	{
		struct FrameInfo *curFrame;
		allocate_frame(&curFrame);
		map_frame(myenv->env_page_directory, curFrame, va, PERM_USER | PERM_WRITEABLE |PERM_PRESENT);
        newShare->framesStorage[i] = curFrame;
	}
	acquire_spinlock(&AllShares.shareslock);
	LIST_INSERT_HEAD(&AllShares.shares_list, newShare);
	release_spinlock(&AllShares.shareslock);
	return newShare->ID;
}


//======================
// [5] Get Share Object:
//======================
int getSharedObject(int32 ownerID, char* shareName, void* virtual_address)
{

//	//TODO: [PROJECT'24.MS2 - #21] [4] SHARED MEMORY [KERNEL SIDE] - getSharedObject()
//	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("getSharedObject is not implemented yet");
//	//Your Code is Here...
	struct Env* myenv = get_cpu_proc(); //The calling environment
	struct Share *curShare = get_share(ownerID, shareName);
	if(curShare == NULL)
		return E_SHARED_MEM_NOT_EXISTS;

	struct FrameInfo *curFrame;
	uint32 n = sizeof(curShare->framesStorage) / sizeof(curShare->framesStorage[0]);
	curFrame = curShare->framesStorage[0];
	for(uint32 i = 0, curVa = (uint32)virtual_address; i < n; i++, curVa += PAGE_SIZE)
	{
		if(curShare->isWritable == 1)
			map_frame(myenv->env_page_directory, curFrame,curVa, PERM_USER | PERM_PRESENT | PERM_WRITEABLE);
		else
			map_frame(myenv->env_page_directory, curFrame,curVa, PERM_USER | PERM_PRESENT);
		if(i != n - 1)
			curFrame++;
	}
	curShare->references++;
	return curShare->ID;
}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//==========================
// [B1] Delete Share Object:
//==========================
//delete the given shared object from the "shares_list"
//it should free its framesStorage and the share object itself
void free_share(struct Share* ptrShare)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - free_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("free_share is not implemented yet");
	//Your Code is Here...
	    // Free frames storage
      kfree(ptrShare->framesStorage);
	  LIST_REMOVE( &AllShares.shares_list,ptrShare);
	  kfree(ptrShare);
}
//========================
// [B2] Free Share Object:
//========================

int freeSharedObject(int32 sharedObjectID, void *startVA)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - freeSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("freeSharedObject is not implemented yet");
	//Your Code is Here...
	cprintf("goal : %d\n",sharedObjectID);
	struct Share *share = NULL;
	acquire_spinlock(&AllShares.shareslock);

	    // Find the shared object by ID
	    LIST_FOREACH(share, &AllShares.shares_list)
	    {
	    	cprintf("search :%d and : %d\n",share->ID,share->ID&sharedObjectID);
	        if (share->ID == sharedObjectID)
	            break;
	    }

	    // If not found, return error
	    if (!share)
	    {
	        release_spinlock(&AllShares.shareslock);
	        return E_SHARED_MEM_NOT_EXISTS;
	    }
	    struct Env* myenv = get_cpu_proc();
	    // Unmap all frames in the range
	    for (uint32 va = (uint32)startVA, i = 0; i < ROUNDUP(share->size, PAGE_SIZE) / PAGE_SIZE; va += PAGE_SIZE, i++)
	    {
	        unmap_frame(myenv->env_page_directory, va);

	        // If a page table becomes empty, remove it
	        uint32 *pt = NULL;
	        get_page_table(myenv->env_page_directory, va, &pt);
	            myenv->env_page_directory[PDX(va)] = 0;
	            tlbflush();

	    }

	    // Decrement references or free the share if no references remain
	    share->references--;
	    if (share->references == 0)
	    {
	        free_share(share);
	    }

	    release_spinlock(&AllShares.shareslock);
	    return 0; // Success
}
int getSharedObjectID(void * virtual_address)
{
    struct Share *share;

    // Acquire lock to protect shared list
    acquire_spinlock(&AllShares.shareslock);
     uint32 va = (uint32)virtual_address;
    // Traverse the shares list to find a match for the virtual address
    LIST_FOREACH(share, &AllShares.shares_list)
    {
        uint32 numOfFrames = ROUNDUP(share->size, PAGE_SIZE) / PAGE_SIZE;
        uint32 startVA = (uint32)share->framesStorage[0]->va; // Starting virtual address of the shared object

        // Check if the given virtual address falls within the range of the share
        if (va >= startVA && va < startVA + (numOfFrames * PAGE_SIZE))
        {
            release_spinlock(&AllShares.shareslock);
            return share->ID; // Return the ID of the found share
        }
    }

    release_spinlock(&AllShares.shareslock);

    // If no match is found, return error code
    return E_SHARED_MEM_NOT_EXISTS;
}
