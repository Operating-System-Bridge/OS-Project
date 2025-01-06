# FOS'24 Project  


## About The Project  

The OS'24 Project is a comprehensive, hands-on implementation of an operating system designed to teach fundamental concepts like memory management, process scheduling, synchronization, and fault handling. It emphasizes modularity and provides students with a strong foundation in systems programming.  


---  

## Modules  

### Command Prompt  
- A simple command-line interface to execute kernel-level commands.  
- Supports commands like `kernel_info`, `nclock`, and others for testing and interaction.  

### System Calls  
- Provides a bridge between user programs and the kernel, enabling secure execution of privileged operations.  
- Validates user-provided pointers and ensures proper handling of invalid memory access.  

### Dynamic Allocator  
- **Kernel Heap**: Implemented block and page-level allocators for dynamic memory allocation.  
  - **First-Fit Strategy**: Used for both block and page allocations.  
  - Supports functions like `kmalloc`, `kfree`.  
- **User Heap**: Memory allocation for user programs with lazy allocation using system calls.  

### Memory Manager  
- **Page Allocator**: Handles memory on a page-level granularity for efficient management.  
- **Shared Memory**: Allows multiple processes to share memory regions for interprocess communication.  

### Fault Handler  
- **Page Fault Handling**: Implements lazy allocation and replacement policies to manage memory faults efficiently.  
- **Nth Chance Clock Replacement**: Optimized page replacement algorithm for balancing performance and memory utilization.  

### Synchronization and protection (Locks & Semaphores)  
- **Locks**:  
  - Implemented spinlocks for short critical sections with busy-waiting.  
  - Designed sleeplocks to handle longer critical sections by blocking threads and avoiding CPU wastage.  
  - Ensures safe access to shared kernel resources.  
- **Semaphores**:  
  - User-level semaphores to synchronize processes.  
  - Operations include:  
    - **semWait**: Decreases the semaphore value and blocks the process if the value is negative.  
    - **semSignal**: Increases the semaphore value and unblocks waiting processes if needed.  
  - Handles common issues like deadlocks and priority inversion with proper locking mechanisms.  

### CPU Scheduler  
- **Priority Round-Robin Scheduler**:  
  - Preemptive scheduling with multiple priority levels.  
  - Prevents starvation by promoting processes based on their waiting time.  
  - Supports commands like `schedPRIRR` for dynamic configuration.  

<img align ="center" alt = "Coding" width="500"  src="https://media.giphy.com/media/AnZJpnQwAC29W/giphy.gif?cid=790b76117zo3zvfyq94wge4kehcr77af1mcanz25gq9fyxmu&ep=v1_gifs_search&rid=giphy.gif&ct=g">



