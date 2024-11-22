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
#include <inc/uheap.h>

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
    panic("create_frames_storage is not implemented yet");
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
//    struct FrameInfo** frames = (struct FrameInfo**)malloc(numOfFrames * sizeof(struct FrameInfo*));
//    if (frames == NULL)
//    {return NULL;}
//
//    memset(frames, 0, numOfFrames * sizeof(struct FrameInfo*));
//
//    return frames;
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
	panic("create_share is not implemented yet");
	//Your Code is Here...

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
	panic("createSharedObject is not implemented yet");
	//Your Code is Here...

	struct Env* myenv = get_cpu_proc(); //The calling environment


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
	int n = sizeof(curShare->framesStorage) / sizeof(curShare->framesStorage[0]);
	curFrame = curShare->framesStorage[0];
	for(uint32 i = 0, curVa = (uint32)virtual_address; i < n; i++, curVa += PAGE_SIZE)
	{
		if(curShare->isWritable == 1)
			map_frame(ptr_page_directory,curFrame,curVa, PERM_WRITEABLE);
		else
			map_frame(ptr_page_directory,curFrame,curVa, 0);
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
	panic("free_share is not implemented yet");
	//Your Code is Here...

}
//========================
// [B2] Free Share Object:
//========================
int freeSharedObject(int32 sharedObjectID, void *startVA)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - freeSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("freeSharedObject is not implemented yet");
	//Your Code is Here...

}
