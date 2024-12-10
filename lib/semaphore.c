// User-level Semaphore

#include "inc/lib.h"
#include "inc/semaphore.h"
struct semaphore create_semaphore(char *semaphoreName, uint32 value)
{
    //TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
    //COMMENT THE FOLLOWING LINE BEFORE START CODING
//    panic("create_semaphore is not implemented yet");
    //Your Code is Here...

    struct semaphore *curSemphore = smalloc(semaphoreName, sizeof(struct semaphore), 1);
    cprintf("Smalloced\n");
    if(curSemphore == NULL)
    {
        panic("Not enough memory\n");
    }
    cprintf("WILL SET COUNT NOW\n");
    curSemphore->semdata = malloc(sizeof(struct __semdata));
    curSemphore->semdata->count = value;
    cprintf("Set count value\n");
    strncpy(curSemphore->semdata->name, semaphoreName, sizeof(curSemphore->semdata->name) - 1);
    cprintf("Set name\n");
    curSemphore->semdata->lock = 0;
    cprintf("Set lock and returning\n");

    return *curSemphore;

}
struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName)
{
    //TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
    //COMMENT THE FOLLOWING LINE BEFORE START CODING
//    panic("get_semaphore is not implemented yet");
    //Your Code is Here...
    struct semaphore *ret = sget(ownerEnvID, semaphoreName);
    return *ret;

}

void wait_semaphore(struct semaphore sem)
{
    //TODO: [PROJECT'24.MS3 - #04] [2] USER-LEVEL SEMAPHORE - wait_semaphore
    //COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("wait_semaphore is not implemented yet");
    sys_wait_semaphore(&sem);
    //Your Code is Here...
}

void signal_semaphore(struct semaphore sem)
{
    //TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
    //COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("signal_semaphore is not implemented yet");
    //Your Code is Here...

    sys_signal_semaphore(&sem);
}

int semaphore_count(struct semaphore sem)
{
    return sem.semdata->count;
}
