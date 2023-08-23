#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> 
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "IQDataStore.h"

#define SHARE_MEMORY_SIZE 1024*1024*1024 // 1GB

IQDataStore::IQDataStore()
{
	m_shmaddr = NULL;
//	sem_Wait = NULL;
//	sem_Done = NULL;
}

IQDataStore::~IQDataStore()
{

}



int IQDataStore::MakeSharedMemory()
{
	// Try to Open Share Memory
	size_t shm_size = SHARE_MEMORY_SIZE + sizeof(SharedMsgBlock);

    if((m_shmid = shmget((key_t)27676, 0, 0)) == -1)
    {
        key_t key = 27676;
    	if((m_shmid = shmget((key_t)key, shm_size, IPC_CREAT| IPC_EXCL | 0666)) == -1) {
        	m_shmid = shmget((key_t)key, shm_size, IPC_CREAT| 0666);
 			if(m_shmid == -1) {
            	perror("Shared memory create fail");
            	return 1;
        	}
        	else {
            	shmctl(m_shmid, IPC_RMID, 0);
            	m_shmid = shmget((key_t)key, shm_size, IPC_CREAT| 0666);
            	if(m_shmid == -1) {
                	perror("Shared memory create fail");
                	return 1;
            	}
        	}		
    	}
    }

    if((m_shmaddr = (char *)shmat(m_shmid, (void *)0, 0)) == (char *)-1)
    {
        perror("Shmat failed");
    }

//	printf("ShareMemory ID (%d)\n", m_shmid);
    SharedMsgBlock *pMsgShared = (SharedMsgBlock *)m_shmaddr;
    pMsgShared->read_byte = 12345;
    pMsgShared->write_byte = 54321;
		
	return 0;
}

int IQDataStore::ReleaseSharedMemory()
{
	if (m_shmaddr)
	{
		SharedMsgBlock *MsgShared = (SharedMsgBlock *)m_shmaddr;

		sem_destroy(&MsgShared->sem_Wait);
		sem_destroy(&MsgShared->sem_Done);	

		if (shmdt(m_shmaddr) == -1)
		{
			perror("Shmdt failed");
		}
	}
	
	return 0;
}

int IQDataStore::PrepareSemaphore()
{
	if (m_shmaddr)
	{
		SharedMsgBlock *MsgShared = (SharedMsgBlock *)m_shmaddr;

		sem_init(&MsgShared->sem_Wait, 1, 0);
		sem_init(&MsgShared->sem_Done, 1, 0);
	}	

	return 0;
}